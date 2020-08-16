#ifndef TTINITMANAGER_H
#define TTINITMANAGER_H

#include <QObject>

class QQmlApplicationEngine;

class TTInitManager : public QObject
{
    Q_OBJECT
public:
    explicit TTInitManager(QObject *parent = nullptr);

    void init(QQmlApplicationEngine &engine);
};

#endif // TTINITMANAGER_H
