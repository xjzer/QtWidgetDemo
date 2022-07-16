/*
 * @FilePath     : \xjzer\settings.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-16 13:43:26
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

void settings::settings_handle(SettingsHandle handle, QLabel *label, QLineEdit *line) {
    switch (handle) {
    case LOAD:
        line->setText(m_settings->value(label->text()).toString());
        break;
    case SAVE:
        m_settings->setValue(label->text(), line->text());
        break;
    default:
        break;
    }
}

void settings::settings_handle(SettingsHandle handle, QLabel *label, QComboBox *comboBox) {
    switch (handle) {
    case LOAD:
        if (comboBox->isEditable()) {
            comboBox->setCurrentText(m_settings->value(label->text()).toString());
        } else {
            comboBox->setCurrentIndex(m_settings->value(label->text()).toInt());
        }
        break;
    case SAVE:
        if (comboBox->isEditable()) {
            m_settings->setValue(label->text(), comboBox->currentText());
        } else {
            m_settings->setValue(label->text(), comboBox->currentIndex());
        }
        break;
    default:
        break;
    }
}

void settings::settings_handle(SettingsHandle handle, QCheckBox *checkBox,
                               QLineEdit *line = nullptr) {
    switch (handle) {
    case LOAD:
        checkBox->setChecked(m_settings->value(checkBox->text()).toBool());
        if (line)
            line->setText(m_settings->value(checkBox->text() + "_value").toString());
        break;
    case SAVE:
        m_settings->setValue(checkBox->text(), checkBox->isChecked());
        if (line)
            m_settings->setValue(checkBox->text() + "_value", line->text());
        break;
    default:
        break;
    }
}

void settings::handle_setting_tab_address(SettingsHandle handle) {
    settings_handle(handle, ui->label_tester, ui->lineEdit_tester);
    settings_handle(handle, ui->label_ip, ui->lineEdit_ip);
    settings_handle(handle, ui->label_port, ui->lineEdit_port);
    settings_handle(handle, ui->label_physical, ui->lineEdit_physical);
    settings_handle(handle, ui->label_functional, ui->lineEdit_functional);
}

void settings::handle_setting_tab_uds(SettingsHandle handle) {
    settings_handle(handle, ui->label_odx, ui->comboBox_odx);
    settings_handle(handle, ui->label_dll, ui->comboBox_dll);
    settings_handle(handle, ui->checkBox_uds_3e);
}

void settings::handle_setting_tab_payload_item(SettingsHandle handle) {
    settings_handle(handle, ui->label_version, ui->comboBox_version);
    settings_handle(handle, ui->label_activation_type, ui->comboBox_activation_type);
    settings_handle(handle, ui->label_reserved_iso, ui->lineEdit_reserved_iso);
    settings_handle(handle, ui->checkBox_reserved_oem, ui->lineEdit_reserved_oem);
}

void settings::on_buttonBox_clicked(QAbstractButton *button) {

    const int currentIndex = ui->tab_setting->currentIndex();
    if (button == static_cast<QAbstractButton *>(ui->buttonBox->button(QDialogButtonBox::Save))) {
        m_settings->beginGroup(ui->tab_setting->tabText(ui->tab_setting->currentIndex()));
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_address)) {
            handle_setting_tab_address(SAVE);
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_uds)) {
            handle_setting_tab_uds(SAVE);
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_payload_item)) {
            handle_setting_tab_payload_item(SAVE);
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_address)) {
        }
        m_settings->endGroup();
    }
}

void settings::on_tab_setting_currentChanged(int index) {
    m_settings->beginGroup(ui->tab_setting->tabText(index));
    if (index == ui->tab_setting->indexOf(ui->tab_address)) {
        handle_setting_tab_address(LOAD);
    }
    if (index == ui->tab_setting->indexOf(ui->tab_uds)) {
        handle_setting_tab_uds(LOAD);
    }
    if (index == ui->tab_setting->indexOf(ui->tab_payload_item)) {
        handle_setting_tab_payload_item(LOAD);
    }
    if (index == ui->tab_setting->indexOf(ui->tab_address)) {
    }
    m_settings->endGroup();
}
