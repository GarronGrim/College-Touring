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

    // query.exec("DROP TABLE IF EXISTS Distances");
    // query.exec("DROP TABLE IF EXISTS Souvenirs");
    
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

QStringList parseCSVLine(const QString &line) {
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

std::vector<QString> DatabaseManager::getColleges() {
    std::vector<QString> colleges;
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

double DatabaseManager::getDistance(const QString& startCollege, const QString& endCollege) {
    double distance = std::numeric_limits<double>::max();
    QSqlQuery query;
    query.prepare("SELECT distance FROM Distances WHERE start_college = ? AND end_college = ?");
    query.addBindValue(startCollege);
    query.addBindValue(endCollege);
    if(query.exec() && query.next()) {
        distance = query.value(0).toDouble();
    } else {
        qDebug() << "getDistance query failed:" << query.lastError().text();
    }
    return distance;
}

bool DatabaseManager::importNewCampuses(const QString &filePath) {
    QStringList columns = {"start_college", "end_college", "distance"};
    return importCSV(filePath, "Distances", columns);
}

void DatabaseManager::dropTables() {
    QSqlQuery query;
    
    query.exec("DROP TABLE IF EXISTS Distances");
    query.exec("DROP TABLE IF EXISTS Souvenirs");
}