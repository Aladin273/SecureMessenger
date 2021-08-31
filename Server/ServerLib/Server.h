#ifndef SERVER_H
#define SERVER_H

#include "pch.h"
#include "CommandsTypes.h"

class Server : public QObject
{
    Q_OBJECT

private:
QString mServerIdentifier = "SERVER";
qint64 mCurrentServerCommand = 0;
QMap<QString, QTcpSocket*> mActiveConnections;
QSet<QTcpSocket*> mUnconnectedUsers;
std::unique_ptr<QTcpServer> mServer;
QVector<Command> mUsersCommands;
QVector<std::pair<QTcpSocket*, Command>> mServerCommands;
QMap <QTcpSocket*, DataPortion> mPortionDataFromSocket;

QTimer* mUpdateReadCommandsVec = nullptr;
QTimer* mSendServerCommandsTimer = nullptr;
QTimer* mTryToReadNextPortion = nullptr;

private:
QTcpSocket* getSocketByLogin(const QString& login);

Command& receiveCommand(const QByteArray& data, QTcpSocket *sender);
void writeCommand(Command& command, QTcpSocket* sender);
void onReadCommandEnd(const Command& command, QTcpSocket* sender);

void startCheckReadCommandTimer();
void startSendServerDataTimer();
void startCheckPortionDataTimer();

void performServerCommand(const Command& command, QTcpSocket* sender);

void updateReadCommandsVec();

void tryToRegister(const Command& command, QTcpSocket *sender);
void tryToIdentify(const Command& command, QTcpSocket *sender);
void giveOnlineUsers(const Command& command, QTcpSocket *sender);
void giveAllUsers(const Command& command, QTcpSocket *sender);

void onNewUserConnected(const QString& login);
void onUserConnected(const QString& login);
void onUserDisconnected(const QString& login);

bool needToParseSender(CommandsToServer command);

void tryToReadData(QTcpSocket* socket, QByteArray& arrayData);
void tryToReadDataPortions();

QString getNextServerCommandIdentifier();

private slots:
void onReadyRead();
void onNewConnection();
void onDisconnect();
void writeServerCommands();

public:
    Server(QObject* parent = nullptr);
    virtual ~Server();

    bool runServer(QHostAddress hostToListen, qint16 portToListen );

    void stopServer();
};

#endif // SERVER_H
