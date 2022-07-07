/*
 * @FilePath     : \xjzer\mainwindow.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-03 17:04:35
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMetaEnum>
#include <QNetworkProxy>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->ServerAddress->setEditable(true);
    socket = new QTcpSocket(this);
    ui->ServerAddress->setEditText("192.168.11.197");
    ui->ServerPort->setText("13400");
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
    QString _log;
    QTextStream log(&_log) ;
    if (ui->clientButton->text() == tr("连接")) {
        ui->clientButton->setEnabled(false);
        qApp->processEvents();

        this->m_adddress = ui->ServerAddress->currentText();
        this->m_port     = ui->ServerPort->text().toInt();
        log << "address = "<<this->m_adddress<<"port = " << this->m_port;
        ui->textBrowser->append(log.readAll());

        ui->clientButton->setText(tr("连接中"));
        ui->clientButton->repaint();

        //建立连接
        socket->setProxy(QNetworkProxy::NoProxy);
        socket->connectToHost(this->m_adddress,this->m_port);

        auto rConnect = socket->waitForConnected(1000); // block

        if (rConnect) {
            log << "Connect successfully" << Qt::endl;
            log << "state = " << qDebug().toString(socket->state()) << Qt::endl;
            log << "socketype=" << qDebug().toString(socket->socketType());
            ui->textBrowser->append(log.readAll());
            ui->clientButton->setText("断开连接");
        } else {
            log << "Connection failed , error = " << qDebug().toString(socket->error()) << " state = " << qDebug().toString(socket->state());
            ui->textBrowser->append(log.readAll());
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

void MainWindow::on_pushButton_clicked()
{
//    QMetaEnum metaColor = QMetaEnum::fromType<QTcpSocket::SocketState>();
//    ui->textBrowser->setText(metaColor.valueToKey(socket->state()));
    ui->textBrowser->append(qDebug().toString(socket->state()));
    ui->textBrowser->append(qDebug().toString(socket->isValid()));
}

void MainWindow::on_pushButton_2_clicked()
{
//    UnconnectedState,
//    HostLookupState,
//    ConnectingState,
//    ConnectedState,
//    BoundState,
//    ListeningState,
//    ClosingState
    if(this->socket->state() == QAbstractSocket::ConnectedState && this->socket->isValid()){
        socket->write();
    }
}

