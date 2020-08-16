#include "tt.comunicationmanager.h"

const QString k_serverAddr = "localhost";
const int k_serverPort = 8080;

TTComunicationManager::TTComunicationManager(QObject *parent)
    : QObject(parent),
      m_serverConnection(false)
{
    connect(&m_socket, &QTcpSocket::connected, this, &TTComunicationManager::connected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &TTComunicationManager::disconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &TTComunicationManager::processServerResponse);
    connect(&m_socket, &QTcpSocket::stateChanged, [this](QAbstractSocket::SocketState state) {
        switch (state) {
        case QAbstractSocket::SocketState::ConnectedState:
            update_serverConnection(true);
            m_reconnectTimer.stop();
            break;
        default:
            update_serverConnection(false);
            m_reconnectTimer.start();
        }
    });

    m_reconnectTimer.setInterval(5000);
    m_reconnectTimer.stop();

    connect(&m_reconnectTimer, &QTimer::timeout, this, &TTComunicationManager::restartConnection);
}

void TTComunicationManager::registration(QString login, QString password)
{
    sendDataToServer("[REGISTER]" + login + "|" + password);
}

void TTComunicationManager::login(QString login, QString password)
{
    sendDataToServer(login + "|" + password);
}

void TTComunicationManager::logout()
{
    emit loggedOut();
    sendDataToServer("[LOGOUT]");
    update_loginString("");
}

void TTComunicationManager::sendMessage(QString message, QString scope)
{
    sendDataToServer(scope + "[" + get_loginString() + "]" + message + "[MESSAGEEND]\n");
}

void TTComunicationManager::connectToServer()
{
    m_socket.connectToHost(k_serverAddr, k_serverPort);
}

void TTComunicationManager::processServerResponse()
{
    QStringList newData = QString::fromUtf8(m_socket.readAll()).split("\n", QString::SplitBehavior::SkipEmptyParts);

    qDebug() << newData;

    QString buffer;
    bool isMessage = false;

    for (QString dataLine : newData) {
        if (isMessage) {
            buffer += "\n" + dataLine;
            if (dataLine.endsWith("[MESSAGEEND]")) {
                globalSectionProcessing(buffer);
                isMessage = false;
            }
            continue;
        }

        if (authProccessing(dataLine)) {
            continue;
        }
        if (registerProccessing(dataLine)) {
            continue;
        }
        if (dataLine.startsWith("[GLOBAL]")) {
            if (!dataLine.contains("[MESSAGEEND]")) {
                isMessage = true;
                buffer.clear();
                buffer.append(dataLine.replace("[GLOBAL]", ""));
                continue;
            }
            globalSectionProcessing(dataLine.replace("[GLOBAL]", ""));
            continue;
        }
    }
}

bool TTComunicationManager::authProccessing(QString response)
{
    if (response == "[BadAuth]")
        emit authError(0);
    else if (response == "[UserNotExist]")
        emit authError(1);
    else if (response == "[WrongPassOrLogin]")
        emit authError(2);
    else if (response == "[AlreadyAuthorized]")
        emit authError(3);
    else if (response.startsWith("[SuccessAuth]")) {
        update_loginString(response.replace("[SuccessAuth]", ""));
        emit authSuccess();
    } else
        return false;
    return true;
}

void TTComunicationManager::globalSectionProcessing(QString response)
{
    if (response.startsWith("[System]")) {
        return emit systemMessage(response.remove("[System]").remove("[MESSAGEEND]"));
    }

    int endLoginSection = response.indexOf("]");

    emit globalMessageGot(response.remove(0, endLoginSection + 1).remove("[MESSAGEEND]"), response.left(endLoginSection).remove(0,1));
}

bool TTComunicationManager::registerProccessing(QString response)
{
    if (response == "[BadRegister]")
        emit registrationError(0);
    else if (response == "[AlreadyExist]")
        emit registrationError(1);
    else if (response == "[ShortPassword]")
        emit registrationError(2);
    else if (response == "[HaveRestrictedSymbols]")
        emit registrationError(3);
    else if (response == "[SuccessRegistration]")
        emit registrationSuccess();
    else
        return false;
    return true;
}

void TTComunicationManager::sendDataToServer(QString data)
{
    if (m_socket.write(data.toUtf8()) == -1) {
        emit sendingError();
    }
}

void TTComunicationManager::restartConnection()
{
    connectToServer();
}

