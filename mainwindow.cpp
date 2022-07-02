#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ServerAddress->setEditable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    this->m_adddress = ui->ServerAddress->currentText();
    this->m_port = ui->ServerPort->text();
    std::cout << "address = " <<this->m_adddress.toStdString() << std::endl;
    std::cout << "port = " <<this->m_port.toStdString() << std::endl;
}

