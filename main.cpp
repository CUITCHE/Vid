#include <QCoreApplication>
#include <QtDebug>
#include "control/ControlClient.h"
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>
#include <QHostInfo>
#include <QTextStream>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include "settings/Configure.h"
#include "git/Git.h"


void start_client(const QString &host, const QString &path, uint16_t port, bool strict);
void testLogin();
void _main();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Vid");
    QCoreApplication::setOrganizationName("Anonymous");
    QCoreApplication::setApplicationVersion("1.0.0");
    Git git("/Users/hejunqiu/Documents/QtProjects/Vid3/");
    QStringList dirs, files;
    git.allFiles(&dirs, &files);
    qDebug() << dirs;
    qDebug() << files;
    exit(0);
#ifdef PRODUCT
    _main();
#else
    testLogin();
#endif
    return a.exec();
}


void start_client(const QString &host, const QString &path, uint16_t port, bool strict) {
    ControlClient *control = new ControlClient();
    auto name = QString("[%1]-%2").arg(QHostInfo::localHostName(), QStandardPaths::writableLocation(QStandardPaths::HomeLocation).section("/", -1));
    qDebug("Welcome, %s ! Please Enter THE TOKEN which server given: ", name.toStdString().c_str()) ;

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= static_cast<ulong>(~ICANON);
    newt.c_lflag &= static_cast<ulong>(~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    QString token;
    int c = 0;
    fflush(stdin);
    while ((c = getchar()) != '\n') {
        token.append(QChar(static_cast<uchar>(c)));
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    control->setLoginInfo(name, token);
    control->start(path, host, port, strict);
}


void _main() {
    auto &cf = *Configure::shared();
    auto server = cf.section("server");
    auto host = server.get("host", "localhost").toString();
    auto port = server.get("port", 9010).toUInt();

    auto local = cf.section("local");
    auto path = local.get("path").toString();
    auto strict = local.get("strict").toBool();

    start_client(host, path, port, strict);
}


void testLogin() {
    qDebug() << "client start";
    ControlClient *control = new ControlClient();
    control->setLoginInfo("hejunqiu", "");
    control->start("/tmp/test1/work", "localhost", 9010, false);}
