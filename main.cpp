#include <QCoreApplication>
#include <QtDebug>

#include "control/ControlClient.h"
#include "server/Server.h"
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    auto args = a.arguments();
    if (args.count() > 1 && args[1] == "client") {
        qDebug() << "client start";
        ControlClient *control = new ControlClient();
        control->setLoginInfo("hejunqiu", "2X5T0YVW");
        control->start("/tmp/test1/work", "localhost", 10999);
    } else {
        Server *server = new Server;
        server->startListen(10999, "/tmp/test2/work");
    }
    return a.exec();
}
