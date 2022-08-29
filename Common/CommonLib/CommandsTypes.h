#ifndef COMMANDSTYPES_H
#define COMMANDSTYPES_H

#include "pch.h"

constexpr int updateCommandsIntervalMs = 5000;
constexpr int sendServerCommandsIntervalMs = 1;
constexpr int maxBytesPerSending = 1024 * 16;
extern const char* key;

enum class CommandsToServer
{
    NONE=0,
    REGISTER,
    IDENTIFY,
    GET_MESSAGE_HISTORY,
    SEND_MESSAGE,
    SEND_MESSAGE_HISTORY,
    GET_ONLINE_USERS,
    GET_ALL_USERS,
    GET_FILE,
    SEND_FILE,
    FILE_GETTING_ERROR
};

enum class CommandsToClient
{
    NONE=0,
    ON_REGISTER_SUCCESS,
    ON_REGISTER_FAIL,
    ON_IDENTIFY_SUCCESS,
    ON_IDENTIFY_FAIL,
    ON_USER_CONNECT,
    ON_NEW_USER,
    ON_USER_DISCONNECT,
    GET_MESSAGE_HISTORY,
    SEND_MESSAGE_HISTORY,
    SEND_MESSAGE,
    ALL_USERS,
    ONLINE_USERS,
    GET_FILE,
    SEND_FILE,
    FILE_GETTING_ERROR
};

enum class CommandErrors
{
    NONE=0,
    USER_EXIST,
    INCORRECT_DATA
};

enum class ServerCommandResponce
{
    NONE=0,
    REG_FAIL_USER_EXIST,
    DISCONNECTED,
    CONNECT_SUCCESS,
    CONNECT_FAIL,
    REGISTER_SUCCESS,
    AUTENTIFY_SUCCESS,
    AUTENTIFY_FAIL
};

struct Command
{
    QString cmdIdentifier;
    qint16 cmdId;
    qint64 needBytes;
    qint64 gottenBytes;
    QTime lastUpdateTime;
    QString target;
    QString sender;
    QVector<QByteArray> data;
    bool sendedFirstTime;

    Command()
        :cmdId(0)
        , needBytes(0)
        , gottenBytes(0)
        , sendedFirstTime(false)
    {
    };
};

struct DataPortion
{
    quint64 blockSize = 0;
    QTime lastUpdateTime;
    QByteArray unsortedData;
};

struct SendMessageStruct
{
    qint64 datetime;
    QString content;
    bool file;
};

struct SendingFileStruct
{
    QString fileName;
    QString owner;
    std::shared_ptr<QFile> file;
    QString cmdIdentifier;
    QTime lastUpdateTime;
};

bool isCommandToServer(CommandsToServer cmd);

CommandsToClient getCommandToClient(CommandsToServer command);

QString getUnconflictedNewFileName(const QString& fileName);

void encodeDecode(QByteArray& data);

#endif // COMMANDSTYPES_H
