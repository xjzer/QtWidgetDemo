/*
 * @FilePath     : \xjzer\settings.h
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-11 23:48:34
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-13 00:24:43
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class settings;
}

class settings : public QDialog {
    Q_OBJECT

  public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

  private:
    Ui::settings *ui;
    QSettings *m_settings;
};

#endif // SETTINGS_H
