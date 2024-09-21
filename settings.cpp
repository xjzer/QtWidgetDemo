/*
 * @FilePath     : \xjzer\settings.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2024-04-18 01:34:17
 */
#include "settings.h"
#include "ui_settings.h"
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDesktopServices>

settings::settings(QWidget *parent) : QDialog(parent), ui(new Ui::settings) {
    ui->setupUi(this);
    qDebug() << "settings";
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

//    handle_setting_tab_address(LOAD);
//    handle_setting_tab_uds(LOAD);
//    handle_setting_tab_payload_item(LOAD);
//    handle_setting_tab_autotest_item(LOAD);

    for (qint32 i = ui->tab_setting->count() - 1; i >= 0; i--) {
        tab_setting_load(i); //第一次运行时，自动将ini中的配置加载到setting窗口
    }
}

settings::~settings() {
    delete ui;
}

static void clicked_pushButton_select_file(QWidget *parent, QComboBox *comboBox,
                                           const QString &filter = QString()) {
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(
                                                     parent, QObject::tr("Open File"), QDir::currentPath(), filter));
    if (!directory.isEmpty()) {
        if (comboBox->findText(directory) == -1)
            comboBox->addItem(directory);
        comboBox->setCurrentIndex(comboBox->findText(directory));
    }
}

void settings::on_pushButton_odx_clicked() {
    clicked_pushButton_select_file(this, ui->comboBox_odx, QString("Odx (*.odx-d);;ALL files (*)"));
}

void settings::on_pushButton_dll_clicked() {
    clicked_pushButton_select_file(this, ui->comboBox_dll_1, QString("Odx (*.dll);;ALL files (*)"));
}

void settings::restore_default_tab_address() {
    ui->lineEdit_tester->setText("0E80");
    ui->lineEdit_ip->setText("0.0.0.0");
    ui->lineEdit_port->setText("13400");
    ui->lineEdit_physical->setText("0000");
    ui->lineEdit_functional->setText("0000");
}

void settings::restore_default_tab_uds() {
    ui->checkBox_uds_3e->setChecked(true);
    ui->comboBox_genkey->setCurrentText(".\\GenKey\\gen_key.exe");
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

void settings::settings_handle(SettingsHandle handle, QLabel *label, QSpinBox *spinBox) {
    switch (handle) {
    case LOAD:
        spinBox->setValue(m_settings->value(label->text()).toInt());
        break;
    case SAVE:
        m_settings->setValue(label->text(), spinBox->value());
        break;
    default:
        break;
    }
}

void settings::settings_handle(SettingsHandle handle, QString key, QSpinBox *spinBox)
{
    switch (handle) {
    case LOAD:
        spinBox->setValue(m_settings->value(key).toInt());
        break;
    case SAVE:
        m_settings->setValue(key, spinBox->value());
        break;
    default:
        break;
    }
}

void settings::handle_setting_tab_address(SettingsHandle handle) {
    m_settings->beginGroup(ui->tab_setting->tabText(ui->tab_setting->indexOf(ui->tab_address)));
    settings_handle(handle, ui->label_tester, ui->lineEdit_tester);
    settings_handle(handle, ui->label_ip, ui->lineEdit_ip);
    settings_handle(handle, ui->label_port, ui->lineEdit_port);
    settings_handle(handle, ui->label_physical, ui->lineEdit_physical);
    settings_handle(handle, ui->label_functional, ui->lineEdit_functional);
    m_settings->endGroup();
}

void settings::handle_setting_tab_uds(SettingsHandle handle) {
    m_settings->beginGroup(ui->tab_setting->tabText(ui->tab_setting->indexOf(ui->tab_uds)));
    settings_handle(handle, ui->label_odx, ui->comboBox_odx);

    settings_handle(handle, ui->label_dll_1, ui->comboBox_dll_1);

    settings_handle(handle, ui->label_genkey, ui->comboBox_genkey);
    settings_handle(handle, ui->checkBox_uds_3e);

    m_settings->endGroup();
}

void settings::handle_setting_tab_payload_item(SettingsHandle handle) {
    m_settings->beginGroup(
                ui->tab_setting->tabText(ui->tab_setting->indexOf(ui->tab_payload_item)));
    settings_handle(handle, ui->label_version, ui->comboBox_version);
    settings_handle(handle, ui->label_activation_type, ui->comboBox_activation_type);
    settings_handle(handle, ui->label_reserved_iso, ui->lineEdit_reserved_iso);
    settings_handle(handle, ui->checkBox_reserved_oem, ui->lineEdit_reserved_oem);
    m_settings->endGroup();
}

void settings::handle_setting_tab_autotest_item(SettingsHandle handle) {
    m_settings->beginGroup(
                ui->tab_setting->tabText(ui->tab_setting->indexOf(ui->tab_Autotest)));
    settings_handle(handle, ui->label_autotest_file, ui->comboBox_autotest_file);
    m_settings->endGroup();
}

void settings::on_buttonBox_clicked(QAbstractButton *button) {
    const int currentIndex = ui->tab_setting->currentIndex();

    if (button == static_cast<QAbstractButton *>(ui->buttonBox->button(QDialogButtonBox::Save))) {

        handle_setting_tab_address(SAVE);
        handle_setting_tab_uds(SAVE);
        handle_setting_tab_payload_item(SAVE);
        handle_setting_tab_autotest_item(SAVE);

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
}

void settings::tab_setting_load(int index) {
    qDebug() << "tab_setting_load index" << index;
    if (index == ui->tab_setting->indexOf(ui->tab_address)) {
        handle_setting_tab_address(LOAD);
    }
    if (index == ui->tab_setting->indexOf(ui->tab_uds)) {
        handle_setting_tab_uds(LOAD);
    }
    if (index == ui->tab_setting->indexOf(ui->tab_payload_item)) {
        handle_setting_tab_payload_item(LOAD);
    }
    if (index == ui->tab_setting->indexOf(ui->tab_Autotest)) {
        handle_setting_tab_autotest_item(LOAD);
    }
}

void settings::on_pushButton_genkey_clicked() {
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(
                                                     this, tr("Open File"), QDir::currentPath(), QString("exe (*.exe);;ALL files (*)")));

    if (!directory.isEmpty()) {
        if (ui->comboBox_genkey->findText(directory) == -1)
            ui->comboBox_genkey->addItem(directory);
        ui->comboBox_genkey->setCurrentIndex(ui->comboBox_genkey->findText(directory));
    }
}

void settings::on_pushButton_autotest_file_clicked()
{
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(
                                                     this, tr("Open File"), QDir::currentPath(), QString("ini (*.ini);;ALL files (*)")));

    if (!directory.isEmpty()) {
        if (ui->comboBox_autotest_file->findText(directory) == -1)
            ui->comboBox_autotest_file->addItem(directory);
        ui->comboBox_autotest_file->setCurrentIndex(ui->comboBox_autotest_file->findText(directory));
    }
}


void settings::on_pushButton_create_test_ini_clicked()
{
    QString directory = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(), QString("ini (*.ini);;ALL files (*)")));
    qDebug() << "autotestIniFile" << directory;
    if (!directory.isEmpty()) {
        if (ui->comboBox_autotest_file->findText(directory) == -1)
            ui->comboBox_autotest_file->addItem(directory);
        ui->comboBox_autotest_file->setCurrentIndex(ui->comboBox_autotest_file->findText(directory));
    }
    QSettings *autotestIni = new QSettings(directory, QSettings::IniFormat);
    autotestIni->clear();
    autotestIni->setValue("001/Req", "22 03 03");
    autotestIni->setValue("001/Resp", "62 03 03 30 32 30 32 30 33");
    autotestIni->setValue("001/delayTime", "100");
    autotestIni->setValue("001/isTest", "true");
    autotestIni->setValue("001/isCheckResp", "true");
    autotestIni->setValue("001/isWhole", "true");

    autotestIni->setValue("002/Req", "10 03");
    autotestIni->setValue("002/Resp", "50 03");
    autotestIni->setValue("002/delayTime", "100");
    autotestIni->setValue("002/isTest", "true");
    autotestIni->setValue("002/isCheckResp", "true");
    autotestIni->setValue("002/isWhole", "false");

    autotestIni->setValue("003/Req", "27 01");
    autotestIni->setValue("003/Resp", "67 01");
    autotestIni->setValue("003/delayTime", "100");
    autotestIni->setValue("003/isTest", "true");
    autotestIni->setValue("003/isCheckResp", "true");
    autotestIni->setValue("003/isWhole", "false");

    autotestIni->setValue("004/Req", "27 02");
    autotestIni->setValue("004/Resp", "67 02");
    autotestIni->setValue("004/delayTime", "100");
    autotestIni->setValue("004/isTest", "true");
    autotestIni->setValue("004/isCheckResp", "true");
    autotestIni->setValue("004/isWhole", "true");

    autotestIni->setValue("005/Req", "11 01");
    autotestIni->setValue("005/Resp", "51 01");
    autotestIni->setValue("005/delayTime", "100");
    autotestIni->setValue("005/isTest", "true");
    autotestIni->setValue("005/isCheckResp", "true");
    autotestIni->setValue("005/isWhole", "true");

    autotestIni->setValue("006Reconnect/delayTime", "35000");
    
    autotestIni->setValue("007/Req", "22 03 03");
    autotestIni->setValue("007/Resp", "62 03 03 30 32 30 32 30 33");
    autotestIni->setValue("007/delayTime", "100");
    autotestIni->setValue("007/isTest", "true");
    autotestIni->setValue("007/isCheckResp", "true");
    autotestIni->setValue("007/isWhole", "true");

}


void settings::on_pushButton_create_edit_ini_clicked()
{
    QString filePath = ui->comboBox_autotest_file->currentText();
    QFileInfo directory(filePath);

    //检测ini文件是否存在并打开
    if (directory.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }else{
        QMessageBox::information(NULL, "提示", tr("Autotest.ini 文件路径错误 : %1").arg(directory.filePath()), QMessageBox::Ok,
                                 QMessageBox::Ok);
    }
    return;
}

