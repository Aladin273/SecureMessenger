#include <QCoreApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    server.runServer(QHostAddress::Any, 2020);

    qDebug() << "//////// SecureMessengerServer started ////////\n";

    return a.exec();
}

