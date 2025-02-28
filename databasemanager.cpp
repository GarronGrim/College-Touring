#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/sqliteDatabases/Colleges/colleges.db");

    if (!db.open()) {
        qDebug() << "College Distances Database connection failed:" << db.lastError().text();
    } else {
        qDebug() << "College Distances Database connected!";
    }
}

QVariantList DatabaseManager::fetchColleges() {
    QVariantList colleges;
    QSet<QString> uniqueColleges;  // Ensure unique entries

    QSqlQuery query("SELECT DISTINCT starting_college FROM colleges"); // Ensure DISTINCT is used
    if (query.exec()) {
        while (query.next()) {
            QString collegeName = query.value(0).toString();
            if (!uniqueColleges.contains(collegeName)) {
                uniqueColleges.insert(collegeName);
                QVariantMap college;
                college["name"] = collegeName;
                colleges.append(college);
            }
        }
    } else {
        qDebug() << "Database query failed:" << query.lastError().text();
    }

    return colleges;
}

QVariantList DatabaseManager::fetchDistances(const QString &collegeName) {
    QVariantList dataList;
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return dataList;
    }

    QSqlQuery query;
    query.prepare("SELECT ending_college, distance FROM colleges WHERE starting_college = :collegeName");
    query.bindValue(":collegeName", collegeName);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        return dataList;
    }

    while (query.next()) {
        QVariantMap record;
        record["ending_college"] = query.value(0).toString();
        record["distance"] = query.value(1).toDouble();
        dataList.append(record);
    }

    qDebug() << "Distances loaded for" << collegeName << ":" << dataList.size();
    return dataList;
}

QVariantList DatabaseManager::loadCollegeDistances(const QString &startingCollege) {
    QVariantList result;
    QSqlQuery query(db);
    query.prepare("SELECT ending_college, distance FROM colleges WHERE starting_college = :startingCollege");
    query.bindValue(":startingCollege", startingCollege);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap row;
            row["ending_college"] = query.value(0).toString();
            row["distance"] = query.value(1).toInt();
            result.append(row);
        }
    } else {
        qDebug() << "SQL Error: " << query.lastError().text();
    }
    return result;
}
