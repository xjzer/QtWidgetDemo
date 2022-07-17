/*
 * @FilePath     : \xjzer\mainwindow.h
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-17 00:40:30
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"
#include <QMainWindow>
#include <QRegularExpression>
#include <QTcpSocket>
#include <QTreeWidgetItem>
#include <QTextBrowser>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class settings;
} // namespace Ui
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    enum PayloadTypeValue : quint16 {
        ROUTING_ACTIVATION_REQ  = 0x0005,
        ROUTING_ACTIVATION_RESP = 0x0006,
        UDS_REQ = 0X8001,
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getIp(void);
    quint16 getPort(void);

    friend void myMessageOutput(QtMsgType type, const QMessageLogContext &context,
                                const QString &msg);
    static QTextBrowser *ms_log_browser;
  private slots:
    void on_pushButton_clicked();
    void on_pushButton_send_clicked();

    void on_treeWidget_doipConsole_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void slot_socket_bytesWritten(qint64 bytes);
    void slot_socket_ready_read();
    void slot_action_settings_trigger();

    void on_action_connect_triggered();


private:
    Ui::MainWindow *ui;
    Ui::settings *ui_set;
    settings *window_set;
    QString m_adddress;
    quint16 m_port;
    QTcpSocket *m_tcpSocket;
    QSettings *m_settings;
    QRegularExpression m_QRegExp;
    QByteArray m_sendHeader;
    QByteArray m_sendData;
    QByteArray m_recvHeader;
    QByteArray m_recvData;
};

#endif // MAINWINDOW_H
