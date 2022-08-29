#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QCoreApplication>

#include "pch.h"
#include "CommandsTypes.h"
#include "clientdatabase.h"

class ClientSocket : public QObject
{
    Q_OBJECT

private:
    int mNumOfCurrentCommand = 1;

    const QString mDefaultDownloadingFolder = QCoreApplication::applicationDirPath()+"/files/";

    std::unique_ptr<QTcpSocket> mSocket;
    QString mUserName;

    std::unique_ptr<QTimer> mTimerTosendData;
    std::unique_ptr<QTimer> mTimerToDeleteReceivedCommands;
    std::unique_ptr<QTimer> mTimerToCheckPortionData;

    std::shared_ptr<ClientDataBase> mDataBasePtr;

    QVector<Command> mReceivingCommands;
    QVector<Command> mSendingCommands;
    QVector<SendingFileStruct> mSendingFiles;
    QVector<SendingFileStruct> mReceivingFiles;
    DataPortion mPortionDataFromServer;


private:
    QString generateCommandIdentifier();

    void onFullCommandDataReceived(const Command& command);

    void sendMessageHistory(QString receiver);

    void readCommand(const QByteArray& data);
    void tryToReadData(QByteArray& arrayData);
    void tryToReadDataPortions();

    bool needToParseSender(CommandsToClient command);

    void startSendingCommandsTimer();
    void startCheckReceivingCommandsTimer();
    void startCheckPortionDataTimer();

    void onRegisterFailCommandEnd(const Command& command);
    void onAutentifyFailCommandEnd(const Command& command);
    void onUserConnectCommandEnd(const Command& command);
    void onNewUserConnectCommandEnd(const Command& command);
    void onUserDisconnectCommandEnd(const Command& command);
    void onGetMessageHistoryCommandEnd(const Command& command);
    void onSendMessageHistoryCommandEnd(const Command& command);
    void onSendMessageCommandEnd(const Command& command);
    void onAllUsersCommandEnd(const Command& command);
    void onOnlineUsersCommandEnd(const Command& command);
    void onFileReceivedCommandEnd(const Command& command);
    void onFileGetErrorCommandEnd(const Command& command);
    void onTryToGetFileCommandEnd(const Command& command);//TODO

    void sendCommandOnFileGetError(const QString& fileRequester, const QString& fileName);

    void trySendFile(const QString& fileRequester, const QString& fileName);//TODO

    void tryToReadCommandDataIntoFile(const QString& cmdIdent, const QString& fileOwner, const QByteArray& data);

    void onConnectSuccess();
    void onConnectedFail();
    void onDisconnect();

private slots:
    void onReadyRead();
    void sendNextPortionData();
    void checkForUnusedReceivedCommands();

public: //public slots: проверю сначала без потока
    void connectToHost(const QString &hostName, quint16 port); //tested
    void disconnectFromHost(); //tested
    void sendMessage(const SendMessageStruct& message, QString receiver); //tested
    void tryToRegister(const QString& userName, const QString& passHash); //tested
    void tryToAutentify(const QString& userName, const QString& passHash); //tested
    void tryToGetMessageHistory(const QString& userName); //tested
    void tryToGetOnlineUsers(); //tested
    void tryToGetAllUsers();  //tested
    void tryToGetFile(const QString& userName, const QString& fileName);//TODO

    void setDataBase(std::shared_ptr<ClientDataBase> db); //tested

public:
    ClientSocket(QObject *parent = nullptr);

signals:
    void onServerResponse(ServerCommandResponce responce); //tested

    void onReceivedMessage(const QString& sender, const SendMessageStruct& message); //tested
    void onReceivedMessageHistory(const QString& sender, const QVector<SendMessageStruct>& messages);//tested

    void onFileDownloaded(const QString& fileOwner, const QString& fileName); //not tested
    void onFileAlreadyDownloading(const QString& fileOwner, const QString& fileName);//not tested
    void onFileDownloadingError(const QString& fileOwner, const QString& fileName);//not tested

    void onGetAllUsers(const QVector<QString>& users); //tested
    void onGetOnlineUsers(const QVector<QString>& users); //tested

    void onNewUserConnected(const QString& user); //tested
    void onUserConnected(const QString& user); //tested
    void onUserDisconnected(const QString& user); //incorrect, try test with thread
};

#endif // CLIENTSOCKET_H
