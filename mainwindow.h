#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getIp(void);
    quint16 getPort(void);

    friend void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);


private slots:
    void on_clientButton_clicked();
    void on_pushButton_clicked();
    void on_pushButton_send_clicked();

private:
    Ui::MainWindow *ui;
    QString m_adddress;
    quint16 m_port;
    QTcpSocket *socket;
    QByteArray m_sendBuffr;
    QString m_log;
};
#endif // MAINWINDOW_H
