/*
 * @FilePath     : \xjzer\mainwindow.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-09 17:43:51
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QMetaEnum>
#include <QNetworkProxy>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->ServerAddress->setEditable(true);
    socket = new QTcpSocket(this);
    ui->ServerAddress->setEditText("192.168.11.197");
    ui->ServerPort->setText("13400");
    ui->pushButton_send->setText(tr("发送"));
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
    if (this->socket->state() == QAbstractSocket::ConnectedState && this->socket->isValid()) {
        socket->write(m_sendBuffr);
        qDebug() << "write success";
    } else {
        qDebug() << "write error";
    }
}
