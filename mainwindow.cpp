/*
 * @FilePath     : \xjzer\mainwindow.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-17 10:22:35
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settings.h"
#include "ui_settings.h"
#include <QDataStream>
#include <QDebug>
#include <QFlags>
#include <QLibraryInfo>
#include <QMetaEnum>
#include <QNetworkProxy>
#include <QRegularExpression>

QTextBrowser *MainWindow::ms_log_browser = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), window_set(new settings(this)) {
    ui->setupUi(this);
    m_tcpSocket = new QTcpSocket(this);
    ui->pushButton_send->setText(tr("发送"));
    ui->treeWidget_doipConsole->header()->setSectionResizeMode(
        QHeaderView::Stretch); // treeWidget列宽自适应
    QObject::connect(ui->action_settings, SIGNAL(triggered()), this,
                     SLOT(slot_action_settings_trigger()));
    ui_set = window_set->ui;
    m_QRegExp.setPattern("\\s");
    MainWindow::ms_log_browser = ui->textBrowser;
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
    ui->textBrowser->append(qDebug().toString(m_tcpSocket->state()));
    ui->textBrowser->append(qDebug().toString(m_tcpSocket->isValid()));
}

void MainWindow::on_pushButton_send_clicked() {
    //    UnconnectedState,
    //    HostLookupState,
    //    ConnectingState,
    //    ConnectedState,
    //    BoundState,
    //    ListeningState,
    //    ClosingState
    quint8 buf[] = {0x02, 0xFD, 0x00, 0x05, 0x00, 0x00, 0x00, 0x07, 0x0E, 0x80,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (this->m_tcpSocket->state() == QAbstractSocket::ConnectedState &&
        this->m_tcpSocket->isValid()) {
        m_tcpSocket->write((const char *)buf, sizeof(buf) - 1);
        qDebug() << "write success";
    } else {
        qDebug() << "write error";
    }
}

void MainWindow::on_treeWidget_doipConsole_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    //    item->setCheckState(0,Qt::Unchecked);
    bool ok;
    QByteArray iSendHeader;
    QByteArray iSendData;
    QByteArray iRecvHeader;
    QByteArray iRecvData;
    DoIPProtocol iDoip;

    QDataStream iHeaderStream(&iSendHeader, QDataStream::Append | QDataStream::ReadWrite);
    QDataStream iDataStream(&iSendData, QDataStream::Append | QDataStream::ReadWrite);
    iHeaderStream.setVersion(QDataStream::Qt_6_4);

    iDoip.mSourceAddress   = ui_set->lineEdit_tester->text().first(4).toUInt(&ok, 16);
    iDoip.mHeader.mVersion = ui_set->comboBox_version->currentText().first(2).toUInt(&ok, 16);
    iDoip.mHeader.mInverseVersion = ~iDoip.mHeader.mVersion;
    iDoip.mHeader.mType           = item->text(0).first(4).toUInt(&ok, 16);

    if (!ok) {
        qWarning() << "get payloadType failed";
    }

    // set doip protocol header
    iHeaderStream << iDoip.mHeader.mVersion;
    iHeaderStream << iDoip.mHeader.mInverseVersion;
    iHeaderStream << iDoip.mHeader.mType;
    // set doip protocol data
    iDataStream << iDoip.mSourceAddress;

    //    quint8 buf[] = {0x02, 0xFD, 0x00, 0x05, 0x00, 0x00, 0x00, 0x07, 0x0E, 0x80,
    //                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    switch (iDoip.mHeader.mType) {
    case RoutingActivationRequest:
        iDoip.mRoutingReq.mType =
            ui_set->comboBox_activation_type->currentText().first(2).toUInt(&ok, 16);
        iDoip.mRoutingReq.mISO =
            ui_set->lineEdit_reserved_iso->text().remove(m_QRegExp).toUInt(&ok, 16);
        iDoip.mRoutingReq.mOEM =
            ui_set->lineEdit_reserved_oem->text().remove(m_QRegExp).toUInt(&ok, 16);
        iDataStream << iDoip.mRoutingReq.mType;
        iDataStream << iDoip.mRoutingReq.mISO;
        if (ui_set->checkBox_reserved_oem->isChecked()) {
            iDataStream << iDoip.mRoutingReq.mOEM;
        }
        break;
    default:
        qDebug() << item->text(1).toStdU16String();
        break;
    }

    iDoip.mHeader.mLength = iSendData.size();

    iHeaderStream << iDoip.mHeader.mLength;

    //    iSendHeader.resize(8); //DoIP header size = 8
    //    iSendData.resize(iDoip.mHeader.mLength);

    if (this->m_tcpSocket->state() == QAbstractSocket::ConnectedState &&
        this->m_tcpSocket->isValid()) {
        m_tcpSocket->write(iSendHeader + iSendData);
    }
    if (m_tcpSocket->waitForBytesWritten()) {
        qInfo().noquote() << iSendHeader.toHex(' ') << "|" << iSendData.toHex(' ');
    }
    if (m_tcpSocket->waitForReadyRead()) {
        iRecvHeader      = m_tcpSocket->read(8);
        auto iDataLength = iRecvHeader.toHex().last(8).toUInt(&ok, 16);
        iRecvData        = m_tcpSocket->read(iDataLength);
        qInfo().noquote() << iRecvHeader.toHex(' ') << "|" << iRecvData.toHex(' ');
    }
}

void MainWindow::slot_action_settings_trigger(void) {
    this->window_set->show();
    this->window_set->on_tab_setting_currentChanged(this->ui_set->tab_setting->currentIndex());
}

void MainWindow::on_action_connect_triggered() {

    ui->textBrowser->clear();
    if (ui->action_connect->text() == tr("连接")) {
        ui->action_connect->setEnabled(false);
        qApp->processEvents();
        auto tabIndex = ui_set->tab_setting->indexOf(ui_set->tab_address);
        window_set->m_settings->beginGroup(ui_set->tab_setting->tabText(tabIndex));
        this->m_adddress = ui_set->lineEdit_ip->text();
        this->m_port     = ui_set->lineEdit_port->text().toInt();
        window_set->m_settings->endGroup();
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
    }

    ui->action_connect->setEnabled(true);
}
