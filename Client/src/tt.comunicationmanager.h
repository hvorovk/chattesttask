#ifndef TTCOMUNICATIONMANAGER_H
#define TTCOMUNICATIONMANAGER_H

#include <QTcpSocket>
#include <QTimer>
#include "src/qqmlhelpers.h"

class TTComunicationManager : public QObject
{
    Q_OBJECT
    SINGLETON(TTComunicationManager)
    TTComunicationManager(QObject *parent = nullptr);
    virtual ~TTComunicationManager() {}

    QML_READONLY_PROPERTY(QString, loginString);
    QML_READONLY_PROPERTY(bool, serverConnection);

signals:
    void connected();
    void disconnected();
    void authSuccess();
    void registrationSuccess();
    void authError(int reason); // Change int to enum
    void registrationError(int reason); // Change int to enum
    void globalMessageGot(QString message, QString sender);
    void systemMessage(QString message);
    void connectionError();
    void loggedOut();
    void sendingError();

public slots:
    void registration(QString login, QString password);
    void login(QString login, QString password);
    void logout();

    void sendMessage(QString message, QString scope = "[GLOBAL]");

    void connectToServer();

private slots:
    void processServerResponse();
    bool authProccessing(QString response);
    void globalSectionProcessing(QString response);
    bool registerProccessing(QString response);
    void sendDataToServer(QString data);
    void restartConnection();

private:
    QTimer m_reconnectTimer;
    QTcpSocket m_socket;
};

#endif // TTCOMUNICATIONMANAGER_H
