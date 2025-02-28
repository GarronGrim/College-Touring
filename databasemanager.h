#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>
#include <QDebug>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);

    Q_INVOKABLE QVariantList fetchColleges(); // Fetch list of all colleges
    Q_INVOKABLE QVariantList fetchDistances(const QString &collegeName); // Fetch distances from a selected college
    Q_INVOKABLE QVariantList loadCollegeDistances(const QString &startingCollege);


private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
