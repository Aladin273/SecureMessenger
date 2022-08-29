#ifndef COMMON_H
#define COMMON_H

#include <QString>

struct Message
{
    QString sender;
    QString receiver;
    qint64 datetime;
    QString content;
    bool file;
};

struct User
{
    QString username;
    bool online;
};

#endif // COMMON_H
