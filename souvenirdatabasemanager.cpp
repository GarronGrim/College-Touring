#include "souvenirdatabasemanager.h"

SouvenirDatabaseManager::SouvenirDatabaseManager(QObject *parent)
    : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE", "souvenirs_connection");
    db.setDatabaseName("C:/sqliteDatabases/souvenirs/souvenirs.db"); // Ensure correct path if needed

    if (!db.open()) {
        qDebug() << "Souvenir Database connection failed:" << db.lastError().text();
    } else {
        qDebug() << "Souvenir Database connected!";
    }
}

SouvenirDatabaseManager::~SouvenirDatabaseManager() {
    closeDatabase();
}

bool SouvenirDatabaseManager::openDatabase() {
    if (!db.open()) {
        qDebug() << "Error opening souvenirs database:" << db.lastError().text();
        return false;
    }
    return true;
}

void SouvenirDatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}


QVariantList SouvenirDatabaseManager::fetchSouvenirs(const QString &collegeName) {
    QVariantList souvenirsList;

    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return souvenirsList;
    }

    QSqlQuery query(db);
    query.prepare("SELECT souvenir, price FROM souvenirs WHERE college = ?");
    query.addBindValue(collegeName);

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return souvenirsList;
    }

    while (query.next()) {
        QVariantMap souvenir;
        souvenir["souvenir"] = query.value(0).toString();
        souvenir["price"] = query.value(1).toString(); // Ensure price is returned as string
        souvenirsList.append(souvenir);
    }

    return souvenirsList;
}
