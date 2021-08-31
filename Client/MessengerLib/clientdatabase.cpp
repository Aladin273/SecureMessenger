#include "clientdatabase.h"

ClientDataBase::ClientDataBase(const QString database_name)
    : DataBase(database_name+".db")
{
        DataBase::init();

        if(DataBase::isDataBaseOpened())
        {
            qDebug() << " Database ready";
        }
}

ClientDataBase::~ClientDataBase()
{
    qDebug() << " Database is closing";
}

void ClientDataBase::getMessagesByNames(QVector<Message> &vec_messages, const QString &user, const QString &talker)
{
    auto dataBase = DataBase::getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        QString text_query = "SELECT * FROM messages "
                       "WHERE ((sender = '%1' AND receiver = '%2')"
                       "OR (sender = '%3' AND receiver = '%4'))"
                       "ORDER BY datetime;";

        text_query = text_query.arg(user).arg(talker).arg(talker).arg(user);

        if(!query.exec(text_query))
        {
            qDebug() << " Get messages error: " << query.lastError();
        }
        else
        {
            while(query.next())
            {
                Message message;
                message.sender = query.value(1).toString();
                message.receiver = query.value(2).toString();
                message.datetime = query.value(3).toInt();
                message.content = query.value(4).toString();
                message.file = query.value(5).toBool();

                vec_messages.push_back(message);
            }
        }
    }
    else
    {
        qDebug() << " Database not opened: " << dataBase->lastError();
    }
}

void ClientDataBase::writeMessage(const Message &message)
{
    auto dataBase = DataBase::getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        QString text_query = "INSERT INTO messages (sender, receiver, datetime, content, file)"
                             "VALUES ('%1', '%2', %3, '%4', %5);";

        text_query = text_query.arg(message.sender).arg(message.receiver).arg(message.datetime).arg(message.content).arg(message.file);

        if(!query.exec(text_query))
        {
            qDebug() << " Write message error: " << query.lastError();
        }
    }
    else
    {
        qDebug() << " Database not opened: " << dataBase->lastError();
    }

}

void ClientDataBase::createTable()
{
    auto dataBase = DataBase::getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        qDebug() << " Database created";

        QSqlQuery query(*dataBase);

        if(query.exec("CREATE TABLE messages ("
                      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "    sender TEXT,"
                      "    receiver TEXT,"
                      "    datetime INTEGER,"
                      "    content TEXT,"
                      "    file INTEGER);"))
        {
            qDebug()<<" Table created";
        }
        else
        {
            qDebug()<<" Table create error: "<<query.lastError();
        }
    }
    else
    {
        qDebug() << " Database not opened: " << dataBase->lastError();
    }
}

