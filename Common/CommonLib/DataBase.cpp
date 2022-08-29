#include "DataBase.h"

#include <QDir>
#include <QFile>

QSqlDatabase *DataBase::getDataBase()
{
    return mDataBase;
}

DataBase::DataBase(const QString& dataBaseName)
    :mDataBaseName(dataBaseName)
{
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection))
    {
        mDataBase = new QSqlDatabase(QSqlDatabase::database());
    }
    else
    {
        mDataBase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    }

    if(mDataBase)
    {
        mDataBase->setDatabaseName(dataBaseName);
    }
}

void DataBase::init()
{
    if(!mIsInited)
    {
        if(!QFile(mDataBaseName).exists())
        {
            mDataBase->open();

            createTable();
        }
        else
        {
            mDataBase->open();
        }

        mIsInited=true;
    }
}

bool DataBase::isDataBaseOpened()
{
    bool result = false;

    if(mDataBase)
    {
        result = mDataBase->isOpen();
    }

    return result;
}

DataBase::~DataBase()
{
    if(mDataBase)
    {
        delete mDataBase;
    }
}












