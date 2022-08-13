/*
 * @FilePath     : \xjzer\settings.h
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-17 00:40:36
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include "type.h"
#include <QAbstractButton>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QSpinBox>

namespace Ui {
class settings;
}
class MainWindow;
class settings : public QDialog {
    Q_OBJECT

  public:
    enum SettingsHandle {
        LOAD = 0, /*load_setting_to_line_via_label*/
        SAVE,     /*save_setting_to_ini_via_label_and_line*/
    };
    explicit settings(QWidget *parent = nullptr);
    friend class MainWindow;
    ~settings();

  private slots:
    void on_pushButton_odx_clicked();

    void on_pushButton_dll_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);


    void on_pushButton_genkey_clicked();

    void on_pushButton_dll_2_clicked();

    void on_pushButton_dll_3_clicked();

private:
    void default_setting(void);
    void restore_default_tab_address(void);
    void restore_default_tab_uds(void);
    void restore_default_tab_payload_item(void);
    void tab_setting_load(int index);

    void settings_handle(SettingsHandle handle, QLabel *label, QLineEdit *line);
    void settings_handle(SettingsHandle handle, QLabel *label, QComboBox *comboBox);
    void settings_handle(SettingsHandle handle, QCheckBox *checkBox, QLineEdit *line);
    void settings_handle(SettingsHandle handle, QLabel *label, QSpinBox * spinBox);
    void settings_handle(SettingsHandle handle, QString key, QSpinBox *spinBox); //spinBox作为value，绑定自定义key

    void handle_setting_tab_address(SettingsHandle handle);
    void handle_setting_tab_uds(SettingsHandle handle);
    void handle_setting_tab_payload_item(SettingsHandle handle);

    Ui::settings *ui;
    QSettings *m_settings;
};

#endif // SETTINGS_H
