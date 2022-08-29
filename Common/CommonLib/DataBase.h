#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

class DataBase
{
private:
    QSqlDatabase* mDataBase = nullptr;

    bool mIsInited = false;
  
    QString mDataBaseName;

protected:
    virtual void createTable(){};
     QSqlDatabase* getDataBase();

public:
    DataBase(const QString& aDataBaseName);

    bool isDataBaseOpened();

    void init();

    virtual ~DataBase();
};

#endif // DATABASE_H
