/*
 * @FilePath     : \xjzer\mainwindow.h
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2024-04-17 21:31:12
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"
#include <QMainWindow>
#include <QRegularExpression>
#include <QTcpSocket>
#include <QTextBrowser>
#include <QTreeWidgetItem>
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
        UDS_MSG                 = 0x8001,
        UDS_ACK                 = 0x8002,
        UDS_NACK                = 0x8003,
        CUSTOM_AUTOTEST         = 0xFF01,
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getIp(void);
    quint16 getPort(void);

    friend void myMessageOutput(QtMsgType type, const QMessageLogContext &context,
                                const QString &msg);
    static QTextBrowser *ms_log_browser;
  private slots:
    void on_pushButton_uds_send_clicked();

    void on_treeWidget_doipConsole_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void slot_socket_bytesWritten(qint64 bytes);
    void slot_errorOccurred(QAbstractSocket::SocketError socketError);
    void slot_socket_ready_read();
    void slot_action_settings_trigger();
    void slot_action_insert_triggered(bool checked);
    void slot_action_delete_triggered(bool checked);
    void slot_3E_timeout(void);
    void slot_timeout_100ms(void);
    void slot_disconnected(void);
    void slot_socket_readChannelFinished(void);

    void on_action_connect_triggered();

    void on_treeWidget_doipConsole_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_custom_clicked();

    void on_pushButton_clearlog_clicked();

    void on_actionAbout_DoIPConsole_triggered();

private:

    void on_autotest_clicked();
    bool send_uds_message(const QByteArray reqData, const QByteArray respData, bool isWhole);
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
    QByteArray m_autotestRespCheckData;
    QAction *m_action_insert;
    QAction *m_action_delete;
    QTreeWidgetItem *m_CurItem;
    QTimer *m_3E_timer;
    QTimer *m_timer_100ms;
    QTimer *m_reconnect_timer;
    QByteArray m_Uds27Seed;
    QByteArray m_Uds27Key;
    quint32 m_seedSize;
    const int m_MagicNumButtonClicked = 0xFEFE;
    quint32 m_autotest_group_index;
    bool m_autotestIsWhole;
    bool m_autotestIsEnd = true;
};

#endif // MAINWINDOW_H
