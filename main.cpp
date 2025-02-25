#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Open SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("colleges.db");  // Change to your database file

    if (!db.open()) {
        qDebug() << "Error: Could not open database!" << db.lastError().text();
        return -1;
    }

    // Query data
    QSqlQuery query;
    if (!query.exec("SELECT * FROM colleges;")) {
        qDebug() << "Query failed: " << query.lastError().text();
        return -1;
    }

    // Display results
    while (query.next()) {
        QString start = query.value(0).toString();
        QString end = query.value(1).toString();
        double distance = query.value(2).toDouble();

        qDebug() << "Start:" << start << "| End:" << end << "| Distance:" << distance;
    }

    db.close();
    return app.exec();
}
