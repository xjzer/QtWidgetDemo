/*
 * @FilePath     : \xjzer\mainwindow.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-08-10 23:19:26
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settings.h"
#include "ui_settings.h"
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFlags>
#include <QInputDialog>
#include <QLibrary>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QMetaEnum>
#include <QNetworkProxy>
#include <QProcess>
#include <QRegularExpression>
#include <QTimer>
QTextBrowser *MainWindow::ms_log_browser = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), window_set(new settings(this)) {
    ui->setupUi(this);

    m_tcpSocket     = new QTcpSocket(this);
    m_action_insert = new QAction("insert");
    m_action_delete = new QAction("delete");
    m_timer         = new QTimer(this);
    m_timer_100ms   = new QTimer(this);

    ui->treeWidget_doipConsole->header()->setSectionResizeMode(
        QHeaderView::Stretch); // treeWidget列宽自适应

    QObject::connect(ui->action_settings, SIGNAL(triggered()), this,
                     SLOT(slot_action_settings_trigger()));
    QObject::connect(this->m_tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_socket_ready_read()));
    QObject::connect(this->m_tcpSocket, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    QObject::connect(this->m_tcpSocket, SIGNAL(bytesWritten(qint64)), this,
                     SLOT(slot_socket_bytesWritten(qint64)));

    QObject::connect(this->m_tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this,
                     SLOT(slot_errorOccurred(QAbstractSocket::SocketError)));

    QObject::connect(this->m_action_insert, SIGNAL(triggered(bool)), this,
                     SLOT(slot_action_insert_triggered(bool)));
    QObject::connect(this->m_action_delete, SIGNAL(triggered(bool)), this,
                     SLOT(slot_action_delete_triggered(bool)));
    QObject::connect(this->m_timer, SIGNAL(timeout()), this, SLOT(slot_timeout()));
    QObject::connect(this->m_timer_100ms, SIGNAL(timeout()), this, SLOT(slot_timeout_100ms()));
    QObject::connect(ui->lineEdit_custom_uds, SIGNAL(returnPressed()), ui->pushButton_uds_send,
                     SIGNAL(clicked()), Qt::UniqueConnection); // todo  UniqueConnection
    QObject::connect(ui->lineEdit_custom, SIGNAL(returnPressed()), ui->pushButton_custom,
                     SIGNAL(clicked()), Qt::UniqueConnection); // todo  UniqueConnection
    ui_set = window_set->ui;
    m_QRegExp.setPattern("\\s");
    MainWindow::ms_log_browser = ui->textBrowser;

    //支持右键菜单
    ui->treeWidget_doipConsole->setContextMenuPolicy(Qt::CustomContextMenu);

    // MDI
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    ui->mdiArea->setActiveSubWindow(ui->mdiArea->subWindowList().front());

    //打开timer
    m_timer_100ms->start(100);

    //
    ui->treeWidget_doipConsole->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->treeWidget_doipConsole->setColumnWidth(0, 300);
}
MainWindow::~MainWindow() {
    delete ui;
}

QString MainWindow::getIp() {
    return m_adddress;
}

quint16 MainWindow::getPort() {
    return m_port;
}

void MainWindow::on_pushButton_clicked() {
    //    QMetaEnum metaColor = QMetaEnum::fromType<QTcpSocket::SocketState>();
    //    ui->textBrowser->setText(metaColor.valueToKey(socket->state()));
    qDebug() << m_tcpSocket->state();
    qDebug() << m_tcpSocket->isValid();
}

void MainWindow::on_pushButton_uds_send_clicked() {
    QTreeWidgetItem *iItem = nullptr;
    QList<QTreeWidgetItem *> iItemList;
    iItem = new QTreeWidgetItem;
    iItem->setText(0, ui->lineEdit_custom_uds->text());
    iItem->setHidden(true);

    iItemList = ui->treeWidget_doipConsole->findItems("8001", Qt::MatchContains);
    if (iItemList.count() == 1) {
        iItemList.front()->addChild(iItem);
    }
    emit ui->treeWidget_doipConsole->itemDoubleClicked(iItem, m_MagicNumButtonClicked);
}

void MainWindow::on_treeWidget_doipConsole_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    bool ok;

    DoIPProtocol iDoip;
    QTreeWidgetItem *iItemParent = item;

    quint16 iSourceAddress =
        ui_set->lineEdit_tester->text().remove(m_QRegExp).first(4).toUInt(&ok, 16);
    quint16 iTargetFunc = iDoip.mUdsReq.mTarget =
        ui_set->lineEdit_functional->text().remove(m_QRegExp).first(4).toUInt(&ok, 16);
    quint16 iTargetPhy = iDoip.mUdsReq.mTarget =
        ui_set->lineEdit_physical->text().remove(m_QRegExp).first(4).toUInt(&ok, 16);

    QDataStream iHeaderStream(&m_sendHeader, QDataStream::Append | QDataStream::ReadWrite);
    QDataStream iDataStream(&m_sendData, QDataStream::Append | QDataStream::ReadWrite);

    iHeaderStream.setVersion(QDataStream::Qt_6_4);

    //获取顶层节点
    while (iItemParent->parent()) {
        iItemParent = iItemParent->parent();
    }

    iDoip.mHeader.mVersion = ui_set->comboBox_version->currentText().first(2).toUInt(&ok, 16);
    iDoip.mHeader.mInverseVersion = ~iDoip.mHeader.mVersion;
    iDoip.mHeader.mType           = iItemParent->text(0).first(4).toUInt(&ok, 16);

    if (!ok) {
        qWarning() << "get payloadType failed";
    }

    // set doip protocol header
    iHeaderStream << iDoip.mHeader.mVersion;
    iHeaderStream << iDoip.mHeader.mInverseVersion;
    iHeaderStream << iDoip.mHeader.mType;

    switch (iDoip.mHeader.mType) {
    case ROUTING_ACTIVATION_REQ:
        iDoip.mRoutingReq.mSource = iSourceAddress;
        iDoip.mRoutingReq.mType =
            ui_set->comboBox_activation_type->currentText().first(2).toUInt(&ok, 16);
        iDoip.mRoutingReq.mISO =
            ui_set->lineEdit_reserved_iso->text().remove(m_QRegExp).toUInt(&ok, 16);
        iDoip.mRoutingReq.mOEM =
            ui_set->lineEdit_reserved_oem->text().remove(m_QRegExp).toUInt(&ok, 16);
        iDataStream << iDoip.mRoutingReq.mSource;
        iDataStream << iDoip.mRoutingReq.mType;
        iDataStream << iDoip.mRoutingReq.mISO;
        if (ui_set->checkBox_reserved_oem->isChecked()) {
            iDataStream << iDoip.mRoutingReq.mOEM;
        }
        break;
    case UDS_MSG:
        iDoip.mUdsReq.mSource = iSourceAddress;
        iDoip.mUdsReq.mTarget = iTargetPhy;
        iDataStream << iDoip.mUdsReq.mSource;
        iDataStream << iDoip.mUdsReq.mTarget;
        m_sendData = m_sendData + QByteArray::fromHex(item->text(0).toLocal8Bit());
        if (column == m_MagicNumButtonClicked) {
            delete item;
        }
        break;
    default:
        qDebug() << item->text(1).toStdU16String();
        break;
    }

    if (m_sendHeader.first(4).last(2).toHex().toUInt(&ok, 16) == UDS_MSG &&
        m_sendData.at(4) == 0x27) {   //判断是否为发送27服务请求
        if (!m_Uds27Seed.isEmpty()) { //判断是否已经请求过种子
            QFile aFile("uds27key");
            aFile.open(QIODevice::ReadOnly);
            QString readKey = aFile.readAll();
            m_sendData      = m_sendData + QByteArray::fromHex(readKey.toLocal8Bit());
            m_Uds27Seed.clear(); //种子每次使用完毕后丢弃
        } else {
            qDebug() << "m_Uds27Seed.isEmpty() " << m_Uds27Seed.isEmpty();
        }
    }

    if (this->m_tcpSocket->state() == QAbstractSocket::ConnectedState &&
        this->m_tcpSocket->isValid()) {
        // set payload length
        iDoip.mHeader.mLength = m_sendData.size();
        iHeaderStream << iDoip.mHeader.mLength;
        m_tcpSocket->write(m_sendHeader + m_sendData);
        //设置不接受鼠标事件(todo bug)
        ui->treeWidget_doipConsole->setDisabled(true);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); //鼠标转圈

        //等待write
        if (!m_tcpSocket->waitForBytesWritten(500)) {
            qInfo().noquote() <<"REQ" <<"write error" << m_sendHeader.toHex(' ').toUpper() << "|"
                              << m_sendData.toHex(' ').toUpper();
        }
    } else {
        this->m_timer->stop();
        m_sendHeader.clear();
        m_sendData.clear();
        qDebug() << "on_treeWidget_doipConsole_itemDoubleClicked clear";
        qDebug() << m_tcpSocket->state() << m_tcpSocket->isValid();
    }
}

void MainWindow::slot_socket_bytesWritten(qint64 bytes) {
    bool ok;

    //不打印3E服务，可以设置为配置项
    if (m_sendHeader.first(4).last(2).toHex().toUInt(&ok, 16) == UDS_MSG &&
        m_sendData.at(4) == 0x3E) {

    } else {
        qInfo().noquote() << "REQ" << m_sendHeader.toHex(' ').toUpper() << "|"
                          << m_sendData.toHex(' ').toUpper();
    }
    m_sendHeader.clear();
    m_sendData.clear();
    if (!m_tcpSocket->waitForReadyRead(3000)) {
        qDebug() << "timeout_ready_read";
    }

    ui->treeWidget_doipConsole->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void MainWindow::slot_errorOccurred(QAbstractSocket::SocketError socketError) {
    qDebug() << "ERRNO =" << socketError;
    switch (socketError) {
    default:
        QMessageBox::information(
            this, tr("error"),
            tr("ERRNO(%1) : ").arg(m_tcpSocket->error())+(m_tcpSocket->errorString().toLocal8Bit()));
        break;
    }
}

void MainWindow::slot_socket_ready_read() {
    bool ok;
    m_recvHeader     = m_tcpSocket->read(8);
    auto iDataLength = m_recvHeader.toHex().last(8).toUInt(&ok, 16);
    m_recvData       = m_tcpSocket->read(iDataLength);

    if ((m_recvHeader.first(4).last(2).toHex().toUInt(&ok, 16) ==
             UDS_ACK && //不打印3E服务，可以设置为配置项
         m_recvData.at(5) == 0x3E) ||
        (m_recvHeader.first(4).last(2).toHex().toUInt(&ok, 16) == UDS_MSG &&
         m_recvData.at(4) == 0x7E)) {

    } else {
        qInfo().noquote() << "RES" << m_recvHeader.toHex(' ').toUpper() << "|"
                          << m_recvData.toHex(' ').toUpper();
    }

    //    qDebug() <<"========" << m_recvHeader.first(4).last(2).toHex().toUInt(&ok, 16)
    //    <<(quint32)m_recvData.at(4);

    m_seedSize = ui_set->spinBox_seedSize->value();
    if (m_recvHeader.first(4).last(2).toHex().toUInt(&ok, 16) == UDS_MSG &&
        m_recvData.at(4) == 0x50) { //判断收到的是否为：UDS消息 && 10服务的肯定响应
        if (ui_set->checkBox_uds_3e->isChecked()) { //如果设置3E自动触发
            m_timer->start(3000);
        }
    } else if (m_recvHeader.first(4).last(2).toHex().toUInt(&ok, 16) == UDS_MSG &&
               m_recvData.at(4) == 0x67 &&
               m_recvData.size() ==
                   6 + m_seedSize) //判断收到的是否为：UDS消息 && 27服务的响应 && 带有种子
    {
        m_Uds27Seed = m_recvData.last(4);
        QStringList arguments;
        if (m_recvData.at(5) == ui_set->spinBox_dll_1->value()) {
            arguments << ui_set->comboBox_dll_1->currentText();
        } else if (m_recvData.at(5) == ui_set->spinBox_dll_2->value()) {
            arguments << ui_set->comboBox_dll_2->currentText();
        } else if (m_recvData.at(5) == ui_set->spinBox_dll_3->value()) {
            arguments << ui_set->comboBox_dll_3->currentText();
        } else {
            QMessageBox::information(NULL, "提示", tr("未找到对应安全等级"), QMessageBox::Ok,
                                     QMessageBox::Ok);
            return;
        }
        QFileInfo gen_key_file(ui_set->comboBox_genkey->currentText());
        if (!gen_key_file.isFile()) {
            QMessageBox::information(NULL, "提示", tr("dll文件路径为空/错误"), QMessageBox::Ok,
                                     QMessageBox::Ok);
            return;
        }
        arguments << m_Uds27Seed.toHex();
        // qDebug() << "exe = " << ui_set->comboBox_genkey->currentText() << "arg = " << arguments;
        QProcess::execute(ui_set->comboBox_genkey->currentText(), arguments);
    }

    m_recvHeader.clear();
    m_recvData.clear();
}

void MainWindow::slot_action_settings_trigger(void) {
    this->window_set->show();
    this->window_set->tab_setting_load(this->ui_set->tab_setting->currentIndex());
}

void MainWindow::slot_action_insert_triggered(bool checked) {
    bool ok = false;
    QTreeWidgetItem *iItem;
    QString iItemText;
    iItemText = QInputDialog::getMultiLineText(this, "DoipPayloadInput", "", "22 F1 95", &ok);
    qDebug() << iItemText;
    QStringList iItemTextList = iItemText.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (auto &c : iItemTextList) {
        iItem = new QTreeWidgetItem;
        iItem->setText(0, c);
        m_CurItem->addChild(iItem);
    }
}

void MainWindow::slot_action_delete_triggered(bool checked) {
    bool ok;
    if (m_CurItem->parent()) { //一级节点不能删除
        if (m_CurItem->parent()->text(0).first(4).toUInt(&ok, 16) !=
            UDS_MSG) { // UDS二级节点不能删除
            delete m_CurItem;
        }
    }
}

void MainWindow::slot_timeout() {
    QTreeWidgetItemIterator it(ui->treeWidget_doipConsole);
    while (*it) {
        int iRet = (*it)->text(0).compare("3E 80", Qt::CaseSensitive); //遍历寻找3E 80 item
        if (iRet == 0) {
            break;
        }
        ++it;
    }

    if (ui_set->checkBox_uds_3e->isChecked()) {
        //如果设置3E自动触发
        emit ui->treeWidget_doipConsole->itemDoubleClicked(*it, 0); //发送信号，类似双击3E 80
    } else {
        m_timer->stop();
    }
}

void MainWindow::slot_timeout_100ms() {
    switch (m_tcpSocket->state()) {
    case QAbstractSocket::UnconnectedState:
        ui->action_connect->setText(tr("连接"));
        break;
    case QAbstractSocket::HostLookupState:
        ui->action_connect->setText(tr("HostLookupState"));
        break;
    case QAbstractSocket::ConnectingState:
        ui->action_connect->setText(tr("连接中"));
        break;
    case QAbstractSocket::ConnectedState:
        ui->action_connect->setText(tr("断开连接"));
        break;
    case QAbstractSocket::BoundState:
        ui->action_connect->setText(tr("BoundState"));
        break;
    case QAbstractSocket::ListeningState:
        ui->action_connect->setText(tr("断开连接"));
        break;
    case QAbstractSocket::ClosingState:
        ui->action_connect->setText(tr("连接"));
        break;
    default:
        break;
    }
}

void MainWindow::slot_disconnected() {
    ui->action_connect->setText(tr("连接"));
}

void MainWindow::on_action_connect_triggered() {

    ui->textBrowser->clear();
    if (ui->action_connect->text() == tr("连接")) {
        ui->action_connect->setEnabled(false);

        qApp->processEvents();
        auto tabIndex    = ui_set->tab_setting->indexOf(ui_set->tab_address);
        this->m_adddress = ui_set->lineEdit_ip->text();
        this->m_port     = ui_set->lineEdit_port->text().toInt();
        qDebug() << "address=" << this->m_adddress << "port=" << this->m_port;

        ui->action_connect->setText(tr("连接中"));

        //建立连接
        m_tcpSocket->setProxy(QNetworkProxy::NoProxy);
        m_tcpSocket->connectToHost(this->m_adddress, this->m_port);

        auto rConnect = m_tcpSocket->waitForConnected(1000); // block

        if (rConnect) {
            qDebug() << m_tcpSocket->socketType()
                     << "connect success, state =" << m_tcpSocket->state();
            ui->action_connect->setText("断开连接");
            ui->treeWidget_doipConsole->setEnabled(true);
        } else {
            qDebug() << "Connection failed, error =" << m_tcpSocket->error()
                     << "state =" << m_tcpSocket->state();
            ui->action_connect->setText("连接");
        }
    } else if (ui->action_connect->text() == tr("断开连接")) {
        //断开连接
        m_tcpSocket->disconnectFromHost();
        //修改按键文字
        ui->action_connect->setText("连接");
        m_timer->stop();
        m_Uds27Seed.clear();
    }

    ui->action_connect->setEnabled(true);
}

void MainWindow::on_treeWidget_doipConsole_customContextMenuRequested(const QPoint &pos) {
    QMenu *iPopMenu;
    QAction *add_parent_node;
    m_CurItem = ui->treeWidget_doipConsole->itemAt(pos); //获取当前被点击的节点
    if (m_CurItem == NULL)
        return; //在空白位置右击
    QTreeWidgetItem *iItemParent = m_CurItem;
    while (iItemParent->parent()) {
        iItemParent = iItemParent->parent(); //获取顶层节点
    }
    bool ok;

    iPopMenu = new QMenu(this);           //定义一个右键弹出菜单
    iPopMenu->addAction(m_action_insert); //往菜单内添加QAction
    iPopMenu->addAction(m_action_delete); //往菜单内添加QAction
    iPopMenu->exec(QCursor::pos());       //弹出右键菜单，菜单位置为光标位置
}

void MainWindow::on_pushButton_custom_clicked() {
    m_sendData = QByteArray::fromHex(ui->lineEdit_custom->text().toLocal8Bit());
    if (this->m_tcpSocket->state() == QAbstractSocket::ConnectedState &&
        this->m_tcpSocket->isValid()) {

        m_tcpSocket->write(m_sendData);

        //设置不接受鼠标事件(todo bug)
        ui->treeWidget_doipConsole->setDisabled(true);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); //鼠标转圈
        qApp->processEvents();

        //等待write
        if (!m_tcpSocket->waitForBytesWritten(500)) {
        }
    } else {
        this->m_timer->stop();
        m_sendData.clear();
        qDebug() << m_tcpSocket->state() << m_tcpSocket->isValid();
    }
}
