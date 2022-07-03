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

private slots:
    void on_clientButton_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString m_adddress;
    quint16 m_port;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
