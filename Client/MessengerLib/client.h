#ifndef CLIENT_H
#define CLIENT_H

#include "clientdatabase.h"
#include "clientsocket.h"

enum Status
{
    SUCCESS,
    FAIL,
    FAIL_REGISTER,
    FAIL_AUTENTIFY,
    ERROR_EMPTY,
    ERROR_SHORT_LOGIN,
    ERROR_SHORT_PASS,
    ERROR_BIG_LOGIN,
    ERROR_BIG_PASS,
    ERROR_WRONG_SYMBOL,
    DISCONNECTED,
    FILE_DOWNLOADED,
    FILE_IS_ALDREADY_DOWNLOAD,
    FILE_DOWNLOAD_ERROR,
};

class Client : public QObject
{
    Q_OBJECT

    std::shared_ptr<ClientDataBase> m_db;
    ClientSocket *m_socket;

    QVector<User> m_vec_users;
    QVector<Message> m_vec_messages;

    QString m_login = "Admin";
    QString m_passhash = "password";

    QString m_receiver = "Receiver";
    bool m_receiver_online = false;

    const QString m_files_path = QCoreApplication::applicationDirPath()+"/files/";
    const QString m_db_path = QCoreApplication::applicationDirPath()+"/userdata/";

public:
    Client(QObject *parent = nullptr);
    ~Client();

    Status processInput(QString ip, QString login, QString pass);

    QString getLogin();
    QString getReceiver();
    QString getFilesPath();
    QString getDataBasePath();
    bool getReceiverOnline();

    void loginUser(QString ip, QString login, QString pass);
    void registerUser(QString ip, QString login, QString pass);
    void logoutUser();

    void getChatList();
    void getMessageHistory();
    void getFile(const QString &fileName);

    void setReceiver(QString receiver);
    void setReceiverOnline(bool online);
    void sendMessage(const Message &message);

private slots:
    void serverResponceToStatus(ServerCommandResponce responce);

    void createListUsers(const QVector<QString> &vec_users);
    void createListUsersOnline(const QVector<QString> &vec_users);

    void createMessage(QString sender, const SendMessageStruct &message);
    void createMessageHistory(QString sender, const QVector<SendMessageStruct> &vec_messages);

    void aboutFileDownloaded(const QString& fileOwner, const QString& fileName);
    void aboutFileAlreadyDownload(const QString& fileOwner, const QString& fileName);
    void aboutFileDownloadError(const QString& fileOwner, const QString& fileName);

    void userConnected(const QString& user);

signals:
    void statusToUI(Status status);
    void usersToUI(const QVector<User> &vec_users);
    void messageToUI(const Message &message);
    void messageHistoryToUI(const QVector<Message> &vec_messages);
    void aboutFileToUI(const QString& fileOwner, const QString& fileName, Status status);

    void startSession(const QString &login, const QString &passhash);

};

#endif // CLIENT_H
