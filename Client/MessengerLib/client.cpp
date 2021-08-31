#include "client.h"

Client::Client(QObject *parent)
    :QObject(parent)
{
    QDir dir1(m_files_path);
    if (!dir1.exists())
    {
        dir1.mkpath(".");
    }

    QDir dir2(m_db_path);
    if (!dir2.exists())
    {
        dir2.mkpath(".");
    }
}

Client::~Client()
{
    logoutUser();
}


void Client::loginUser(QString ip, QString login, QString pass)
{
    Status status = processInput(ip, login, pass);

    if(status == SUCCESS)
    {
        QString m_ip = ip;
        QString m_port = ip;
        int index_port = ip.indexOf(":");

        m_ip.remove(index_port, ip.size());
        m_port.remove(0, index_port+1);

        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(pass.toUtf8());

        m_passhash = QString(hash.result().toHex());
        m_login = login;

        m_db.reset(new ClientDataBase(m_db_path+m_login));
        m_socket = new ClientSocket(this);
        m_socket->setDataBase(m_db);

        connect(this, &Client::startSession, m_socket, &ClientSocket::tryToAutentify);

        connect(m_socket, &ClientSocket::onServerResponse, this, &Client::serverResponceToStatus);

        connect(m_socket, &ClientSocket::onGetAllUsers, this, &Client::createListUsers);
        connect(m_socket, &ClientSocket::onGetOnlineUsers, this, &Client::createListUsersOnline);

        connect(m_socket, &ClientSocket::onReceivedMessage, this, &Client::createMessage);
        connect(m_socket, &ClientSocket::onReceivedMessageHistory, this, &Client::createMessageHistory);

        connect(m_socket, &ClientSocket::onFileDownloaded, this, &Client::aboutFileDownloaded);
        connect(m_socket, &ClientSocket::onFileAlreadyDownloading, this, &Client::aboutFileAlreadyDownload);
        connect(m_socket, &ClientSocket::onFileDownloadingError, this, &Client::aboutFileDownloadError);

        // Пока что просто перезагружает ChatList
        connect(m_socket, &ClientSocket::onNewUserConnected, this, &Client::getChatList);
//        connect(m_socket, &ClientSocket::onUserDisconnected, this, &Client::getChatList); not worked

        // Не просто перезагружаем ChatList, но и если открыта переписка с этим юзером, то обновляем
        connect(m_socket, &ClientSocket::onUserConnected, this, &Client::userConnected);

        m_socket->connectToHost(m_ip, m_port.toInt());
    }
    else
        emit statusToUI(status);
}

void Client::registerUser(QString ip, QString login, QString pass)
{
    Status status = processInput(ip, login, pass);

    if(status == SUCCESS)
    {
        QString m_ip = ip;
        QString m_port = ip;
        int index_port = ip.indexOf(":");

        m_ip.remove(index_port, ip.size());
        m_port.remove(0, index_port+1);

        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(pass.toUtf8());

        m_passhash = QString(hash.result().toHex());
        m_login = login;

        m_db.reset(new ClientDataBase(m_db_path+m_login));
        m_socket = new ClientSocket(this);
        m_socket->setDataBase(m_db);

        connect(this, &Client::startSession, m_socket, &ClientSocket::tryToRegister);

        connect(m_socket, &ClientSocket::onServerResponse, this, &Client::serverResponceToStatus);

        connect(m_socket, &ClientSocket::onGetAllUsers, this, &Client::createListUsers);
        connect(m_socket, &ClientSocket::onGetOnlineUsers, this, &Client::createListUsersOnline);

        connect(m_socket, &ClientSocket::onReceivedMessage, this, &Client::createMessage);
        connect(m_socket, &ClientSocket::onReceivedMessageHistory, this, &Client::createMessageHistory);

        connect(m_socket, &ClientSocket::onFileDownloaded, this, &Client::aboutFileDownloaded);
        connect(m_socket, &ClientSocket::onFileAlreadyDownloading, this, &Client::aboutFileAlreadyDownload);
        connect(m_socket, &ClientSocket::onFileDownloadingError, this, &Client::aboutFileDownloadError);

        // Пока что просто перезагружает ChatList
        connect(m_socket, &ClientSocket::onNewUserConnected, this, &Client::getChatList);
//        connect(m_socket, &ClientSocket::onUserDisconnected, this, &Client::getChatList); not worked

        // Не просто перезагружаем ChatList, но и если открыта переписка с этим юзером, то обновляем
        connect(m_socket, &ClientSocket::onUserConnected, this, &Client::userConnected);

        m_socket->connectToHost(m_ip, m_port.toInt());
    }
    else
        emit statusToUI(status);
}

Status Client::processInput(QString ip, QString login, QString pass)
{
    QRegExp idExpr(QStringLiteral("[a-zA-Z0-9_]*"));
    bool wrong_symbol_login = !idExpr.exactMatch(login);
    bool wrong_symbol_pass = !idExpr.exactMatch(pass);

    if(ip.isEmpty() | login.isEmpty() | pass.isEmpty())
    {
        return ERROR_EMPTY;
    }
    else if(login.size() < 3)
    {
        return ERROR_SHORT_LOGIN;
    }
    else if(pass.size() < 8)
    {
        return ERROR_SHORT_PASS;
    }
    else if(login.size() > 30)
    {
        return ERROR_BIG_LOGIN;
    }
    else if(pass.size() > 30)
    {
        return ERROR_BIG_PASS;
    }
    else if(wrong_symbol_login || wrong_symbol_pass)
    {
        return ERROR_WRONG_SYMBOL;
    }
    else
    {
        return SUCCESS;
    }
}

void Client::logoutUser()
{
    m_socket->disconnect();
    m_socket->disconnectFromHost();

    m_db.reset();
    m_socket->deleteLater();
}

QString Client::getLogin()
{
    return m_login;
}

QString Client::getReceiver()
{
    return m_receiver;
}

bool Client::getReceiverOnline()
{
    return m_receiver_online;
}

QString Client::getFilesPath()
{
    return m_files_path;
}

QString Client::getDataBasePath()
{
    return m_db_path;
}

void Client::getChatList()
{           
    m_vec_users.clear();
    m_socket->tryToGetAllUsers();
    m_socket->tryToGetOnlineUsers();
}

void Client::getMessageHistory()
{
    m_vec_messages.clear();
    m_db->getMessagesByNames(m_vec_messages, m_login, m_receiver);

    if(m_receiver_online)
    {
        m_socket->tryToGetMessageHistory(m_receiver);
    }
    else
    {
        emit messageHistoryToUI(m_vec_messages);
    }
}

void Client::getFile(const QString &fileName)
{
    if(m_receiver_online)
        m_socket->tryToGetFile(m_receiver, fileName);
    else
        aboutFileDownloadError(m_receiver, fileName);
}

void Client::setReceiver(const QString receiver)
{
    m_receiver = receiver;
}

void Client::setReceiverOnline(bool online)
{
    m_receiver_online = online;
}

void Client::sendMessage(const Message &message)
{
    SendMessageStruct send_message {message.datetime, message.content, message.file };

    m_db->writeMessage(message);

    if(m_receiver_online)
        m_socket->sendMessage(send_message, message.receiver);
}

void Client::serverResponceToStatus(ServerCommandResponce responce)
{
    switch(responce)
    {
        case ServerCommandResponce::CONNECT_SUCCESS: emit startSession(m_login, m_passhash); break;
        case ServerCommandResponce::AUTENTIFY_SUCCESS: emit statusToUI(SUCCESS); break;
        case ServerCommandResponce::REGISTER_SUCCESS: emit statusToUI(SUCCESS); break;
        case ServerCommandResponce::CONNECT_FAIL: emit statusToUI(FAIL); logoutUser(); break;
        case ServerCommandResponce::AUTENTIFY_FAIL: emit statusToUI(FAIL_AUTENTIFY); logoutUser(); break;
        case ServerCommandResponce::REG_FAIL_USER_EXIST: emit statusToUI(FAIL_REGISTER); logoutUser(); break;
        case ServerCommandResponce::DISCONNECTED: emit statusToUI(DISCONNECTED); logoutUser(); break;
        default: break;
    }
}

void Client::createListUsers(const QVector<QString> &users)
{
    for(auto &user : users)
    {
        m_vec_users.push_back(User {user, false});
    }
}

void Client::createListUsersOnline(const QVector<QString> &users)
{
    for(auto &user_online : users)
    {
       for(auto &user: m_vec_users)
       {
           if(user_online == user.username)
               user.online = true;
       }
    }

    emit usersToUI(m_vec_users);
}

void Client::createMessage(const QString sender, const SendMessageStruct &message)
{
    m_db->writeMessage(Message { sender, m_login, message.datetime, message.content, message.file });

    if(sender == m_receiver)
        emit messageToUI(Message {sender, m_login, static_cast<int>(message.datetime), message.content, message.file});
}

void Client::createMessageHistory(QString sender, const QVector<SendMessageStruct> &vec_new)
{
    bool have_new = true;

    for(auto &new_m : vec_new)
    {
        have_new = true;

        for(auto &old_m : m_vec_messages)
        {
            if(old_m.sender == sender)
            {
                if(old_m.datetime == new_m.datetime)
                {
                    have_new = false;
                    break;
                }
            }
        }

        if(have_new)
        {
            Message message {sender, m_login, new_m.datetime, new_m.content, new_m.file};
            m_vec_messages.push_back(message);
            m_db->writeMessage(message);
        }
    }

    emit messageHistoryToUI(m_vec_messages);
}

void Client::userConnected(const QString &user)
{
    getChatList();

    if(user == m_receiver)
    {
        m_receiver_online = true;
        getMessageHistory();
    }
}

void Client::aboutFileDownloaded(const QString& fileOwner, const QString& fileName)
{
    emit aboutFileToUI(fileOwner, fileName, FILE_DOWNLOADED);
}

void Client::aboutFileAlreadyDownload(const QString& fileOwner, const QString& fileName)
{
    emit aboutFileToUI(fileOwner, fileName, FILE_IS_ALDREADY_DOWNLOAD);
}

void Client::aboutFileDownloadError(const QString& fileOwner, const QString& fileName)
{
    emit aboutFileToUI(fileOwner, fileName, FILE_DOWNLOAD_ERROR);
}
