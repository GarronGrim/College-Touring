#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "DatabaseManager.h"
#include "SouvenirDatabaseManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // College distances database manager
    DatabaseManager dbManager;

    // Souvenirs database manager
    SouvenirDatabaseManager souvenirDB;

    // Expose both database managers to QML
    engine.rootContext()->setContextProperty("dbManager", &dbManager);
    engine.rootContext()->setContextProperty("souvenirDB", &souvenirDB);

    // Load the QML UI
    engine.loadFromModule("College", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
