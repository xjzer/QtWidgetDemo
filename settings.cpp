/*
 * @FilePath     : \xjzer\settings.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-15 01:11:11
 */
#include "settings.h"
#include "ui_settings.h"
#include <QDir>
#include <QFileDialog>
#include <QSettings>

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
    default_setting();
}

QVariant settings::get_value(QAnyStringView key)
{
    return m_settings->value(key);
}

settings::~settings() {
    delete ui;
}

void settings::on_pushButton_odx_clicked() {
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::currentPath(), QString("Odx (*.odx-d);;ALL files (*)")));

    if (!directory.isEmpty()) {
        if (ui->comboBox_odx->findText(directory) == -1)
            ui->comboBox_odx->addItem(directory);
        ui->comboBox_odx->setCurrentIndex(ui->comboBox_odx->findText(directory));
    }
}

void settings::on_pushButton_dll_clicked() {
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::currentPath(), QString("Odx (*.dll);;ALL files (*)")));

    if (!directory.isEmpty()) {
        if (ui->comboBox_dll->findText(directory) == -1)
            ui->comboBox_dll->addItem(directory);
        ui->comboBox_dll->setCurrentIndex(ui->comboBox_dll->findText(directory));
    }
}

void settings::default_setting() {
    m_settings->beginGroup(ui->groupBox_Upper->title());
    m_settings->setValue(ui->label_tester->text().toUtf8(), 0E80);
    m_settings->endGroup();

    m_settings->beginGroup(ui->groupBox_lower->title());
    m_settings->setValue(ui->label_ip->text(), "");
    m_settings->setValue(ui->label_port->text(), 13400);
    m_settings->setValue(ui->label_physical->text(), "");
    m_settings->setValue(ui->label_functional->text(), "");
    m_settings->endGroup();

    auto keys = m_settings->allKeys();
    for (int i = 0; i < keys.size(); ++i) {
        qDebug() << keys.at(i).toLocal8Bit().constData();
    }
}

void settings::on_buttonBox_clicked(QAbstractButton *button) {

    const int currentIndex = ui->tab_setting->currentIndex();

    if (button == static_cast<QAbstractButton *>(ui->buttonBox->button(QDialogButtonBox::Save))) {
        qDebug() << ui->tab_setting->currentIndex();

        if (currentIndex == ui->tab_setting->indexOf(ui->tab_address)) {
            m_settings->beginGroup(ui->groupBox_Upper->title());
            m_settings->setValue(ui->label_tester->text().toUtf8(), ui->lineEdit_tester->text());
            m_settings->endGroup();
            m_settings->beginGroup(ui->groupBox_lower->title());
            m_settings->setValue(ui->label_ip->text(),ui->lineEdit_ip->text());
            m_settings->setValue(ui->label_port->text(), ui->lineEdit_port->text());
            m_settings->setValue(ui->label_physical->text(), ui->lineEdit_physical->text());
            m_settings->setValue(ui->label_functional->text(), ui->lineEdit_functional->text());
            m_settings->endGroup();
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_uds)) {
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_payload_item)) {
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_address)) {
        }
    }
}

void settings::on_tab_setting_currentChanged(int index) {
    qDebug() << index;
}
