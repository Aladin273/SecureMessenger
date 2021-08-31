#ifndef USERWIDGETS_H
#define USERWIDGETS_H

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QEvent>
#include <QWidget>
#include <QLabel>
#include <QSpacerItem>
#include <QGridLayout>
#include <QPushButton>
#include <QDateTime>


#include "common.h"


class userButton : public QPushButton
{
    Q_OBJECT

    QHBoxLayout *m_layout;
    QLabel *m_user;
    QWidget *m_online;

    User m_user_info;

protected:
    bool eventFilter(QObject *, QEvent *event);

public:
    explicit userButton(const User &user, QWidget *parent = nullptr);

    QString getUserName();
    bool isOnline();

};

class userMessage : public QWidget
{
    Q_OBJECT

    QGridLayout *m_gridLayout_message;
    QLabel *m_label_user;
    QLabel *m_label_datetime;
    QLabel *m_label_content;
    QPushButton *m_pushButton_content;

    Message m_message;

public:
    explicit userMessage(const Message &message, QWidget *parent = nullptr);

    QPushButton *getFileButton();
    bool isFile();

};


#endif // USERWIDGETS_H
