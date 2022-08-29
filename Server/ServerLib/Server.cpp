#include "Server.h"
#include "ServerDatabase.h"

Server::Server(QObject* parent)
    :QObject(parent)
{
    mServer.reset(new QTcpServer);

    if(mServer)
    {
        connect(mServer.get(), &QTcpServer::newConnection, this, &Server::onNewConnection);
    }


}

Server::~Server()
{
}

void Server::startCheckReadCommandTimer()
{
    if(!mUpdateReadCommandsVec)
    {
        mUpdateReadCommandsVec = new QTimer();

        mUpdateReadCommandsVec->setSingleShot(true);

        connect(mUpdateReadCommandsVec, &QTimer::timeout, this, &Server::updateReadCommandsVec);

        mUpdateReadCommandsVec->start(updateCommandsIntervalMs);
    }
    else if(mUpdateReadCommandsVec->remainingTime()<=0)
    {
        mUpdateReadCommandsVec->start(updateCommandsIntervalMs);
    }
}

void Server::startSendServerDataTimer()
{
    if(!mSendServerCommandsTimer)
    {
        mSendServerCommandsTimer = new QTimer();

        mSendServerCommandsTimer->setSingleShot(true);

        connect(mSendServerCommandsTimer, &QTimer::timeout, this, &Server::writeServerCommands);

        mSendServerCommandsTimer->start(sendServerCommandsIntervalMs);
    }
    else if(mSendServerCommandsTimer->remainingTime()<=0)
    {
        mSendServerCommandsTimer->start(sendServerCommandsIntervalMs);
    }
}

void Server::startCheckPortionDataTimer()
{
    if(!mTryToReadNextPortion)
    {
        mTryToReadNextPortion = new QTimer();

        mTryToReadNextPortion->setSingleShot(true);

        connect(mTryToReadNextPortion, &QTimer::timeout, this, &Server::tryToReadDataPortions);

        mTryToReadNextPortion->start(sendServerCommandsIntervalMs);
    }
    else if(mTryToReadNextPortion->remainingTime()<=0)
    {
        mTryToReadNextPortion->start(sendServerCommandsIntervalMs);
    }
}

void Server::performServerCommand(const Command &command, QTcpSocket *sender)
{
    if(sender)
    {
        auto serverCommand = static_cast<CommandsToServer>(command.cmdId);

        switch(serverCommand)
        {
        case CommandsToServer::REGISTER:
        {
            tryToRegister(command, sender);
            break;
        }
        case CommandsToServer::IDENTIFY:
        {
            tryToIdentify(command, sender);
            break;
        }
        case CommandsToServer::GET_ONLINE_USERS:
        {
            giveOnlineUsers(command, sender);
            break;
        }
        case CommandsToServer::GET_ALL_USERS:
        {
            giveAllUsers(command, sender);
            break;
        }
        default: break;
        }
    }

    startSendServerDataTimer();
}

void Server::updateReadCommandsVec()
{
    for(int i=0;i<mUsersCommands.size();i++)
    {
        QTime currentTime = QTime::currentTime();

        int diff = mUsersCommands[i].lastUpdateTime.msecsTo(currentTime);

        if(diff>=updateCommandsIntervalMs || diff<0)
        {
            mUsersCommands.erase(mUsersCommands.begin() + i);
            --i;
        }
    }

    if(!mUsersCommands.size() && mUpdateReadCommandsVec)
    {
        mUpdateReadCommandsVec->deleteLater();
        mUpdateReadCommandsVec = nullptr;
    }
    else
    {
        startCheckReadCommandTimer();
    }
}

void Server::tryToRegister(const Command& command, QTcpSocket *sender)
{
    if(sender)
    {
        auto dataBasePtr = ServerDataBase::getInstance();
        auto dataBase = dataBasePtr.lock();

        if(dataBase)
        {
            if(command.data.size() == 2)
            {
                QString login(command.data[0]);
                QString passHash(command.data[1]);

                Command serverCommand;

                serverCommand.cmdIdentifier = command.cmdIdentifier;

                bool isUserExist = dataBase->isUserExist(login);

                if(isUserExist)
                {
                    serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_REGISTER_FAIL);

                    QByteArray arr;
                    QDataStream stream(&arr, QIODevice::WriteOnly);

                    qint16 error = static_cast<int>(CommandErrors::USER_EXIST);
                    stream<<error;

                    serverCommand.data.push_back(arr);
                }
                else
                {
                    serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_REGISTER_SUCCESS);

                    dataBase->addUserToDataBase(login, passHash);

                    mUnconnectedUsers.remove(sender);

                    mActiveConnections[login] = sender;

                    onNewUserConnected(login);
                }

                mServerCommands.push_back(std::make_pair(sender, serverCommand));
            }
        }
    }
}

void Server::tryToIdentify(const Command& command, QTcpSocket *sender)
{
    if(sender)
    {
        auto dataBasePtr = ServerDataBase::getInstance();
        auto dataBase = dataBasePtr.lock();

        if(dataBase)
        {
            if(command.data.size() == 2)
            {
                QString login(command.data[0]);
                QString passHash(command.data[1]);

                Command serverCommand;

                serverCommand.cmdIdentifier = command.cmdIdentifier;

                bool isUserDataCorrect = dataBase->identifyUser(login, passHash);

                if(isUserDataCorrect)
                {
                    serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_IDENTIFY_SUCCESS);

                    mUnconnectedUsers.remove(sender);

                    mActiveConnections[login] = sender;

                    onUserConnected(login);
                }
                else
                {
                    serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_IDENTIFY_FAIL);

                    QByteArray arr;
                    QDataStream stream(&arr, QIODevice::WriteOnly);

                    qint16 error = static_cast<int>(CommandErrors::INCORRECT_DATA);
                    stream<<error;

                    serverCommand.data.push_back(arr);
                }

                mServerCommands.push_back(std::make_pair(sender, serverCommand));
            }
        }
    }
}

void Server::giveOnlineUsers(const Command& command, QTcpSocket *sender)
{
    Command serverCommand;
    serverCommand.cmdIdentifier = command.cmdIdentifier;
    serverCommand.cmdId = static_cast<int>(CommandsToClient::ONLINE_USERS);

    qint64 needBytes = 0;

    auto it = std::find_if(mActiveConnections.begin(), mActiveConnections.end(), [=](const QTcpSocket* connection)
    {
        return connection == sender;
    });

    QString requester;

    if(it!=mActiveConnections.end())
    {
        requester = it.key();
    }

    for(auto& user : mActiveConnections.keys())
    {
        if(user!=requester)
        {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);

            stream<<user;

            serverCommand.data.push_back(data);
            needBytes+=data.size();
        }
    }

    serverCommand.needBytes = needBytes;

    mServerCommands.push_back(std::make_pair(sender, serverCommand));
}

void Server::giveAllUsers(const Command &command, QTcpSocket *sender)
{
    Command serverCommand;
    serverCommand.cmdIdentifier = command.cmdIdentifier;
    serverCommand.cmdId = static_cast<int>(CommandsToClient::ALL_USERS);

    qint64 needBytes = 0;

    auto it = std::find_if(mActiveConnections.begin(), mActiveConnections.end(), [=](const QTcpSocket* connection)
    {
        return connection == sender;
    });

    QString requester;

    if(it!=mActiveConnections.end())
    {
        requester = it.key();
    }

    QVector<QString> allUsers;

    auto dataBasePtr = ServerDataBase::getInstance();
    auto dataBase = dataBasePtr.lock();

    if(dataBase)
    {
        allUsers = dataBase->getExistingUsers();
    }

    for(auto& user : allUsers)
    {
        if(user!=requester)
        {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);

            stream<<user;

            serverCommand.data.push_back(data);
            needBytes+=data.size();
        }
    }

    serverCommand.needBytes = needBytes;

    mServerCommands.push_back(std::make_pair(sender, serverCommand));
}

void Server::onNewUserConnected(const QString &login)
{
    for(auto it = mActiveConnections.begin(); it!=mActiveConnections.end();it++)
    {
        Command serverCommand;
        serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_NEW_USER);

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream<<login;

        serverCommand.needBytes = data.size();
        serverCommand.data.push_back(data);

        if(it.key()!=login)
        {
            serverCommand.cmdIdentifier = getNextServerCommandIdentifier();
            mServerCommands.push_back(std::make_pair(it.value(),serverCommand));
        }
    }
}

void Server::onUserConnected(const QString &login)
{
    for(auto it = mActiveConnections.begin(); it!=mActiveConnections.end();it++)
    {
        Command serverCommand;
        serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_USER_CONNECT);

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream<<login;

        serverCommand.needBytes = data.size();
        serverCommand.data.push_back(data);

        if(it.key()!=login)
        {
            serverCommand.cmdIdentifier = getNextServerCommandIdentifier();
            mServerCommands.push_back(std::make_pair(it.value(),serverCommand));
        }
    }
}

void Server::onUserDisconnected(const QString &login)
{
    for(auto it = mActiveConnections.begin(); it!=mActiveConnections.end();it++)
    {
        Command serverCommand;
        serverCommand.cmdId = static_cast<int>(CommandsToClient::ON_USER_DISCONNECT);

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream<<login;

        serverCommand.needBytes = data.size();
        serverCommand.data.push_back(data);

        if(it.key()!=login)
        {
            serverCommand.cmdIdentifier = getNextServerCommandIdentifier();
            mServerCommands.push_back(std::make_pair(it.value(),serverCommand));
        }
    }
}

bool Server::needToParseSender(CommandsToServer command)
{
    bool result = false;

    if(command == CommandsToServer::SEND_MESSAGE
            || command == CommandsToServer::GET_MESSAGE_HISTORY
            || command == CommandsToServer::SEND_MESSAGE_HISTORY
            || command == CommandsToServer::GET_FILE
            || command == CommandsToServer::SEND_FILE
            || command == CommandsToServer::FILE_GETTING_ERROR)
    {
        result = true;
    }

    return result;
}

QString Server::getNextServerCommandIdentifier()
{
    QString identifier = mServerIdentifier + QString::number(mCurrentServerCommand);

    ++mCurrentServerCommand;

    return identifier;
}

bool Server::runServer(QHostAddress hostToListen, qint16 portToListen)
{
    bool result = false;

    if(mServer)
    {
        mServer->close();

        result = mServer->listen(hostToListen, portToListen);
    }

    return result;
}

void Server::stopServer()
{
    mServer->close();
}

QTcpSocket *Server::getSocketByLogin(const QString &login)
{
    QTcpSocket* result = nullptr;

    auto it = mActiveConnections.find(login);

    if(it!=mActiveConnections.end())
    {
        result = *it;
    }

    return result;
}

Command& Server::receiveCommand(const QByteArray &data, QTcpSocket *sender)
{
    Command* resultCommand = nullptr;

    if(sender)
    {
        Command command;

        QDataStream stream(data);

        QString cmdIdentifier;

        stream>>cmdIdentifier;

        std::ostringstream stringStream;
        stringStream<<sender;

        cmdIdentifier = stringStream.str().c_str() + QString(" ") + cmdIdentifier;

        auto it = std::find_if(mUsersCommands.begin(), mUsersCommands.end(), [=](const Command& com)
        {
            return com.cmdIdentifier == cmdIdentifier;
        });

        if(it==mUsersCommands.end())
        {
            command.cmdIdentifier = cmdIdentifier;

            stream>>command.cmdId;

            if(!isCommandToServer(static_cast<CommandsToServer>(command.cmdId)))
            {
                stream>>command.target;
            }

            if(needToParseSender(static_cast<CommandsToServer>(command.cmdId)))
            {
                stream>>command.sender;
            }

            stream>>command.needBytes;

            while(!stream.atEnd())
            {
                QByteArray arr;
                stream>>arr;
                command.data.push_back(arr);

                command.gottenBytes += arr.size();
            }

            command.lastUpdateTime = QTime::currentTime();

            mUsersCommands.push_back(command);

            resultCommand = &(*mUsersCommands.rbegin());
        }
        else
        {
            while(!stream.atEnd())
            {
                QByteArray arr;
                stream>>arr;
                it->data.push_back(arr);

                command.gottenBytes += arr.size();
            }

            it->lastUpdateTime = QTime::currentTime();

            resultCommand = &(*it);
        }

        startCheckReadCommandTimer();

        if(!resultCommand)
        {
            qDebug()<<"Error command adding";
        }
    }

    return *resultCommand;
}

void Server::onReadCommandEnd(const Command& command, QTcpSocket* sender)
{
    if(sender)
    {
        if(isCommandToServer(static_cast<CommandsToServer>(command.cmdId)))
        {
            performServerCommand(command, sender);
        }

        auto it = std::find_if(mUsersCommands.begin(), mUsersCommands.end(), [=](const Command& comm)
        {
            return comm.cmdIdentifier == command.cmdIdentifier;
        });

        if(it!=mUsersCommands.end())
        {
            mUsersCommands.erase(it);
        }
    }
}

void Server::writeCommand(Command &command, QTcpSocket *sender)
{
    if(sender)
    {
        if(!isCommandToServer(static_cast<CommandsToServer>(command.cmdId)))
        {
            auto socket = getSocketByLogin(command.target);

            if(socket)
            {
                QByteArray arr;
                QDataStream stream(&arr, QIODevice::WriteOnly);

                stream<<command.cmdIdentifier;

                if(!command.sendedFirstTime)
                {
                     stream<<qint16(getCommandToClient(static_cast<CommandsToServer>(command.cmdId)));

                     if(!command.sender.isEmpty())
                     {
                         stream<<command.sender;
                     }

                     stream<<command.needBytes;

                     command.sendedFirstTime = true;
                }

                for(auto& data : command.data)
                {
                    stream<<data;
                }

                encodeDecode(arr);

                QByteArray dataWithSize;
                QDataStream streamForSetSize(&dataWithSize, QIODevice::WriteOnly);

                streamForSetSize<<quint64(0);
                streamForSetSize<<arr;
                streamForSetSize.device()->seek(0);
                streamForSetSize<<quint64(dataWithSize.size() - sizeof (quint64));

                socket->write(dataWithSize);

                command.data.clear();
            }
        }

        if(command.needBytes <= command.gottenBytes)
        {
            onReadCommandEnd(command, sender);
        }
    }
}

void Server::onReadyRead()
{
    QTcpSocket* socketSender = dynamic_cast<QTcpSocket*>(sender());

    if(socketSender)
    {
        QByteArray receiveBytes = socketSender->readAll();

        tryToReadData(socketSender, receiveBytes);
    }
}

void Server::tryToReadData(QTcpSocket* socket, QByteArray& arrayData)
{
    if(socket)
    {
        int quint64Size = sizeof(quint64);

        auto it = mPortionDataFromSocket.find(socket);

        if(it == mPortionDataFromSocket.end())
        {
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

                    Command& command = receiveCommand(data, socket);

                    writeCommand(command, socket);

                    arrayData.remove(0, blockSize + quint64Size);
                }
            }

            if(arrayData.size())
            {
                DataPortion portion;
                portion.lastUpdateTime = QTime::currentTime();
                portion.unsortedData = arrayData;

                mPortionDataFromSocket[socket] = portion;
                startCheckPortionDataTimer();
            }
        }
        else
        {
            it->lastUpdateTime = QTime::currentTime();
            it->unsortedData+=arrayData;
        }
    }
}

void Server::tryToReadDataPortions()
{
    for(auto beginIt = mPortionDataFromSocket.begin(); beginIt != mPortionDataFromSocket.end(); )
    {
        int qint64Size = sizeof(quint64);

        if(!beginIt->blockSize && beginIt->unsortedData.size()>=qint64Size)
        {
            QDataStream stream(&beginIt->unsortedData, QIODevice::ReadOnly);
            stream>>beginIt->blockSize;
            beginIt->unsortedData.remove(0, qint64Size);

            beginIt->lastUpdateTime = QTime::currentTime();
        }

        if(beginIt->blockSize && beginIt->unsortedData.size()>=beginIt->blockSize)
        {
            QByteArray data;

            QDataStream stream(&beginIt->unsortedData, QIODevice::ReadOnly);

            stream>>data;

            encodeDecode(data);

            Command& command = receiveCommand(data, beginIt.key());

            writeCommand(command, beginIt.key());

            beginIt->unsortedData.remove(0, beginIt->blockSize);

            beginIt->lastUpdateTime = QTime::currentTime();

            beginIt->blockSize = 0;
        }

        QTime currentTime = QTime::currentTime();

        int diff = beginIt->lastUpdateTime.msecsTo(currentTime);

        if(diff >= updateCommandsIntervalMs || diff<0 || (!beginIt->blockSize && !beginIt->unsortedData.size()))
        {
            beginIt = mPortionDataFromSocket.erase(beginIt);
        }
        else
        {
            ++beginIt;
        }
    }

    if(!mPortionDataFromSocket.size() && mTryToReadNextPortion)
    {
        mTryToReadNextPortion->deleteLater();
        mTryToReadNextPortion = nullptr;
    }
    else
    {
        startCheckPortionDataTimer();
    }
}

void Server::onNewConnection()
{
    if(mServer)
    {
        QTcpSocket* nextConnection = mServer->nextPendingConnection();

        connect(nextConnection, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(nextConnection, &QTcpSocket::disconnected, this, &Server::onDisconnect);

        mUnconnectedUsers.insert(nextConnection);
    }
}

void Server::onDisconnect()
{
    QTcpSocket* socket = dynamic_cast<QTcpSocket*>(sender());

    if(socket)
    {
        auto it = mUnconnectedUsers.find(socket);

        QString senderLogin;

        if(it!=mUnconnectedUsers.end())
        {
            mUnconnectedUsers.erase(it);
        }
        else
        {
            auto mapIt = std::find_if(mActiveConnections.begin(), mActiveConnections.end(), [=](const QTcpSocket* connection)
            {
               return  connection==socket;
            });

            if(mapIt!=mActiveConnections.end())
            {
                senderLogin = mapIt.key();
                mActiveConnections.erase(mapIt);
            }
        }

        if(!senderLogin.isEmpty())
        {
            onUserDisconnected(senderLogin);
        }

        auto commandsIt = std::find_if(mServerCommands.begin(), mServerCommands.end(), [=](const std::pair<QTcpSocket*, Command>& serverCommand)
                        {
                            return serverCommand.first == socket;
                        });

        while(commandsIt!=mServerCommands.end())
        {
            mServerCommands.erase(commandsIt);

            commandsIt = std::find_if(mServerCommands.begin(), mServerCommands.end(), [=](const std::pair<QTcpSocket*, Command>& serverCommand)
                        {
                            return serverCommand.first == socket;
                        });
        }
    }
}

void Server::writeServerCommands()
{
    for (auto it = mServerCommands.begin(); it != mServerCommands.end();)
    {
        int removingBeginParts = 0;
        int currentBytes = 0;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        if(it->second.sendedFirstTime)
        {
            stream<<it->second.cmdIdentifier;
        }
        else
        {
            stream<<it->second.cmdIdentifier<<it->second.cmdId<<it->second.needBytes;
        }

        for(auto& dataPart : it->second.data)
        {
            if(maxBytesPerSending > (currentBytes + dataPart.size()))
            {
                stream<<dataPart;
                currentBytes += dataPart.size();
                ++removingBeginParts;
            }
            else
            {
                if(!(data.size() - it->second.cmdIdentifier.size()) && it->second.data.size())
                {
                    stream<<it->second.data[0];
                    currentBytes += dataPart.size();
                    ++removingBeginParts;
                }

                break;
            }
        }

        for(int i = 0;i<removingBeginParts;i++)
        {
            if(it->second.data.begin()!=it->second.data.end())
            {
                it->second.data.erase(it->second.data.begin());
            }
        }

        it->second.gottenBytes+=currentBytes;

        encodeDecode(data);

        QByteArray dataWithSize;
        QDataStream streamForSetSize(&dataWithSize, QIODevice::WriteOnly);

        streamForSetSize<<quint64(0);
        streamForSetSize<<data;
        streamForSetSize.device()->seek(0);
        streamForSetSize<<quint64(dataWithSize.size() - sizeof (quint64));

        it->first->write(dataWithSize);

        if(it->second.needBytes <= it->second.gottenBytes)
        {
            it = mServerCommands.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if(!mServerCommands.size() && mSendServerCommandsTimer)
    {
        mSendServerCommandsTimer->deleteLater();
        mSendServerCommandsTimer = nullptr;
    }
    else
    {
        startSendServerDataTimer();
    }
}
