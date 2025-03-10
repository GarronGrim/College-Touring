#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString& dbPath) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
    } else {
        initializeTables();
    }
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

void DatabaseManager::initializeTables() {
    QSqlQuery query;
    
    // Create the Distances table with a composite primary key
    if (!query.exec("CREATE TABLE IF NOT EXISTS Distances ("
                    "start_college TEXT NOT NULL, "
                    "end_college TEXT NOT NULL, "
                    "distance REAL NOT NULL, "
                    "PRIMARY KEY (start_college, end_college))")) {
        qDebug() << "Failed to create Distances table:" << query.lastError().text();
    }
    
    // Create the Souvenirs table with a composite primary key
    if (!query.exec("CREATE TABLE IF NOT EXISTS Souvenirs ("
                    "college TEXT NOT NULL, "
                    "souvenir TEXT NOT NULL, "
                    "price REAL NOT NULL, "
                    "PRIMARY KEY (college, souvenir))")) {
        qDebug() << "Failed to create Souvenirs table:" << query.lastError().text();
    }
}

bool DatabaseManager::importCSV(const QString &filePath, const QString &tableName, const QStringList &columns) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open" << filePath;
        return false;
    }
    
    QTextStream in(&file);
    
    QSqlQuery query;
    while (!in.atEnd()) {
        QString line = in.readLine();
        // Use the CSV parser to properly split the line.
        QStringList values = parseCSVLine(line);
        
        // Check if the number of fields matches the number of columns.
        if (values.size() != columns.size()) {
            qDebug() << "Skipping line due to field count mismatch:" << line;
            continue;
        }
        
        // Construct placeholders for prepared statement, e.g., (?, ?, ?)
        QStringList phList;
        for (int i = 0; i < columns.size(); i++) {
            phList << "?";
        }
        QString placeholders = "(" + phList.join(", ") + ")";
        
        // Construct the full INSERT statement.
        QString sql = QString("INSERT OR IGNORE INTO %1 (%2) VALUES %3")
                          .arg(tableName)
                          .arg(columns.join(", "))
                          .arg(placeholders);
        query.prepare(sql);
        
        // Bind values to the prepared statement.
        for (const QString &value : values) {
            query.addBindValue(value.trimmed());
        }
        
        if (!query.exec()) {
            qDebug() << "Insert failed:" << query.lastError().text();
        }
    }
    file.close();
    return true;
}

QStringList DatabaseManager::parseCSVLine(const QString &line) {
    QStringList result;
    QString current;
    bool inQuotes = false;
    
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];
        if (c == '\"') {
            // Toggle the inQuotes flag unless it's an escaped quote.
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '\"') {
                // Escaped quote, add one quote and skip the next character.
                current.append('\"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // Field separator found outside of quotes.
            result.append(current);
            current.clear();
        } else {
            current.append(c);
        }
    }
    // Append the last field.
    result.append(current);
    return result;
}

std::vector<QString> DatabaseManager::getColleges() {
    std::vector<QString> colleges;
    // Now only selecting distinct start_college from the Distances table.
    QSqlQuery query("SELECT DISTINCT start_college FROM Distances");
    while (query.next()) {
        colleges.push_back(query.value(0).toString());
    }
    return colleges;
}

std::vector<std::pair<QString, double>> DatabaseManager::getDistances(const QString& college) {
    std::vector<std::pair<QString, double>> distances;
    QSqlQuery query;
    query.prepare("SELECT end_college, distance FROM Distances WHERE start_college = ?");
    query.addBindValue(college);
    if (query.exec()) {
        while (query.next()) {
            distances.emplace_back(query.value(0).toString(), query.value(1).toDouble());
        }
    } else {
        qDebug() << "getDistances query failed:" << query.lastError().text();
    }
    return distances;
}

std::vector<std::pair<QString, double>> DatabaseManager::getSouvenirs(const QString& college) {
    std::vector<std::pair<QString, double>> souvenirs;
    QSqlQuery query;
    query.prepare("SELECT souvenir, price FROM Souvenirs WHERE college = ?");
    query.addBindValue(college);
    if (query.exec()) {
        while (query.next()) {
            souvenirs.emplace_back(query.value(0).toString(), query.value(1).toDouble());
        }
    } else {
        qDebug() << "getSouvenirs query failed:" << query.lastError().text();
    }
    return souvenirs;
}

bool DatabaseManager::updateSouvenirPrice(const QString& souvenir, double newPrice) {
    QSqlQuery query;
    query.prepare("UPDATE Souvenirs SET price = ? WHERE souvenir = ?");
    query.addBindValue(newPrice);
    query.addBindValue(souvenir);
    if (!query.exec()) {
        qDebug() << "updateSouvenirPrice failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addSouvenir(const QString& college, const QString& souvenir, double price) {
    QSqlQuery query;
    query.prepare("INSERT INTO Souvenirs (college, souvenir, price) VALUES (?, ?, ?)");
    query.addBindValue(college);
    query.addBindValue(souvenir);
    query.addBindValue(price);
    if (!query.exec()) {
        qDebug() << "addSouvenir failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::removeSouvenir(const QString& souvenir) {
    QSqlQuery query;
    query.prepare("DELETE FROM Souvenirs WHERE souvenir = ?");
    query.addBindValue(souvenir);
    if (!query.exec()) {
        qDebug() << "removeSouvenir failed:" << query.lastError().text();
        return false;
    }
    return true;
}

void DatabaseManager::addCollegeDistanceMaintenance(const QString &startCollege, const QString &endCollege, double distance) {
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Distances WHERE start_college = :start AND end_college = :end");
    checkQuery.bindValue(":start", startCollege);
    checkQuery.bindValue(":end", endCollege);

    if (!checkQuery.exec()) {
        qDebug() << "Error checking for existing pair:" << checkQuery.lastError();
        return;
    }

    checkQuery.next();
    int count = checkQuery.value(0).toInt();

    if (count == 0) {
        // If the combination doesn't exist, insert the new record
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO Distances (start_college, end_college, distance) VALUES (:start, :end, :distance)");
        insertQuery.bindValue(":start", startCollege);
        insertQuery.bindValue(":end", endCollege);
        insertQuery.bindValue(":distance", distance);

        if (!insertQuery.exec()) {
            qDebug() << "Error inserting distance:" << insertQuery.lastError();
        }
    } else {
        // If the combination exists, update the record
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE Distances SET distance = :distance WHERE start_college = :start AND end_college = :end");
        updateQuery.bindValue(":start", startCollege);
        updateQuery.bindValue(":end", endCollege);
        updateQuery.bindValue(":distance", distance);

        if (!updateQuery.exec()) {
            qDebug() << "Error updating distance:" << updateQuery.lastError();
        }
    }
}


void DatabaseManager::addSouvenirMaintenance(const QString &collegeName, const QString &souvenirName, double price) {
    // Check if the souvenir already exists for the given college
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Souvenirs WHERE college = :college AND souvenir = :souvenir");
    checkQuery.bindValue(":college", collegeName);
    checkQuery.bindValue(":souvenir", souvenirName);

    if (!checkQuery.exec()) {
        qDebug() << "Error checking for existing souvenir:" << checkQuery.lastError();
        return;
    }

    checkQuery.next();
    int count = checkQuery.value(0).toInt();

    if (count == 0) {
        // If the souvenir doesn't exist, insert the new record
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO Souvenirs (college, souvenir, price) VALUES (:college, :souvenir, :price)");
        insertQuery.bindValue(":college", collegeName);
        insertQuery.bindValue(":souvenir", souvenirName);
        insertQuery.bindValue(":price", price);

        if (!insertQuery.exec()) {
            qDebug() << "Failed to insert souvenir:" << insertQuery.lastError();
        }
    } else {
        // If the souvenir exists, update the record
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE Souvenirs SET price = :price WHERE college = :college AND souvenir = :souvenir");
        updateQuery.bindValue(":college", collegeName);
        updateQuery.bindValue(":souvenir", souvenirName);
        updateQuery.bindValue(":price", price);

        if (!updateQuery.exec()) {
            qDebug() << "Error updating souvenir:" << updateQuery.lastError();
        }
    }
}


void DatabaseManager::updateSouvenir(const QString &college, const QString &souvenir, double newPrice) {
    QSqlQuery query;
    query.prepare("UPDATE Souvenirs SET price = :price WHERE college = :college AND souvenir = :souvenir");
    query.bindValue(":price", newPrice);
    query.bindValue(":college", college);
    query.bindValue(":souvenir", souvenir);

    if (!query.exec()) {
        qDebug() << "Failed to update souvenir price:" << query.lastError();
    } else {
        qDebug() << "Souvenir price updated successfully.";
    }
}

void DatabaseManager::updateDistance(const QString &startCollege, const QString &endCollege, double newDistance) {
    QSqlQuery query;
    query.prepare("UPDATE Distances SET distance = :distance WHERE start_college = :start AND end_college = :end");
    query.bindValue(":distance", newDistance);
    query.bindValue(":start", startCollege);
    query.bindValue(":end", endCollege);


    if (!query.exec()) {
        qDebug() << "Failed to update distance:" << query.lastError();
    } else {
        qDebug() << "Distance updated successfully.";
    }
}

void DatabaseManager::updateCollegeName(const QString &oldName, const QString &newName) {
    QSqlQuery query;

    // Update the name in the Distances table (both start_college and end_college)
    query.prepare("UPDATE Distances SET start_college = :newName WHERE start_college = :oldName");
    query.bindValue(":newName", newName);
    query.bindValue(":oldName", oldName);

    if (!query.exec()) {
        qDebug() << "Failed to update start_college name:" << query.lastError();
    }

    query.prepare("UPDATE Distances SET end_college = :newName WHERE end_college = :oldName");
    query.bindValue(":newName", newName);
    query.bindValue(":oldName", oldName);

    if (!query.exec()) {
        qDebug() << "Failed to update end_college name:" << query.lastError();
    }

    // Update the name in the Souvenirs table
    query.prepare("UPDATE Souvenirs SET college = :newName WHERE college = :oldName");
    query.bindValue(":newName", newName);
    query.bindValue(":oldName", oldName);

    deleteCollege(oldName);

    if (!query.exec()) {
        qDebug() << "Failed to update college name in Souvenirs table:" << query.lastError();
    } else {
        qDebug() << "College name updated successfully in all tables.";
    }
}

void DatabaseManager::deleteCollege(const QString &college) {
    QSqlQuery query;

    // First, delete all distances involving this college
    query.prepare("DELETE FROM Distances WHERE start_college = :college OR end_college = :college");
    query.bindValue(":college", college);

    if (!query.exec()) {
        qDebug() << "Failed to delete distances for college:" << query.lastError();
    }

    // Then, delete all souvenirs associated with this college
    query.prepare("DELETE FROM Souvenirs WHERE college = :college");
    query.bindValue(":college", college);

    if (!query.exec()) {
        qDebug() << "Failed to delete souvenirs for college:" << query.lastError();
    }

    qDebug() << "College and associated data deleted successfully.";
}

void DatabaseManager::deleteDistance(const QString &startCollege, const QString &endCollege) {
    QSqlQuery query;
    query.prepare("DELETE FROM Distances WHERE start_college = :start AND end_college = :end");
    query.bindValue(":start", startCollege);
    query.bindValue(":end", endCollege);

    if (!query.exec()) {
        qDebug() << "Failed to delete distance:" << query.lastError();
    } else {
        qDebug() << "Distance entry deleted successfully.";
    }
}

void DatabaseManager::deleteSouvenir(const QString &college, const QString &souvenir) {
    QSqlQuery query;
    query.prepare("DELETE FROM Souvenirs WHERE college = :college AND souvenir = :souvenir");
    query.bindValue(":college", college);
    query.bindValue(":souvenir", souvenir);

    if (!query.exec()) {
        qDebug() << "Failed to delete souvenir:" << query.lastError();
    } else {
        qDebug() << "Souvenir deleted successfully.";
    }
}