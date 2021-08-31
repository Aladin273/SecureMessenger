#ifndef CLIENTDATABASE_H
#define CLIENTDATABASE_H

#include "pch.h"
#include "CommandsTypes.h"
#include "DataBase.h"
#include "common.h"

class ClientDataBase : public DataBase
{

protected:
    virtual void createTable() override;

public:
    ClientDataBase(const QString database_name);
    ~ClientDataBase();

    void getMessagesByNames(QVector<Message> &messages, const QString &user, const QString &talker);
    void writeMessage(const Message &message);

};

#endif // CLIENTDATABASE_H
