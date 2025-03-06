#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <vector>
#include <utility>
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringList>
#include <limits>
#include <algorithm>

class DatabaseManager {
public:
    // Constructor: opens the database file (default: campus.db)
    DatabaseManager(const QString& dbPath = "campus.db");
    
    // Destructor: closes the database connection if open
    ~DatabaseManager();

    // Returns a list of unique college names from both tables
    std::vector<QString> getColleges();

    // Given a college name, returns a list of (end_college, distance) pairs
    std::vector<std::pair<QString, double>> getDistances(const QString& college);

    // Given a college name, returns a list of (souvenir, price) pairs
    std::vector<std::pair<QString, double>> getSouvenirs(const QString& college);

    // Update the price of a souvenir; returns true if successful
    bool updateSouvenirPrice(const QString& souvenir, double newPrice);

    // Adds a new souvenir for a given college; returns true if successful
    bool addSouvenir(const QString& college, const QString& souvenir, double price);

    // Removes a souvenir by name; returns true if successful
    bool removeSouvenir(const QString& souvenir);

    // Initial import from csv files 
    bool importCSV(const QString& filePath, const QString& tableName, const QStringList& columns);

private:
    QSqlDatabase db;
    void initializeTables();
};

#endif // DATABASEMANAGER_H