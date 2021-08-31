#include "ServerDatabase.h"

ServerDataBase::ServerDataBase(const QString& dataBaseName)
    :DataBase(dataBaseName)
{
}

bool ServerDataBase::isUserExist(const QString &userName)
{
    bool result = false;

    auto dataBase = getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        query.prepare("SELECT Login "
                       "FROM ServerUsers "
                       "WHERE Login = :UserName");

        query.bindValue(":UserName", userName);

        if(!query.exec())
        {
            qDebug()<<query.lastError();
        }
        else
        {
            if(query.next())
            {
                result = true;
            }
        }
    }

    return result;
}

void ServerDataBase::addUserToDataBase(const QString &userName, const QString &passHash)
{
    auto dataBase = getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        query.prepare("INSERT INTO ServerUsers "
                        "(Login, Password) "
                        "VALUES(:UserName, :PassHash)");

        query.bindValue(":UserName", userName);
        query.bindValue(":PassHash", passHash);

        if(!query.exec())
        {
            qDebug()<<query.lastError();
        }
    }
}

bool ServerDataBase::identifyUser(const QString &userName, const QString &passHash)
{
    bool result = false;

    auto dataBase = getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        query.prepare("SELECT * FROM ServerUsers "
                       "WHERE Login = :UserName "
                       "AND Password = :PassHash");

        query.bindValue(":UserName", userName);
        query.bindValue(":PassHash", passHash);

        if(!query.exec())
        {
            qDebug()<<query.lastError();
        }
        else
        {
            if(query.next())
            {
                result = true;
            }
        }
    }

    return result;
}

QVector<QString> ServerDataBase::getExistingUsers()
{
    QVector<QString> result;

    auto dataBase = getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        if(!query.exec("SELECT Login FROM ServerUsers"))
        {
            qDebug()<<query.lastError();
        }
        else
        {
            while(query.next())
            {
                result.push_back(query.value(0).toString());
            }
        }
    }

    return result;
}

std::weak_ptr<ServerDataBase> ServerDataBase::getInstance()
{
    if(!mDataBase)
    {
        mDataBase.reset(new ServerDataBase("ServerDataBase.db"));

        mDataBase->init();
    }

    return mDataBase;
}

void ServerDataBase::createTable()
{
    auto dataBase = getDataBase();

    if(dataBase && dataBase->isOpen())
    {
        QSqlQuery query(*dataBase);

        if(query.exec("CREATE TABLE ServerUsers("
                    "Login TEXT NOT NULL,"
                    "Password TEXT NOT NULL)"))
        {
            qDebug()<<"Table created";
        }
        else
        {
            qDebug()<<query.lastError();
        }
    }
}

std::shared_ptr<ServerDataBase> ServerDataBase::mDataBase;
