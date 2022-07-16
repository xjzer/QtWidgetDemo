/*
 * @FilePath     : \xjzer\settings.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-17 00:40:33
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

    handle_setting_tab_address(LOAD);
    handle_setting_tab_uds(LOAD);
    handle_setting_tab_payload_item(LOAD);

    for (qint32 i = ui->tab_setting->count() - 1; i >= 0; i--) {
        on_tab_setting_currentChanged(i); //第一次运行时，自动将ini中的配置加载到setting窗口
    }
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
void settings::restore_default_tab_address() {
    ui->lineEdit_tester->setText("0x0E80");
    ui->lineEdit_ip->setText("0.0.0.0");
    ui->lineEdit_port->setText("13400");
    ui->lineEdit_physical->setText("0x0000");
    ui->lineEdit_functional->setText("0x0000");
}

void settings::restore_default_tab_uds() {
    ui->checkBox_uds_3e->setChecked(true);
}

void settings::restore_default_tab_payload_item() {
    ui->comboBox_version->setCurrentIndex(1);
    ui->comboBox_activation_type->setCurrentIndex(0);
    ui->lineEdit_reserved_iso->setText("00 00 00 00");
    ui->lineEdit_reserved_oem->setText("00 00 00 00");
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
    m_settings->beginGroup(ui->tab_setting->tabText(ui->tab_setting->currentIndex()));
    if (button == static_cast<QAbstractButton *>(ui->buttonBox->button(QDialogButtonBox::Save))) {

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
    } else if (button == static_cast<QAbstractButton *>(
                             ui->buttonBox->button(QDialogButtonBox::RestoreDefaults))) {
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_address)) {
            restore_default_tab_address();
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_uds)) {
            restore_default_tab_uds();
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_payload_item)) {
            restore_default_tab_payload_item();
        }
        if (currentIndex == ui->tab_setting->indexOf(ui->tab_address)) {
        }
    }
    m_settings->endGroup();
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
