/*
 * @FilePath     : \xjzer\mainwindow.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-09 21:47:50
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settings.h"
#include "ui_settings.h"
#include <QDebug>
#include <QMetaEnum>
#include <QNetworkProxy>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), ui_set(new settings(this)){
    ui->setupUi(this);
    ui->ServerAddress->setEditable(true);
    socket = new QTcpSocket(this);
    ui->ServerAddress->setEditText("192.168.11.197");
    ui->ServerPort->setText("13400");
    ui->pushButton_send->setText(tr("发送"));
    ui->treeWidget_doipConsole->header()->setSectionResizeMode(QHeaderView::Stretch); //treeWidget列宽自适应
    QObject::connect(ui->action_settings,SIGNAL(triggered()),this,SLOT(on_action_settings_trigger()));
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

void MainWindow::on_clientButton_clicked() {
    ui->textBrowser->clear();
    if (ui->clientButton->text() == tr("连接")) {
        ui->clientButton->setEnabled(false);
        qApp->processEvents();

        this->m_adddress = ui->ServerAddress->currentText();
        this->m_port     = ui->ServerPort->text().toInt();
        qDebug() << "address=" << this->m_adddress << "port=" << this->m_port;

        ui->clientButton->setText(tr("连接中"));
        ui->clientButton->repaint();

        //建立连接
        socket->setProxy(QNetworkProxy::NoProxy);
        socket->connectToHost(this->m_adddress, this->m_port);

        auto rConnect = socket->waitForConnected(1000); // block

        if (rConnect) {
            qDebug() << socket->socketType() << "connect success, state =" << socket->state();
            ui->clientButton->setText("断开连接");
        } else {
            qDebug() << "Connection failed, error =" << socket->error()
                     << "state =" << socket->state();
            ui->clientButton->setText("连接");
        }
    } else if (ui->clientButton->text() == tr("断开连接")) {
        //断开连接
        socket->disconnectFromHost();
        //修改按键文字
        ui->clientButton->setText("连接");
    }

    ui->clientButton->setEnabled(true);
}

void MainWindow::on_pushButton_clicked() {
    //    QMetaEnum metaColor = QMetaEnum::fromType<QTcpSocket::SocketState>();
    //    ui->textBrowser->setText(metaColor.valueToKey(socket->state()));
    ui->textBrowser->append(qDebug().toString(socket->state()));
    ui->textBrowser->append(qDebug().toString(socket->isValid()));
}

void MainWindow::on_pushButton_send_clicked() {
    QTextStream log(&m_log);
    //    UnconnectedState,
    //    HostLookupState,
    //    ConnectingState,
    //    ConnectedState,
    //    BoundState,
    //    ListeningState,
    //    ClosingState
    quint8 buf[] = {0x02, 0xFD, 0x00, 0x05, 0x00, 0x00, 0x00, 0x07, 0x0E, 0x80,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (this->socket->state() == QAbstractSocket::ConnectedState && this->socket->isValid()) {
        socket->write((const char *)buf, sizeof(buf)-1);
        qDebug() << "write success";
    } else {
        qDebug() << "write error";
    }

}


void MainWindow::on_treeWidget_doipConsole_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
//    item->setCheckState(0,Qt::Unchecked);
    auto str = item->text(1).toLocal8Bit().begin();
    auto payloadTypeValue = std::strtoul(str, &str, 16);
    qDebug() << "payloadTypeValue = "<<payloadTypeValue;
    quint8 buf[] = {0x02, 0xFD, 0x00, 0x05, 0x00, 0x00, 0x00, 0x07, 0x0E, 0x80,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


    //
    QComboBox *frame=new QComboBox();

    switch (payloadTypeValue) {
    case RoutingActivationRequest:
        frame->addItem("Deafult");
        frame->addItem("Diagnostic communication required by regulation");
        frame->addItem("Central security");
        frame->addItem("Available for additional VM-specific use");
        break;
    default:
        qDebug() <<     item->text(1).toStdU16String();
        break;
    }

    if(frame->count()){
        ui->treeWidget_doipConsole->setItemWidget(item->child(0),1,frame);
    }

    if (this->socket->state() == QAbstractSocket::ConnectedState && this->socket->isValid()) {
        socket->write((const char *)buf, sizeof(buf)-1);
    }
    if (socket->waitForBytesWritten()) {
        qDebug() << "write success!!!";
    }


}

void MainWindow::on_action_settings_trigger(void)
{
    this->ui_set->show();
}
