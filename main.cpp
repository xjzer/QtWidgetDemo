/*
 * @FilePath     : \xjzer\main.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-17 00:40:43
 */
#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QMessageLogContext>
#include <QTranslator>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "xjzer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    qInstallMessageHandler(myMessageOutput);
    MainWindow w;

    w.show();

    return a.exec();
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString text;
    QTextStream log(&text);
    FILE *fp = stdout;
    log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " ";

    switch (type) {
    case QtDebugMsg:
        log << "(DEBUG) : ";
        break;
    case QtWarningMsg:
        log << "(WARN)  : ";
        fp = stderr;
        break;
    case QtCriticalMsg:
        log << "(Critical) : ";
        break;
    case QtFatalMsg:
        log << "(FATAL) : ";
        break;
    case QtInfoMsg:
        log << "(INFO)  : ";
        break;
    default:
        log << "(unknown) : ";
        break;
    }
#if defined(QT_MESSAGELOGCONTEXT)
    log << msg << " " << context.file << ":" << context.line << " " << context.function;
#elif defined(QT_NO_MESSAGELOGCONTEXT)
    log << msg;
#elif
#error "No macro definition found"
#endif
    if(MainWindow::ms_log_browser != nullptr){
        auto iPreColor = MainWindow::ms_log_browser->textColor();
        if(text.contains("REQ", Qt::CaseSensitive)){
            MainWindow::ms_log_browser->setTextColor(QColor("Green"));
        }else if(text.contains("RES", Qt::CaseSensitive)){
            MainWindow::ms_log_browser->setTextColor(QColor("Purple"));
        }
        MainWindow::ms_log_browser->append(text);
        MainWindow::ms_log_browser->setTextColor(iPreColor);
    }else{
        log << Qt::endl;
        fprintf(fp, "%s",
                text.toLocal8Bit().constData()); // or fprintf(stderr, "%s", qPrintable(text));
    }
    fflush(fp);
}
