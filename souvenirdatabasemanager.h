#ifndef SOUVENIRDATABASEMANAGER_H
#define SOUVENIRDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>

class SouvenirDatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit SouvenirDatabaseManager(QObject *parent = nullptr);
    ~SouvenirDatabaseManager();

    bool openDatabase();
    void closeDatabase();
    Q_INVOKABLE QVariantList fetchSouvenirs(const QString &collegeName);

private:
    QSqlDatabase db;
};

#endif // SOUVENIRDATABASEMANAGER_H
