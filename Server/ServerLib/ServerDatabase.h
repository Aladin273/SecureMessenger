#ifndef SERVERDATABASE_H
#define SERVERDATABASE_H

#include "DataBase.h"
#include "pch.h"

class ServerDataBase : public DataBase
{
private:
    static std::shared_ptr<ServerDataBase> mDataBase;

private:
    ServerDataBase(const QString& aDataBaseName);

protected:
    virtual void createTable() override;

public:
    ServerDataBase() = delete;

    bool isUserExist(const QString& aUserName);
    void addUserToDataBase(const QString& aUserName, const QString& aPassHash);
    bool identifyUser(const QString& aUserName, const QString& aPassHash);
    QVector<QString> getExistingUsers();

    static std::weak_ptr<ServerDataBase> getInstance();
};

#endif // SERVERDATABASE_H
