#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QStackedWidget>
#include <QFileDialog>
#include <QDesktopServices>
#include <QScrollBar>
#include <QMessageBox>

#include "client.h"
#include "userwidgets.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QStackedWidget
{   
    Q_OBJECT

    Ui::MainWindow *m_ui;
    Client *m_client;

    QList<int> m_splitter;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void clearLayout(QLayout *layout);
    void setupSession(Status status);

private slots:
    // LOGIN PAGE
    void on_pushButton_connect_clicked();
    void on_pushButton_register_clicked();

    // CHATLIST
    void on_pushButton_logout_clicked();
    void on_pushButton_chatlist_clicked();
    void displayChatList(const QVector<User> &vec_users);

    // CHAT AREA
    void openFile();
    void getMessageHistory();
    void getMessageHlstory();
    void displayReceivedMessage(const Message &message);
    void displayMessageHistory(const QVector<Message> &vec_messages);
    void displayFileInfo(const QString& fileOwner, const QString& fileName, Status status);

    void on_pushButton_hidesb_clicked();
    void on_pushButton_send_clicked();
    void on_pushButton_attach_clicked();
    void on_splitter_main_splitterMoved(int pos, int index);

};

#endif // MAINWINDOW_H
