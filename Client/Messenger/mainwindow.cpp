#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QStackedWidget(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this); setFocus();
    m_client = new Client(this);

    connect(m_client, &Client::statusToUI, this, &MainWindow::setupSession);
    connect(m_client, &Client::usersToUI, this, &MainWindow::displayChatList);
    connect(m_client, &Client::messageToUI, this, &MainWindow::displayReceivedMessage);
    connect(m_client, &Client::messageHistoryToUI, this, &MainWindow::displayMessageHistory);
    connect(m_client, &Client::aboutFileToUI, this, &MainWindow::displayFileInfo);

    connect(m_ui->pushButton_front, &QPushButton::clicked, this, &MainWindow::getMessageHlstory);
    connect(m_ui->pushButton_back, &QPushButton::clicked, this, &MainWindow::getMessageHlstory);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::clearLayout(QLayout *layout)
{
    QLayoutItem* child;
    while(layout->count()!=0)
    {
        child = layout->takeAt(0);
        if(child->layout() != 0)
        {
            clearLayout(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}

void MainWindow::setupSession(Status status)
{

    if(status == SUCCESS)
    {
        m_ui->label_error->setStyleSheet("");
        m_ui->label_error->clear();
        m_ui->lineEdit_ip->clear();
        m_ui->lineEdit_login->clear();
        m_ui->lineEdit_pass->clear();
        m_ui->widget_authc->hide();

        on_pushButton_chatlist_clicked();
        this->setCurrentIndex(1);
        m_splitter = m_ui->splitter_main->sizes();
    }
    else
    {
        m_ui->label_error->setStyleSheet("QLabel { border-radius:15%; background-color: rgb(190, 0, 0); color:white; }");

        switch (status)
        {
            case FAIL: m_ui->label_error->setText("Error. Connection error."); break;
            case FAIL_REGISTER: m_ui->label_error->setText("Error. Users exists."); break;
            case FAIL_AUTENTIFY: m_ui->label_error->setText("Error. Incorrect login or pass."); break;
            case ERROR_EMPTY: m_ui->label_error->setText("Error. Fill in all the fields."); break;
            case ERROR_SHORT_LOGIN: m_ui->label_error->setText("Error. Login is short."); break;
            case ERROR_SHORT_PASS: m_ui->label_error->setText("Error. Password is short."); break;
            case ERROR_BIG_LOGIN: m_ui->label_error->setText("Error. Login is big."); break;
            case ERROR_BIG_PASS: m_ui->label_error->setText("Error. Password is big."); break;
            case ERROR_WRONG_SYMBOL: m_ui->label_error->setText("Error. Available symbols: A-Z, a-z, 0-9, _, *."); break;
            case DISCONNECTED: m_ui->label_error->setText("Error. Disconnected."); break;
            default: m_ui->label_error->setText("Error. Invalid."); break;
        }

        m_ui->widget_authc->show();
        this->setCurrentIndex(0);
    }
}

// LOGIN PAGE
////////////////////////////////////////////////////

void MainWindow::on_pushButton_connect_clicked()
{
    m_client->loginUser(m_ui->lineEdit_ip->text(), m_ui->lineEdit_login->text(), m_ui->lineEdit_pass->text());
}

void MainWindow::on_pushButton_register_clicked()
{
    m_client->registerUser(m_ui->lineEdit_ip->text(), m_ui->lineEdit_login->text(), m_ui->lineEdit_pass->text());
}

// CHATLIST
////////////////////////////////////////////////////

void MainWindow::on_pushButton_logout_clicked()
{
    m_client->logoutUser();

    if (m_ui->scrollWidget_messages->layout())
    {
        clearLayout(m_ui->scrollWidget_messages->layout());
    }

    if (m_ui->scrollWidget_users->layout())
    {
        clearLayout(m_ui->scrollWidget_users->layout());
    }

    m_ui->label_talker->clear();
    m_ui->lineEdit_message->clear();
    m_ui->widget_mainarea->setCurrentIndex(1);
    m_ui->widget_authc->show();

    this->setCurrentIndex(0);
}

void MainWindow::on_pushButton_chatlist_clicked()
{
    m_client->getChatList();
}

void MainWindow::displayChatList(const QVector<User> &vec_users)
{
    // Очищаем или создаём layout
    if (m_ui->scrollWidget_users->layout())
    {
        clearLayout(m_ui->scrollWidget_users->layout());
    }
    else
    {
        m_ui->scrollWidget_users->setLayout(new QVBoxLayout());
        m_ui->scrollWidget_users->layout()->setSpacing(0);
        m_ui->scrollWidget_users->layout()->setContentsMargins(0, 0, 0, 0);
    }

    // Генерируем кнопки
    if(!vec_users.empty())
    {   for(auto &user : vec_users)
        {
            userButton *button_user = new userButton(user);
            m_ui->scrollWidget_users->layout()->addWidget(button_user);

            connect(button_user, &userButton::clicked, this, &MainWindow::getMessageHistory);
        }
    }

    m_ui->scrollWidget_users->layout()->addItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

// CHAT_AREA
////////////////////////////////////////////////////

void MainWindow::openFile()
{
  QPushButton *button = (QPushButton *)sender();
  QString filename = button->text();

  QFileInfo check_file(m_client->getFilesPath()+filename);

  if(check_file.exists())
      QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(m_client->getFilesPath()+filename)));
  else
  {
      int download = QMessageBox::question(this, "", "File does not exist. Try download?");

      if(download == QMessageBox::Yes)
        m_client->getFile(filename);
  }
}

void MainWindow::getMessageHistory()
{
    userButton *button = (userButton *)sender();
    m_ui->label_talker->setText(button->getUserName());
    m_client->setReceiver(button->getUserName());
    m_client->setReceiverOnline(button->isOnline());
    m_client->getMessageHistory();

    m_ui->widget_mainarea->setCurrentIndex(0);
}

void MainWindow::getMessageHlstory()
{
    if(this->currentIndex() == 1)
        this->setCurrentIndex(2);
    else
        this->setCurrentIndex(1);
}

void MainWindow::displayReceivedMessage(const Message &message)
{
    userMessage *obj_message = new userMessage(message);
    m_ui->scrollWidget_messages->layout()->addWidget(obj_message);

    if(obj_message->isFile())
      connect(obj_message->getFileButton(), &QPushButton::clicked, this, &MainWindow::openFile);

    m_ui->scrollArea_messages->verticalScrollBar()->setMaximum(999999999);
    m_ui->scrollArea_messages->verticalScrollBar()->setValue(m_ui->scrollArea_messages->verticalScrollBar()->maximum());
}

void MainWindow::displayMessageHistory(const QVector<Message> &vec_messages)
{
    // Очищаем или создаём layout
    if (m_ui->scrollWidget_messages->layout())
    {
        clearLayout(m_ui->scrollWidget_messages->layout());
    }
    else
    {
        m_ui->scrollWidget_messages->setLayout(new QVBoxLayout());
        m_ui->scrollWidget_messages->layout()->setSpacing(0);
        m_ui->scrollWidget_messages->layout()->setContentsMargins(0, 5, 0, 5);
    }

    m_ui->scrollWidget_messages->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

    if(!vec_messages.isEmpty())
    {
      for(auto &message : vec_messages)
      {
          userMessage *obj_message = new userMessage(message);
          m_ui->scrollWidget_messages->layout()->addWidget(obj_message);

          if(obj_message->isFile())
            connect(obj_message->getFileButton(), &QPushButton::clicked, this, &MainWindow::openFile);
        }
    }

    m_ui->scrollArea_messages->verticalScrollBar()->setMaximum(999999999);
    m_ui->scrollArea_messages->verticalScrollBar()->setValue(m_ui->scrollArea_messages->verticalScrollBar()->maximum());
}

void MainWindow::displayFileInfo(const QString& fileOwner, const QString& fileName, Status status)
{
    switch (status)
    {
        case FILE_DOWNLOADED: QMessageBox::about(this, "About", fileName+" from "+fileOwner+" is downloaded!"); break;
        case FILE_IS_ALDREADY_DOWNLOAD: QMessageBox::warning(this, "About", fileName+" from "+fileOwner+" is already downloading."); break;
        case FILE_DOWNLOAD_ERROR: QMessageBox::critical(this, "About", fileName+" from "+fileOwner+" can`t be downloaded :(( "); break;
        default: break;
    }
}

void MainWindow::on_pushButton_hidesb_clicked()
{
    if(m_ui->splitter_main->sizes()[0] > 0)
    {
        m_splitter = m_ui->splitter_main->sizes();
        m_ui->splitter_main->setSizes(QList<int> {0, m_splitter[1]});
        m_ui->pushButton_hidesb->setIcon(QIcon(":/resources/img/unhide_button_v1.png"));
    }
    else
    {
        m_ui->splitter_main->setSizes(m_splitter);
        m_ui->pushButton_hidesb->setIcon(QIcon(":/resources/img/hide_button_v1.png"));
    }
}

void MainWindow::on_pushButton_send_clicked()
{
    if(m_ui->lineEdit_message->text().isEmpty())
        return;

    QDateTime dt = QDateTime::currentDateTimeUtc();

    Message message;
    message.sender = m_client->getLogin();
    message.receiver = m_client->getReceiver();
    message.content = m_ui->lineEdit_message->text();
    message.datetime = dt.toTime_t();
    message.file = false;

    m_client->sendMessage(message);
    m_ui->lineEdit_message->clear();
    m_ui->scrollWidget_messages->layout()->addWidget(new userMessage(message));

    m_ui->scrollArea_messages->verticalScrollBar()->setMaximum(999999999);
    m_ui->scrollArea_messages->verticalScrollBar()->setValue(m_ui->scrollArea_messages->verticalScrollBar()->maximum());
}

void MainWindow::on_pushButton_attach_clicked()
{
    // Копируем отправляемый файл в единую директорию для цельности переписки
    QFileInfo file(QFileDialog::getOpenFileName(this));

    if(file.fileName().isEmpty())
        return;

    QFile::copy(file.filePath(), m_client->getFilesPath()+file.fileName());

    QDateTime dt = QDateTime::currentDateTimeUtc();

    Message message;
    message.sender = m_client->getLogin();
    message.receiver = m_client->getReceiver();
    message.content = file.fileName();
    message.datetime = dt.toTime_t();
    message.file = true;

    m_client->sendMessage(message);

    userMessage *obj_message = new userMessage(message);
    m_ui->scrollWidget_messages->layout()->addWidget(obj_message);

    connect(obj_message->getFileButton(), &QPushButton::clicked, this, &MainWindow::openFile);

    m_ui->scrollArea_messages->verticalScrollBar()->setMaximum(999999999);
    m_ui->scrollArea_messages->verticalScrollBar()->setValue(m_ui->scrollArea_messages->verticalScrollBar()->maximum());
}

void MainWindow::on_splitter_main_splitterMoved(int pos, __attribute__((unused)) int index)
{
    if(pos == 0)
        m_ui->pushButton_hidesb->setIcon(QIcon(":/resources/img/unhide_button_v1.png"));
    else
        m_ui->pushButton_hidesb->setIcon(QIcon(":/resources/img/hide_button_v1.png"));
}


//              Най поки тут буде

//    // Получение даты и времени в Unix формате,
//    // используем UTC, общий часовой пояс
//    QDateTime dt = QDateTime::currentDateTimeUtc();
//    int datetime = dt.toTime_t();

//    // Из Unix формата назад в обычное время
//    // toLocalTime, если нужно "твой" часовой пояс
//    dt.setTime_t(datetime);
//    dt.toLocalTime().toString(" | dd.MM.yyyy  | hh:mm \n");


//::::*==+=+===+=+=+++===+++==+:-:+==%%:+%@@@@@@@#@@@##@@@@@@@@**%@@@@%%:+%===**=#####%=++++++====++++++==*
//----++++++++++++++++**+++=+==:-:*++++-*+++++=@@@@@@@#@+=%=+++*:+++++++-+%@########@=++==++=++=++==++=++=+
//---:==+++=++==+=+=+=++======###@%%=**-:++++++%@@#@@@@#@#+*+=%=******=@##########@=======================%
//---*==+=+=+=+=+=+=+===+=+=+=@####@@@@@@#=+==%@##############=::***%=%##########%==========+=========+===%
//---:::::::::::::::::::::::::*#########@##@@@###%+:%#########@%%=%%=%#########%:::::::::::::::::::::::::::
//.---------------------------*#####@%:::::*##*:::----*@=%#####%=====########@*:---------------------------
//.--------:+++*++++++++*:---%######::+*::::#@:::=@:----:=====%#@===########=**:------::+++++++++++++:-----
//..------:+*:*************:+--:###=:@##+-::%+:-@##@-----==========@@@%%@#@+***::----:***************+:----
//..-------**************+:-----:+%::=#%:..-=:.-:=@:-----+===========%%%@%::::*::----:*++************:-----
//-.------------::::::::--------:*+::::::::*=+*::------:*==============%%%****+::-------::::::::::::-------
//-..----------------------**---:*+%%==%@@@@@@@@@@@%%%===================%****+*:--------------------------
//-..------------------:%######=**%%@@@@@@@@@@@@@@%=*::::::::::::::::+====+************:-------------------
//--..:::::**********+#############@@@@@@@@@%+*:::::::::::::::::::::**=======*************************:::::
//**++*****************###############%=***::::::::::::::::::::::-:::========%+****************************
//**********************%##########@=+******************::::::..--::+==========****************************
//***********************+######=*+++++*****************:-....:.-::*==========%+***************************
//---------:---------------=*------:+++++++++++*::-.........:*.-::*=============-:-:---:--::---------:-----
//-----------------------------------*.....................=..-:::=============%:--------------------------
//-----------------------------------+...................=:..-:::+=============%+--------------------------
//-----------------------------------**................*%...-:::*===============%:-------------------------
//-----------------------------------:=.....----::::***-...-::::+===============%+-------------------------
//-----------------------------------:=-...................::::*+=====%@%========%-------------------------
//-----------------------------------:%*..................::::*+======@@@@=======%+------------------------
//-----------------------------------+%+*++===+++*****:--:::::++=====%@@#=:=======%+-----------------------
//----------------------------------+%%+***********::::::::::*+======@@@%---======%==----------------------
//---------------------------------*%%=+***********:::::::::*++=====%@@%-----=====%==%---------------------
//--------------------------------:%%=====%*========++*:::::+++=====@@@-------+%%=====*--------------------
//--------------------------------+%%====%+-+==========+++++++=====%@@:-------:%%======--------------------
//--------------------------------=%%====%--:=========++++++++=====@@*---------=%======:-------------------
//::::::::::::::::::::::::::::::::%%%%%%@*:::=====%%%=========%%%%%@=::::::::::+%%%%%%%*:::::::::::::::::::
