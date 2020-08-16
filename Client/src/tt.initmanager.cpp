#include "tt.initmanager.h"

#include <QHostAddress>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>

#include "src/tt.comunicationmanager.h"

TTInitManager::TTInitManager(QObject *parent)
    : QObject(parent)
{

    TTComunicationManager::instance();
    QCoreApplication::setOrganizationName("Hvorovk");
    QCoreApplication::setApplicationName("Chat client");

}

void TTInitManager::init(QQmlApplicationEngine &engine)
{
    engine.rootContext()->setContextProperty(QLatin1String("comunicationManager"),
                                             &TTComunicationManager::instance());
}
