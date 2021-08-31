#include "userwidgets.h"

///////////// userButton
/////////////////////////////////////////////////////////////////////////////////

userButton::userButton(const User &user, QWidget *parent)
    :QPushButton(parent), m_user_info(user)
{
    this->installEventFilter(this);

    m_layout = new QHBoxLayout(this);
    m_user = new QLabel(this);
    m_online = new QWidget(this);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());

    this->setObjectName(QString::fromUtf8("userButton"));
    this->setSizePolicy(sizePolicy);
    this->setMinimumSize(QSize(130, 40));
    this->setMaximumSize(QSize(16777215, 40));
    this->setStyleSheet(QString::fromUtf8("QWidget\n"
              "{\n"
              "	border:solid;\n"
              " color:rgb(240,240,240);\n"
              "	background-color:rgb(33,33,33);\n"
              "}"));

    QFont font;
    font.setPointSize(11);
    font.setBold(false);
    font.setWeight(50);
    sizePolicy.setHeightForWidth(m_user->sizePolicy().hasHeightForWidth());

    m_user->setSizePolicy(sizePolicy);
    m_user->setMinimumSize(QSize(0, 40));
    m_user->setText(m_user_info.username);
    m_user->setFont(font);
    m_user->setFrameShape(QFrame::NoFrame);
    m_user->setFrameShadow(QFrame::Plain);
    m_user->setLineWidth(0);
    m_user->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    m_user->setOpenExternalLinks(false);

    m_layout->setSpacing(10);
    m_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    m_layout->setContentsMargins(15, 0, 15, 0);

    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_online->sizePolicy().hasHeightForWidth());

    m_online->setSizePolicy(sizePolicy1);
    m_online->setMinimumSize(QSize(10, 10));
    m_online->setMaximumSize(QSize(10, 10));

    if(m_user_info.online)
    {
        m_online->setStyleSheet(QString::fromUtf8("QWidget\n"
        "{\n"
        "	background-color:white;\n"
        "	border-radius:4%;\n"
        "}"));
    }
    else
    {
        m_online->setStyleSheet(QString::fromUtf8("QWidget\n"
        "{\n"
        "	background-color:rgba(0,0,0,0);\n"
        "	border-radius:4%;\n"
        "}"));
    }

    m_layout->addWidget(m_user);
    m_layout->addWidget(m_online);
}

bool userButton::eventFilter(QObject *obj, QEvent *event)
{
    if (obj ==  this)
    {
          QEvent::Type type = event->type();
          if  (type == QEvent::HoverLeave)
          {
              this->setStyleSheet(QString::fromUtf8("QWidget\n"
              "{\n"
              "	border:solid;\n"
              " color:rgb(240,240,240);\n"
              "	background-color:rgb(33,33,33);\n"
              "}"));
          }
          else if (type == QEvent::HoverEnter)
          {
              this->setStyleSheet(QString::fromUtf8("QWidget\n"
              "{\n"
              "	border:solid;\n"
              " color:rgb(240,240,240);\n"
              "	background-color:rgb(66,66,66);\n"
              "}"));
          }
          else if (type == QEvent::MouseButtonPress)
          {
              this->setStyleSheet(QString::fromUtf8("QWidget\n"
              "{\n"
              "	border:solid;\n"
              " color:rgb(240,240,240);\n"
              " background-color:rgb(99,99,99);\n"
              "}"));
          }
          else if (type == QEvent::MouseButtonRelease)
          {
              this->setStyleSheet(QString::fromUtf8("QWidget\n"
              "{\n"
              "	border:solid;\n"
              " color:rgb(240,240,240);\n"
              "	background-color:rgb(66,66,66);\n"
              "}"));
          }

      }

    return QWidget::eventFilter(obj, event);
}

QString userButton::getUserName()
{
    return m_user_info.username;
}

bool userButton::isOnline()
{
    return m_user_info.online;
}


///////////// userMessage
/////////////////////////////////////////////////////////////////////////////////

userMessage::userMessage(const Message &message, QWidget *parent)
    : QWidget(parent), m_message(message)
{
        m_gridLayout_message = new QGridLayout(this);
        m_label_user = new QLabel(this);
        m_label_datetime = new QLabel(this);

        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(this->sizePolicy().hasHeightForWidth());

        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(m_label_user->sizePolicy().hasHeightForWidth());

        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(m_label_user->sizePolicy().hasHeightForWidth());

        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);

        QFont font1;
        font1.setPointSize(11);

        this->setObjectName(QString::fromUtf8("userMessage"));
        this->setSizePolicy(sizePolicy1);
        this->setMinimumSize(QSize(310, 40));
        this->setStyleSheet(QString::fromUtf8(
"QLabel:hover\n"
"{\n"
"	border-radius:7%;\n"
"	background-color:rgb(66,66,66);\n"
"}\n"
" QPushButton\n"
"{\n"
"	border:1px solid white;\n"
"	border-radius:7%;\n"
"}\n"
"QPushButton:hover\n"
"{\n"
"	background-color:rgb(66,66,66);\n"
"}\n"
"QPushButton:pressed\n"
"{\n"
"	background-color:rgb(99,99,99);\n"
"}\n"
"\n"
"\n"
""));


        m_gridLayout_message->setHorizontalSpacing(5);
        m_gridLayout_message->setVerticalSpacing(2);
        m_gridLayout_message->setContentsMargins(20, 5, 20, 5);


        m_label_user->setSizePolicy(sizePolicy2);
        m_label_user->setFont(font);
        m_label_user->setLineWidth(1);
        m_label_user->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        m_label_user->setWordWrap(false);
        m_label_user->setMargin(2);
        m_label_user->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);


        sizePolicy2.setHeightForWidth(m_label_datetime->sizePolicy().hasHeightForWidth());
        m_label_datetime->setSizePolicy(sizePolicy2);
        m_label_datetime->setMinimumSize(QSize(0, 0));
        m_label_datetime->setMaximumSize(QSize(16777215, 16777215));
        m_label_datetime->setStyleSheet(QString::fromUtf8("color: rgb(132, 132, 132);"));
        m_label_datetime->setAlignment(Qt::AlignCenter);
        m_label_datetime->setMargin(2);
        m_label_datetime->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);



        if(message.file == false)
        {
            m_label_content = new QLabel(this);
            sizePolicy1.setHeightForWidth(m_label_content->sizePolicy().hasHeightForWidth());
            m_label_content->setSizePolicy(sizePolicy1);
            m_label_content->setFont(font1);
            m_label_content->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
            m_label_content->setWordWrap(true);
            m_label_content->setMargin(2);
            m_label_content->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

            m_label_content->setOpenExternalLinks(true);
            m_label_content->setText(m_message.content);

            m_gridLayout_message->addWidget(m_label_content, 1, 0, 1, 3);
        }
        else
        {
            static QIcon m_icon;
            static bool cache = false;

            if(!cache)
            {
                m_icon.addFile(QString::fromUtf8(":/resources/img/file_white.png"), QSize(), QIcon::Normal, QIcon::Off);
                cache = true;
            }

            m_pushButton_content = new QPushButton(this);

            sizePolicy3.setHeightForWidth(m_pushButton_content->sizePolicy().hasHeightForWidth());
            m_pushButton_content->setSizePolicy(sizePolicy3);
            m_pushButton_content->setMinimumSize(QSize(100, 24));
            m_pushButton_content->setMaximumSize(QSize(16777215, 24));
            m_pushButton_content->setFont(font1);
            m_pushButton_content->setIcon(m_icon);
            m_pushButton_content->setIconSize(QSize(24, 24));
            m_pushButton_content->setText(m_message.content);

            m_gridLayout_message->setVerticalSpacing(4);
            m_gridLayout_message->addWidget(m_pushButton_content, 1, 0, 1, 3);
        }

        QDateTime dt;
        dt.setTime_t(m_message.datetime);

        m_label_user->setText(m_message.sender);
        m_label_datetime->setText(dt.toLocalTime().toString("dd.MM.yyyy  |  hh:mm"));

        m_gridLayout_message->addWidget(m_label_user, 0, 0, 1, 1);
        m_gridLayout_message->addWidget(m_label_datetime, 0, 1, 1, 1);
        m_gridLayout_message->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2, 1, 1);
}

bool userMessage::isFile()
{
    return m_message.file;
}

QPushButton *userMessage::getFileButton()
{
    return m_pushButton_content;
}

//            m_gridLayout_message->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 2, 1, 3);  // 1212
