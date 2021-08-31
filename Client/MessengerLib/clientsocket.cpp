#include "clientsocket.h"

ClientSocket::ClientSocket(QObject* parent)
    :QObject(parent)
{
    mSocket.reset(new QTcpSocket());

    connect(mSocket.get(), &QTcpSocket::connected, this, &ClientSocket::onConnectSuccess);
    connect(mSocket.get(), &QTcpSocket::disconnected, this, &ClientSocket::onDisconnect);
    connect(mSocket.get(), &QTcpSocket::readyRead, this, &ClientSocket::onReadyRead);
    connect(mSocket.get(), &QTcpSocket::errorOccurred, this, &ClientSocket::onConnectedFail);
}

void ClientSocket::connectToHost(const QString &hostName, quint16 port)
{
    if(mSocket)
    {
        mSocket->connectToHost(hostName, port);
    }
}

void ClientSocket::disconnectFromHost()
{
    mSocket->disconnectFromHost();
}

void ClientSocket::tryToRegister(const QString &userName, const QString &passHash)
{
    mUserName = userName;

    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::REGISTER);
    command.cmdIdentifier = generateCommandIdentifier();

    QByteArray byteArray;

    byteArray = userName.toUtf8();

    command.needBytes+=byteArray.size();

    command.data.push_back(byteArray);

    byteArray = passHash.toUtf8();

    command.needBytes+=byteArray.size();

    command.data.push_back(byteArray);

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::tryToAutentify(const QString &userName, const QString &passHash)
{
    mUserName = userName;

    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::IDENTIFY);
    command.cmdIdentifier = generateCommandIdentifier();

    QByteArray byteArray;

    byteArray = userName.toUtf8();

    command.needBytes+=byteArray.size();

    command.data.push_back(byteArray);

    byteArray = passHash.toUtf8();

    command.needBytes+=byteArray.size();

    command.data.push_back(byteArray);

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::sendMessage(const SendMessageStruct &message, QString receiver)
{
    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::SEND_MESSAGE);
    command.cmdIdentifier = generateCommandIdentifier();
    command.target = receiver;
    command.sender = mUserName;

    QByteArray byteArray;

    QDataStream stream(&byteArray, QIODevice::WriteOnly);

    stream<<message.datetime<<message.content<<message.file;

    command.data.push_back(byteArray);
    command.needBytes = byteArray.size();

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::tryToGetMessageHistory(const QString &userName)
{
    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::GET_MESSAGE_HISTORY);
    command.cmdIdentifier = generateCommandIdentifier();
    command.target = userName;
    command.sender = mUserName;

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::tryToGetOnlineUsers()
{
    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::GET_ONLINE_USERS);
    command.cmdIdentifier = generateCommandIdentifier();

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::tryToGetAllUsers()
{
    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::GET_ALL_USERS);
    command.cmdIdentifier = generateCommandIdentifier();

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::tryToGetFile(const QString &userName, const QString &fileName)
{
    auto it = std::find_if(mReceivingFiles.begin(), mReceivingFiles.end(), [=](const SendingFileStruct& file)
    {
        return (file.fileName == fileName && file.owner == userName);
    });

    if(it!=mReceivingFiles.end())
    {
        emit onFileAlreadyDownloading(userName, fileName);
    }
    else
    {
        SendingFileStruct fileStruct;
        fileStruct.fileName = fileName;
        fileStruct.owner = userName;
        fileStruct.lastUpdateTime = QTime::currentTime();

        QString newFileName = getUnconflictedNewFileName(mDefaultDownloadingFolder + fileName);

        QFile* fileToOpen = new QFile(newFileName);

        fileStruct.file.reset(fileToOpen);

        if(fileToOpen->open(QIODevice::WriteOnly))
        {
            mReceivingFiles.push_back(fileStruct);

            Command command;

            command.cmdId = static_cast<int>(CommandsToServer::GET_FILE);
            command.cmdIdentifier = generateCommandIdentifier();
            command.target = userName;
            command.sender = mUserName;

            QByteArray byteArray;

            QDataStream stream(&byteArray, QIODevice::WriteOnly);

            stream<<fileName;

            command.data.push_back(byteArray);
            command.needBytes = byteArray.size();

            mSendingCommands.push_back(command);

            startSendingCommandsTimer();
        }
        else
        {
            emit onFileDownloadingError(userName, fileName);
        }
    }
}

QString ClientSocket::generateCommandIdentifier()
{
    QString identifier = mUserName + QString::number(mNumOfCurrentCommand);
    ++mNumOfCurrentCommand;

    return identifier;
}

void ClientSocket::onFullCommandDataReceived(const Command &command)
{
    switch(static_cast<CommandsToClient>(command.cmdId))
    {
    case CommandsToClient::ON_REGISTER_SUCCESS:
    {
        emit onServerResponse(ServerCommandResponce::REGISTER_SUCCESS);
        break;
    }
    case CommandsToClient::ON_REGISTER_FAIL:
    {
        onRegisterFailCommandEnd(command);
        break;
    }
    case CommandsToClient::ON_IDENTIFY_SUCCESS:
    {
        emit onServerResponse(ServerCommandResponce::AUTENTIFY_SUCCESS);
        break;
    }
    case CommandsToClient::ON_IDENTIFY_FAIL:
    {
        onAutentifyFailCommandEnd(command);
        break;
    }
    case CommandsToClient::ON_USER_CONNECT:
    {
        onUserConnectCommandEnd(command);
        break;
    }
    case CommandsToClient::ON_NEW_USER:
    {
        onNewUserConnectCommandEnd(command);
        break;
    }
    case CommandsToClient::ON_USER_DISCONNECT:
    {
        onUserDisconnectCommandEnd(command);
        break;
    }
    case CommandsToClient::GET_MESSAGE_HISTORY:
    {
        onGetMessageHistoryCommandEnd(command);
        break;
    }
    case CommandsToClient::SEND_MESSAGE_HISTORY:
    {
        onSendMessageHistoryCommandEnd(command);
        break;
    }
    case CommandsToClient::SEND_MESSAGE:
    {
        onSendMessageCommandEnd(command);
        break;
    }
    case CommandsToClient::ALL_USERS:
    {
        onAllUsersCommandEnd(command);
        break;
    }
    case CommandsToClient::ONLINE_USERS:
    {
        onOnlineUsersCommandEnd(command);
        break;
    }
    case CommandsToClient::GET_FILE:
    {
        onTryToGetFileCommandEnd(command);
        break;
    }
    case CommandsToClient::SEND_FILE:
    {
        onFileReceivedCommandEnd(command);
        break;
    }
    case CommandsToClient::FILE_GETTING_ERROR:
    {
        onFileGetErrorCommandEnd(command);
        break;
    }
    default: break;
    }
}

void ClientSocket::sendMessageHistory(QString receiver)
{
    QVector<SendMessageStruct> messages;
    QVector<Message> vec_messages;

    mDataBasePtr->getMessagesByNames(vec_messages, mUserName, receiver);

    for(auto &temp : vec_messages)
    {
        if(temp.sender == mUserName)
        {
            messages.push_back(SendMessageStruct {static_cast<qint64>(temp.datetime), temp.content, temp.file});
        }
    }

    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::SEND_MESSAGE_HISTORY);
    command.cmdIdentifier = generateCommandIdentifier();
    command.target = receiver;
    command.sender = mUserName;

    for(auto& message : messages)
    {
        QByteArray byteArray;

        QDataStream stream(&byteArray, QIODevice::WriteOnly);

        stream<<message.datetime<<message.content<<message.file;

        command.data.push_back(byteArray);
        command.needBytes = byteArray.size();
    }

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::readCommand(const QByteArray &data)
{
    Command command;

    QDataStream stream(data);

    QString cmdIdentifier;

    stream>>cmdIdentifier;

    auto it = std::find_if(mReceivingCommands.begin(), mReceivingCommands.end(), [=](const Command& com)
    {
       return com.cmdIdentifier == cmdIdentifier;
    });

    if(it==mReceivingCommands.end())
    {
        command.cmdIdentifier = cmdIdentifier;

        stream>>command.cmdId;

        if(needToParseSender(static_cast<CommandsToClient>(command.cmdId)))
        {
            stream>>command.sender;
        }

        stream>>command.needBytes;

        while(!stream.atEnd())
        {
            QByteArray arr;
            stream>>arr;

            if(static_cast<CommandsToClient>(command.cmdId) == CommandsToClient::SEND_FILE)
            {
                tryToReadCommandDataIntoFile(command.cmdIdentifier, command.sender, arr);
            }
            else
            {
                command.data.push_back(arr);
            }

            command.gottenBytes += arr.size();
        }

        command.lastUpdateTime = QTime::currentTime();

        mReceivingCommands.push_back(command);
    }
    else
    {
        while(!stream.atEnd())
        {
            QByteArray arr;
            stream>>arr;

            if(static_cast<CommandsToClient>(it->cmdId) == CommandsToClient::SEND_FILE)
            {
                tryToReadCommandDataIntoFile(it->cmdIdentifier, it->sender, arr);
            }
            else
            {
                it->data.push_back(arr);
            }

            it->gottenBytes += arr.size();
        }

        it->lastUpdateTime = QTime::currentTime();

        command = *it;
    }

    if(command.needBytes <= command.gottenBytes)
    {
        onFullCommandDataReceived(command);

        it = std::find_if(mReceivingCommands.begin(), mReceivingCommands.end(), [=](const Command& com)
        {
           return com.cmdIdentifier == cmdIdentifier;
        });

        if(it!=mReceivingCommands.end())
        {
            mReceivingCommands.erase(it);
        }
    }

    startCheckReceivingCommandsTimer();
}

void ClientSocket::tryToReadData(QByteArray &arrayData)
{
    if(!mPortionDataFromServer.blockSize && !mPortionDataFromServer.unsortedData.size())
    {
        int quint64Size = sizeof(quint64);

        if(arrayData.size() >= quint64Size)
        {
            QDataStream stream(&arrayData, QIODevice::ReadOnly);

            qint64 blockSize;

            stream>>blockSize;

            if((arrayData.size() - quint64Size) >= blockSize)
            {
                QByteArray data;

                stream>>data;

                encodeDecode(data);

                readCommand(data);

                arrayData.remove(0, blockSize + quint64Size);
            }
        }

        if(arrayData.size())
        {
            mPortionDataFromServer.lastUpdateTime = QTime::currentTime();
            mPortionDataFromServer.unsortedData = arrayData;

            startCheckPortionDataTimer();
        }
    }
    else
    {
        mPortionDataFromServer.lastUpdateTime = QTime::currentTime();
        mPortionDataFromServer.unsortedData+=arrayData;
    }
}

void ClientSocket::tryToReadDataPortions()
{
        int qint64Size = sizeof(quint64);

        if(!mPortionDataFromServer.blockSize && mPortionDataFromServer.unsortedData.size()>=qint64Size)
        {
            QDataStream stream(&mPortionDataFromServer.unsortedData, QIODevice::ReadOnly);
            stream>>mPortionDataFromServer.blockSize;
            mPortionDataFromServer.unsortedData.remove(0, qint64Size);

            mPortionDataFromServer.lastUpdateTime = QTime::currentTime();
        }

        if(mPortionDataFromServer.blockSize && mPortionDataFromServer.unsortedData.size()>=mPortionDataFromServer.blockSize)
        {
            QByteArray data;

            QDataStream stream(&mPortionDataFromServer.unsortedData, QIODevice::ReadOnly);

            stream>>data;

            encodeDecode(data);

            readCommand(data);

            mPortionDataFromServer.unsortedData.remove(0, mPortionDataFromServer.blockSize);

            mPortionDataFromServer.lastUpdateTime = QTime::currentTime();

            mPortionDataFromServer.blockSize = 0;
        }

        QTime currentTime = QTime::currentTime();

        int diff = mPortionDataFromServer.lastUpdateTime.msecsTo(currentTime);

        if(diff >= updateCommandsIntervalMs || diff<0)
        {
            mPortionDataFromServer.blockSize = 0;
            mPortionDataFromServer.unsortedData.clear();
        }

    if(!mPortionDataFromServer.blockSize && !mPortionDataFromServer.unsortedData.size() && mTimerToCheckPortionData)
    {
        mTimerToCheckPortionData->deleteLater();
        mTimerToCheckPortionData.release();
    }
    else
    {
        startCheckPortionDataTimer();
    }
}

bool ClientSocket::needToParseSender(CommandsToClient command)
{
    bool result = false;

    switch(command)
    {
    case CommandsToClient::GET_MESSAGE_HISTORY:
    case CommandsToClient::SEND_MESSAGE:
    case CommandsToClient::SEND_MESSAGE_HISTORY:
    case CommandsToClient::GET_FILE:
    case CommandsToClient::SEND_FILE:
    case CommandsToClient::FILE_GETTING_ERROR:
    {
        result = true;
        break;
    }
        default: break;
    }

    return result;
}

void ClientSocket::startSendingCommandsTimer()
{
    if(!mTimerTosendData)
    {
        mTimerTosendData.reset(new QTimer());

        mTimerTosendData->setSingleShot(true);

        connect(mTimerTosendData.get(), &QTimer::timeout, this, &ClientSocket::sendNextPortionData);

        mTimerTosendData->start(sendServerCommandsIntervalMs);
    }
    else if(mTimerTosendData->remainingTime()<=0)
    {
        mTimerTosendData->start(sendServerCommandsIntervalMs);
    }
}

void ClientSocket::startCheckReceivingCommandsTimer()
{
    if(!mTimerToDeleteReceivedCommands)
    {
        mTimerToDeleteReceivedCommands.reset(new QTimer());

        mTimerToDeleteReceivedCommands->setSingleShot(true);

        connect(mTimerToDeleteReceivedCommands.get(), &QTimer::timeout, this, &ClientSocket::checkForUnusedReceivedCommands);

        mTimerToDeleteReceivedCommands->start(updateCommandsIntervalMs);
    }
    else if(mTimerToDeleteReceivedCommands->remainingTime()<=0)
    {
        mTimerToDeleteReceivedCommands->start(updateCommandsIntervalMs);
    }
}

void ClientSocket::startCheckPortionDataTimer()
{
    if(!mTimerToCheckPortionData)
    {
        mTimerToCheckPortionData.reset(new QTimer());

        mTimerToCheckPortionData->setSingleShot(true);

        connect(mTimerToCheckPortionData.get(), &QTimer::timeout, this, &ClientSocket::tryToReadDataPortions);

        mTimerToCheckPortionData->start(sendServerCommandsIntervalMs);
    }
    else if(mTimerToCheckPortionData->remainingTime()<=0)
    {
        mTimerToCheckPortionData->start(sendServerCommandsIntervalMs);
    }
}

void ClientSocket::onRegisterFailCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);

        qint16 error;
        stream>>error;

        if(static_cast<CommandErrors>(error) == CommandErrors::USER_EXIST)
        {
            emit onServerResponse(ServerCommandResponce::REG_FAIL_USER_EXIST);
        }
    }
}

void ClientSocket::onAutentifyFailCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);

        qint16 error;
        stream>>error;

        if(static_cast<CommandErrors>(error) == CommandErrors::INCORRECT_DATA)
        {
            emit onServerResponse(ServerCommandResponce::AUTENTIFY_FAIL);
        }

    }
}

void ClientSocket::onUserConnectCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);
        QString user;

        stream>>user;
        emit onUserConnected(user);
    }
}

void ClientSocket::onNewUserConnectCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);
        QString user;

        stream>>user;
        emit onNewUserConnected(user);
    }
}

void ClientSocket::onUserDisconnectCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);
        QString user;

        stream>>user;
        emit onUserDisconnected(user);
    }
}

void ClientSocket::onGetMessageHistoryCommandEnd(const Command &command)
{
    sendMessageHistory(command.sender);
}

void ClientSocket::onSendMessageHistoryCommandEnd(const Command &command)
{
    QVector<SendMessageStruct> messages;

    for(auto& message : command.data)
    {
        QDataStream stream(message);

        SendMessageStruct messageStruct;

        stream>>messageStruct.datetime;
        stream>>messageStruct.content;
        stream>>messageStruct.file;

        messages.push_back(messageStruct);
    }

    emit onReceivedMessageHistory(command.sender, messages);
}

void ClientSocket::onSendMessageCommandEnd(const Command &command)
{
    SendMessageStruct messageStruct;

    if(command.data.size())
    {
        QDataStream stream(command.data[0]);

        stream>>messageStruct.datetime;
        stream>>messageStruct.content;
        stream>>messageStruct.file;
    }

    emit onReceivedMessage(command.sender, messageStruct);
}

void ClientSocket::onAllUsersCommandEnd(const Command &command)
{
    QVector<QString> users;

    for(auto& data : command.data)
    {
        QDataStream stream(data);
        QString user;

        stream>>user;

        users.push_back(user);
    }

    emit onGetAllUsers(users);
}

void ClientSocket::onOnlineUsersCommandEnd(const Command &command)
{
    QVector<QString> users;

    for(auto& data : command.data)
    {
        QDataStream stream(data);
        QString user;

        stream>>user;

        users.push_back(user);
    }

    emit onGetOnlineUsers(users);
}

void ClientSocket::onFileReceivedCommandEnd(const Command &command)
{
    auto it = std::find_if(mReceivingFiles.begin(), mReceivingFiles.end(), [&](const SendingFileStruct& file)
    {
        return file.cmdIdentifier == command.cmdIdentifier;
    });

    if(it!=mReceivingFiles.end())
    {
        if(it->file)
        {
            it->file->close();
        }

        mReceivingFiles.erase(it);

        QString fileName = it->fileName;

        emit onFileDownloaded(command.sender, fileName);
    }
}

void ClientSocket::onFileGetErrorCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);
        QString fileName;

        stream>>fileName;

        auto it = std::find_if(mReceivingFiles.begin(), mReceivingFiles.end(), [&](const SendingFileStruct& file)
        {
            return file.fileName == fileName && file.owner == command.sender;
        });

        if(it != mReceivingFiles.end())
        {
            if(it->file)
            {
                it->file->remove();
            }
            mReceivingFiles.erase(it);
        }

        emit onFileDownloadingError(command.sender, fileName);
    }
}

void ClientSocket::onTryToGetFileCommandEnd(const Command &command)
{
    if(command.data.size())
    {
        QDataStream stream(command.data[0]);
        QString fileName;

        stream>>fileName;

        trySendFile(command.sender, fileName);
    }
}

void ClientSocket::sendCommandOnFileGetError(const QString &fileRequester, const QString &fileName)
{
    Command command;

    command.cmdId = static_cast<int>(CommandsToServer::FILE_GETTING_ERROR);
    command.sender = mUserName;
    command.target = fileRequester;
    command.cmdIdentifier = generateCommandIdentifier();

    QByteArray data;

    QDataStream stream(&data, QIODevice::WriteOnly);

    stream<<fileName;

    command.needBytes = data.size();
    command.data.push_back(data);

    mSendingCommands.push_back(command);

    startSendingCommandsTimer();
}

void ClientSocket::trySendFile(const QString& fileRequester, const QString &fileName)
{
    QString cmdIdent = generateCommandIdentifier();

    if(QFile::exists(mDefaultDownloadingFolder + fileName))
    {
        SendingFileStruct fileStruct;
        fileStruct.fileName = fileName;
        fileStruct.owner = mUserName;
        fileStruct.cmdIdentifier = cmdIdent;

        QFile* fileToOpen = new QFile(mDefaultDownloadingFolder + fileName);

        fileStruct.file.reset(fileToOpen);

        if(fileToOpen->open(QIODevice::ReadOnly))
        {
            mSendingFiles.push_back(fileStruct);

            Command command;

            command.cmdId = static_cast<int>(CommandsToServer::SEND_FILE);
            command.cmdIdentifier = cmdIdent;
            command.sender = mUserName;
            command.target = fileRequester;
            command.needBytes = fileToOpen->size();

            QByteArray data;

            QDataStream stream(&data, QIODevice::WriteOnly);

            stream<<fileName;

            command.needBytes += data.size();

            command.data.push_back(data);

            mSendingCommands.push_back(command);

            startSendingCommandsTimer();
        }
        else
        {
            sendCommandOnFileGetError(fileRequester, fileName);
        }
    }
    else
    {
        sendCommandOnFileGetError(fileRequester, fileName);
    }
}

void ClientSocket::tryToReadCommandDataIntoFile(const QString &cmdIdent, const QString &fileOwner, const QByteArray& data)
{
    auto filesIt = std::find_if(mReceivingFiles.begin(), mReceivingFiles.end(), [&](const SendingFileStruct& file)
    {
        return file.cmdIdentifier == cmdIdent;
    });

    if(filesIt == mReceivingFiles.end())
    {
        QDataStream stream(data);

        QString fileName;
        stream>>fileName;

        auto fileNameIt = std::find_if(mReceivingFiles.begin(), mReceivingFiles.end(), [&](const SendingFileStruct& file)
        {
            return file.fileName == fileName && file.owner == fileOwner;
        });

        if(fileNameIt != mReceivingFiles.end())
        {
            fileNameIt->cmdIdentifier = cmdIdent;
            fileNameIt->lastUpdateTime = QTime::currentTime();
        }
    }
    else
    {
        if(filesIt->file)
        {
            filesIt->file->write(data);
        }
        filesIt->lastUpdateTime = QTime::currentTime();
    }
}

void ClientSocket::onConnectSuccess()
{
    emit onServerResponse(ServerCommandResponce::CONNECT_SUCCESS);
}

void ClientSocket::onConnectedFail()
{
    emit onServerResponse(ServerCommandResponce::CONNECT_FAIL);
}

void ClientSocket::onDisconnect()
{
    emit onServerResponse(ServerCommandResponce::DISCONNECTED);
}

void ClientSocket::setDataBase(std::shared_ptr<ClientDataBase> db)
{
    mDataBasePtr = db;
}

void ClientSocket::onReadyRead()
{
    QTcpSocket* socketSender = dynamic_cast<QTcpSocket*>(sender());

    if(socketSender)
    {
        QByteArray receiveBytes = socketSender->readAll();

        tryToReadData(receiveBytes);
    }
}

void ClientSocket::sendNextPortionData()
{
    for (auto it = mSendingCommands.begin(); it != mSendingCommands.end();)
    {
        auto fileIt = std::find_if(mSendingFiles.begin(), mSendingFiles.end(), [&](const SendingFileStruct& file)
        {
           return it->cmdIdentifier == file.cmdIdentifier;
        });

        if(fileIt != mSendingFiles.end())
        {
            if(fileIt->file)
            {
                if(!fileIt->file->atEnd())
                {
                    it->data.push_back(fileIt->file->read(maxBytesPerSending));
                }

                if(fileIt->file->atEnd())
                {
                    mSendingFiles.erase(fileIt);
                }
            }
        }

        int removingBeginParts = 0;
        int currentBytes = 0;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        if(it->sendedFirstTime)
        {
            stream<<it->cmdIdentifier;
        }
        else
        {
            stream<<it->cmdIdentifier<<it->cmdId;

            if(!it->target.isEmpty())
            {
                stream<<it->target;
            }

            if(!it->sender.isEmpty())
            {
                stream<<it->sender;
            }

            stream<<it->needBytes;

            it->sendedFirstTime=true;
        }

        for(auto& dataPart : it->data)
        {
            if(maxBytesPerSending > (currentBytes + dataPart.size()))
            {
                stream<<dataPart;
                currentBytes += dataPart.size();
                ++removingBeginParts;
            }
            else
            {
                if(!currentBytes && it->data.size())
                {
                    stream<<it->data[0];
                    currentBytes += dataPart.size();
                    ++removingBeginParts;
                }

                break;
            }
        }

        for(int i = 0;i<removingBeginParts;i++)
        {
            if(it->data.begin()!=it->data.end())
            {
                it->data.erase(it->data.begin());
            }
        }

        it->gottenBytes+=currentBytes;

        encodeDecode(data);

        if(mSocket)
        {
            QByteArray dataWithSize;
            QDataStream streamForSetSize(&dataWithSize, QIODevice::WriteOnly);

            streamForSetSize<<quint64(0);
            streamForSetSize<<data;
            streamForSetSize.device()->seek(0);
            streamForSetSize<<quint64(dataWithSize.size() - sizeof (quint64));

            mSocket->write(dataWithSize);
        }

        if(it->needBytes <= it->gottenBytes)
        {
            it = mSendingCommands.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if(!mSendingCommands.size() && mTimerTosendData)
    {
        mTimerTosendData->deleteLater();
        mTimerTosendData.release();
    }
    else
    {
        startSendingCommandsTimer();
    }
}

void ClientSocket::checkForUnusedReceivedCommands()
{
    for(int i=0;i<mReceivingCommands.size();i++)
    {
        QTime currentTime = QTime::currentTime();

        int diff = mReceivingCommands[i].lastUpdateTime.msecsTo(currentTime);

        if(diff>=updateCommandsIntervalMs || diff<0)
        {
            mReceivingCommands.erase(mReceivingCommands.begin() + i);
            --i;
        }
    }

    for(int i=0;i<mReceivingFiles.size();i++)
    {
        QTime currentTime = QTime::currentTime();

        int diff = mReceivingFiles[i].lastUpdateTime.msecsTo(currentTime);

        if(diff>=updateCommandsIntervalMs || diff<0)
        {
            if(mReceivingFiles[i].file)
            {
                mReceivingFiles[i].file->close();
                mReceivingFiles[i].file->remove();
            }

            mReceivingFiles.erase(mReceivingFiles.begin() + i);
            --i;
        }
    }

    if(!mReceivingCommands.size() && !mReceivingFiles.size() && mTimerToDeleteReceivedCommands)
    {
        mTimerToDeleteReceivedCommands->deleteLater();
        mTimerToDeleteReceivedCommands.release();
    }
    else
    {
        startCheckReceivingCommandsTimer();
    }
}
