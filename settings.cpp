/*
 * @FilePath     : \xjzer\settings.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-13 00:24:39
 */
#include "settings.h"
#include "ui_settings.h"
#include <QDir>
#include <QSettings>
#include <QFileDialog>

settings::settings(QWidget *parent) : QDialog(parent), ui(new Ui::settings) {
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("XJZER");
    QCoreApplication::setOrganizationDomain("xjzer.com");
    QCoreApplication::setApplicationName("AutoDoIP");

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "./");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    m_settings = new QSettings();

    qDebug() << "currentPath" << QDir::currentPath();
    qDebug() << "filename = " << m_settings->fileName();
    qDebug() << "format = " << m_settings->format();
    qDebug() << "scope = " << m_settings->scope();
}

settings::~settings() {
    delete ui;
}

void settings::on_pushButton_odx_clicked()
{
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), QString("Odx (*.odx-d);;ALL files (*)")));

    if (!directory.isEmpty()) {
        if (ui->comboBox_odx->findText(directory) == -1)
            ui->comboBox_odx->addItem(directory);
        ui->comboBox_odx->setCurrentIndex(ui->comboBox_odx->findText(directory));
        }
}


void settings::on_pushButton_dll_clicked()
{
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), QString("Odx (*.dll);;ALL files (*)")));

    if (!directory.isEmpty()) {
        if (ui->comboBox_dll->findText(directory) == -1)
            ui->comboBox_dll->addItem(directory);
        ui->comboBox_dll->setCurrentIndex(ui->comboBox_dll->findText(directory));
    }
}

