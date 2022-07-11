#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTreeWidgetItem>
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum PayloadTypeValue
    {
        RoutingActivationRequest = 0x0005,
        RoutingActivationResponse = 0x0006,
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getIp(void);
    quint16 getPort(void);

    friend void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private slots:
    void on_clientButton_clicked();
    void on_pushButton_clicked();
    void on_pushButton_send_clicked();

    void on_treeWidget_doipConsole_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_action_settings_trigger(void);
private:
    Ui::MainWindow *ui;
    settings *ui_set;
    QString m_adddress;
    quint16 m_port;
    QTcpSocket *socket;
    QByteArray m_sendBuffr;
    QString m_log;

};
#endif // MAINWINDOW_H
