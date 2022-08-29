#include "CommandsTypes.h"

const char* key = "PRIVATE_KEY";

bool isCommandToServer(CommandsToServer cmd)
{
    bool result = false;

    switch(cmd)
    {
    case CommandsToServer::GET_ONLINE_USERS:
    case CommandsToServer::GET_ALL_USERS:
    case CommandsToServer::IDENTIFY:
    case CommandsToServer::REGISTER:
    {
        result = true;
        break;
    }
    }

    return result;
}


CommandsToClient getCommandToClient(CommandsToServer command)
{
    CommandsToClient result = CommandsToClient::NONE;

    switch(command)
    {
    case CommandsToServer::GET_MESSAGE_HISTORY:
    {
        result = CommandsToClient::GET_MESSAGE_HISTORY;
        break;
    }
    case CommandsToServer::SEND_MESSAGE:
    {
        result = CommandsToClient::SEND_MESSAGE;
        break;
    }
    case CommandsToServer::SEND_MESSAGE_HISTORY:
    {
        result = CommandsToClient::SEND_MESSAGE_HISTORY;
        break;
    }
    case CommandsToServer::GET_FILE:
    {
        result = CommandsToClient::GET_FILE;
        break;
    }
    case CommandsToServer::SEND_FILE:
    {
        result = CommandsToClient::SEND_FILE;
        break;
    }
    case CommandsToServer::FILE_GETTING_ERROR:
    {
        result = CommandsToClient::FILE_GETTING_ERROR;
        break;
    }
    }

    return result;
}

void encodeDecode(QByteArray &data)
{
//       AES_ctx ctx;

//       uint8_t key[] = "3s6v9y$B&E)H@McQ";
//       uint8_t iv[]  = "Zr4u7x!z%C*F-JaN";

//       int buffer_size = data.size();

//       AES_init_ctx_iv(&ctx, key, iv);
//       AES_CTR_xcrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(data.data()), buffer_size);

//    // XOR
//    int keySize = strlen(key);

//    for(int i = 0; i<data.size(); i++)
//    {
//        data[i]=data[i]^key[i%keySize];
//    }
}

QString getUnconflictedNewFileName(const QString &fileName)
{
    QString result = fileName;

    QString wtf = QCoreApplication::applicationDirPath();

    if(QFile::exists(fileName))
    {
        QString extension;

        int index = result.lastIndexOf('.');

        if(index != -1)
        {
            int extensionSize = result.size() - index;
            extension = result.right(extensionSize);
            result.remove(index, extensionSize);
        }

        int i = 1;

        while(QFile::exists(fileName + '(' + QString::number(i) + ')' + extension))
        {
            ++i;
        }

        result+='(' + QString::number(i) + ')' + extension;
    }

    return  result;
}
