#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QColor>
#include <QListView>
#include <unordered_map>
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>
#include <unordered_set>
#include "DatabaseManager.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // When the combo box selection changes, update the list of distances.
    void onCollegeChanged(const QString &college);
    // When a list item is clicked, toggle its background color.
    void onDistanceItemClicked(QListWidgetItem *item);
    // When the lock button is pressed, disable further clicking.
    void onLockButtonClicked();

    void onSouvenirItemClicked(QListWidgetItem *item);

    void onMaintenanceButtonClicked();

    std::vector<std::pair<QString, double>> calculateShortestTrip(DatabaseManager* dbManager, const QString& startCollege);

private:
    Ui::MainWindow *ui;
    DatabaseManager *dbManager;
    bool listLocked;
    QMap<QString, QPair<int, double>> cart;
    void updateCartView();
    void populateColleges();
    void updateDistanceList(const QString &college);
    void updateSouvenirsList(const QString &college);
    void openEditDataDialog();
    void editCollegeNames();
    void editDistances();
    void editSouvenirs();
    void deleteEntries();
};

#endif // MAINWINDOW_H