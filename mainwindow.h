#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QColor>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <unordered_set>
#include "DatabaseManager.h"

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

    void updateSouvenirList(const QString &college);
    
    // When the lock button is pressed, disable further clicking.
    void onLockButtonClicked();
    void onUnlockButtonClicked();
    void onNextButtonClicked();
    // Imports new colleges
    void on_importButton_clicked();

private:
    Ui::MainWindow *ui;
    DatabaseManager *dbManager;
    bool listLocked;
    void onListWidgetContextMenuRequested(const QPoint &pos);
    void toggleItemHighlight(QListWidgetItem* item);
    std::vector<QString> highlightedCollegeNames;
    int currentIndex = 0; // To keep track of the current college being displayed
    void unlockList();
    void updateCollegeComboBox();
    void updateDistanceList(const QString &college);
};

#endif // MAINWINDOW_H