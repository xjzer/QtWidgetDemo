/*
 * @FilePath     : \xjzer\main.cpp
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-03 14:32:16
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-13 00:24:02
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

    log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " ";

    switch (type) {
    case QtDebugMsg:
        log << "(Debug):";
        break;
    case QtWarningMsg:
        log << "(Warning):";
        break;
    case QtCriticalMsg:
        log << "(Critical):";
        break;
    case QtFatalMsg:
        log << "(Fatal):";
        break;
    case QtInfoMsg:
        log << "(Info):";
        break;
    default:
        log << "(unknown):";
        break;
    }
#if defined(QT_MESSAGELOGCONTEXT)
    log << msg << " " << context.file << ":" << context.line << " " << context.function << Qt::endl;
#elif defined(QT_NO_MESSAGELOGCONTEXT)
    log << msg << Qt::endl;
#elif
#error "No macro definition found"
#endif

    fprintf(stderr, "%s", text.toLocal8Bit().constData());
    fflush(stderr);
}
