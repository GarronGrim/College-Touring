#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TripPlanner.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <algorithm>   
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), listLocked(false)
{
    ui->setupUi(this);

    // Initialize DatabaseManager (assumes campus.db is in the working directory (build)).
    dbManager = new DatabaseManager("campus.db");
//    listWidgetSelectedColleges = findChild<QListWidget*>("listWidgetSelectedColleges");

    QString appDir = QCoreApplication::applicationDirPath();
    QString distancesFile = appDir + "/collegedistances.csv";
    QString souvenirsFile = appDir + "/souvenirslist.csv";

    QStringList distanceColumns = {"start_college", "end_college", "distance"};
    QStringList souvenirColumns = {"college", "souvenir", "price"};

    if (dbManager->importCSV(distancesFile, "Distances", distanceColumns)) {
        qDebug() << "Imported distances successfully.";
    } else {
        qDebug() << "Failed to import distances.";
    }

    if (dbManager->importCSV(souvenirsFile, "Souvenirs", souvenirColumns)) {
        qDebug() << "Imported souvenirs successfully.";
    } else {
        qDebug() << "Failed to import souvenirs.";
    }

    // Populate the combo box with colleges from the database.
    updateCollegeComboBox();

    // Connect signals:
    ui->listWidgetDistances->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidgetDistances, &QListWidget::customContextMenuRequested, 
            this, &MainWindow::onListWidgetContextMenuRequested);
    connect(ui->comboBoxColleges, &QComboBox::currentTextChanged,
            this, &MainWindow::onCollegeChanged);
    connect(ui->listWidgetDistances, &QListWidget::itemClicked,
            this, &MainWindow::onDistanceItemClicked);
    connect(ui->lockButton, &QPushButton::clicked,
            this, &MainWindow::onLockButtonClicked);
    connect(ui->unlockButton, &QPushButton::clicked, 
            this, &MainWindow::onUnlockButtonClicked);
    connect(ui->nextButton, &QPushButton::clicked, 
            this, &MainWindow::onNextButtonClicked);
}

MainWindow::~MainWindow() {
    delete dbManager;
    delete ui;
}

void MainWindow::onCollegeChanged(const QString &college) {
    updateDistanceList(college);
}

void MainWindow::updateCollegeComboBox() {
    QString currentSelection = ui->comboBoxColleges->currentText(); // Save the current selection
    ui->comboBoxColleges->clear(); // Clear the combo box
    std::vector<QString> colleges = dbManager->getColleges(); // Get colleges from DB
    for (const QString &college : colleges) {
        ui->comboBoxColleges->addItem(college);
    }

    if (!colleges.empty()) {
        // Restore the previously selected college if available
        int index = ui->comboBoxColleges->findText(currentSelection);
        if (index != -1) {
            ui->comboBoxColleges->setCurrentIndex(index);
        } else {
            ui->comboBoxColleges->setCurrentIndex(0); // Set the first one if the previous selection is not found
        }
    }
}

void MainWindow::onListWidgetContextMenuRequested(const QPoint &pos) {
    QListWidgetItem* item = ui->listWidgetDistances->itemAt(pos);
    if (!item) {
        return;
    }

    // Create the context menu
    QMenu contextMenu(this);

    // Add action to toggle highlight
    QAction* toggleHighlightAction = new QAction("Toggle Highlight", this);
    connect(toggleHighlightAction, &QAction::triggered, this, [this, item]() {
        toggleItemHighlight(item);
    });
    contextMenu.addAction(toggleHighlightAction);

    // Show the menu at the right-clicked position
    contextMenu.exec(ui->listWidgetDistances->mapToGlobal(pos));
}

void MainWindow::toggleItemHighlight(QListWidgetItem* item) {
    QColor currentColor = item->background().color();
    if (currentColor == QColor(Qt::yellow)) {
        item->setBackground(Qt::white);  // Unhighlight the item
    } else {
        item->setBackground(Qt::yellow);  // Highlight the item
    }
}

void MainWindow::updateDistanceList(const QString &college) {
    // Retrieve all colleges from the database.
    std::vector<QString> colleges = dbManager->getColleges();

    // Reorder the list so that the selected college is the starting point.
    auto it = std::find(colleges.begin(), colleges.end(), college);
    if (it != colleges.end()) {
        std::iter_swap(colleges.begin(), it);
    }

    // Use TripPlanner to calculate the optimal trip.
    TripPlanner planner;
    planner.calculateTrip(colleges, dbManager);
    double totalDistance = planner.getTotalDistance();
    std::vector<QString> tripPath = planner.getPath();

    // Clear the list widget.
    ui->listWidgetDistances->clear();

    // Display each leg of the trip with the distance.
    double summedDistance = 0.0;
    if (!tripPath.empty()) {
        // First college: starting point.
        QListWidgetItem *item = new QListWidgetItem(tripPath[0] + " -(Start, 0 miles)");
        ui->listWidgetDistances->addItem(item);
    }
    
    // For each subsequent college, show the distance from the previous college.
    for (size_t i = 1; i < tripPath.size(); ++i) {
        QString prev = tripPath[i - 1];
        QString curr = tripPath[i];
        double legDistance = dbManager->getDistance(prev, curr);
        summedDistance += legDistance;
        QString itemText = QString("%1 - %2 miles").arg(curr).arg(legDistance);
        QListWidgetItem *item = new QListWidgetItem(itemText);
        ui->listWidgetDistances->addItem(item);
    }

    // Update the total distance label with the summed distance.
    ui->labelTotalDistance->setText(QString("Total Distance: %1 miles").arg(summedDistance));
}

void MainWindow::onDistanceItemClicked(QListWidgetItem *item) {
    if (listLocked) {
        // If the list is locked, check if the item is highlighted
        if (item->background() == QColor(Qt::yellow)) {
            // Extract the college name from the clicked item
            QString collegeName = item->text().section(" -", 0, 0).trimmed();
            // Show souvenirs for the clicked college
            updateSouvenirList(collegeName);
        } else {
            // If the item is not highlighted, do nothing or show a message
            QMessageBox::information(this, "Item Locked", "Only highlighted colleges are clickable.");
        }
    } else {
        // Normal behavior for unlocked items
        QString collegeName = item->text().section(" -", 0, 0).trimmed();
        updateSouvenirList(collegeName);
    }
}

void MainWindow::updateSouvenirList(const QString &college) {
    std::vector<std::pair<QString, double>> souvenirs = dbManager->getSouvenirs(college);
    
    qDebug() << "Souvenirs retrieved: " << souvenirs.size();
    ui->listWidgetSouvenirs->clear();
    
    for (const auto &souvenir : souvenirs) {
        QString displayText = souvenir.first + " - $" + QString::number(souvenir.second, 'f', 2);
        qDebug() << displayText; // Debugging output
        QListWidgetItem *item = new QListWidgetItem(displayText);
        ui->listWidgetSouvenirs->addItem(item);
    }
}


void MainWindow::onNextButtonClicked() {
    static int currentIndex = -1;
    QList<QListWidgetItem*> highlightedItems;

    for (int i = 0; i < ui->listWidgetDistances->count(); ++i) {
        QListWidgetItem* item = ui->listWidgetDistances->item(i);
        if (item->background() == QColor(Qt::yellow)) {
            highlightedItems.append(item);
        }
    }

    if (highlightedItems.isEmpty()) {
        QMessageBox::information(this, "No Highlights", "No highlighted colleges found.");
        return;
    }

    if (currentIndex == -1 || currentIndex >= highlightedItems.size() - 1) {
        currentIndex = 0;
    } else {
        currentIndex++;
    }

    ui->listWidgetDistances->setCurrentItem(highlightedItems[currentIndex]);
    
    // Extract college name correctly
    QString selectedCollege = highlightedItems[currentIndex]->text().section(" -", 0, 0).trimmed();

    qDebug() << "Fetching souvenirs for: " << selectedCollege;
    
    updateSouvenirList(selectedCollege);
}


void MainWindow::onLockButtonClicked() {
    qDebug() << "Lock button clicked";

    // Step 1: Identify the highlighted items in the current list
    QList<QListWidgetItem*> highlightedItems;
    std::vector<QString> highlightedCollegeNames;

    // Traverse the list to gather the highlighted items and their data
    for (int i = 0; i < ui->listWidgetDistances->count(); ++i) {
        QListWidgetItem* item = ui->listWidgetDistances->item(i);
        if (item->background() == QColor(Qt::yellow)) {
            highlightedItems.append(item);
            highlightedCollegeNames.push_back(item->text());  // Store only the text of the highlighted colleges
        }
    }

    // Debug: Log the number of highlighted items
    qDebug() << "Highlighted items: " << highlightedItems.size();

    // Step 2: If there are no highlighted items, show a warning and exit
    if (highlightedItems.isEmpty()) {
        QMessageBox::information(this, "No Highlights", "No highlighted colleges to lock.");
        return;
    }

    // Step 3: Lock the list visually (but not interaction completely)
    listLocked = true;
    qDebug() << "List locked";

    // Step 4: Clear the list completely before adding the highlighted items
    ui->listWidgetDistances->clear();  // Clears all items from the list

    // Step 5: Iterate through the highlighted colleges and re-add them to the list
    for (const QString& collegeName : highlightedCollegeNames) {
        // Create a new item for each college
        QListWidgetItem* newItem = new QListWidgetItem(collegeName);  // Create a new item with name text
        newItem->setBackground(Qt::yellow);  // Ensure the highlighted background color is preserved
        ui->listWidgetDistances->addItem(newItem);  // Add the new item to the list
    }

    // Step 6: Inform the user that only the highlighted colleges are retained
    QMessageBox::information(this, "List Locked", "Only highlighted colleges are retained.");
}

void MainWindow::onUnlockButtonClicked() {
    // Step 1: Unlock the list
    listLocked = false;
    ui->listWidgetDistances->setEnabled(true);  // Enable the list for further interactions

    // Step 2: Refresh the full list of colleges
    updateDistanceList(ui->comboBoxColleges->currentText());  // Assuming comboBox tracks the current college

    // Step 3: Add back highlighted items after the full list has been restored
    // Iterate over the list to highlight the items again
    for (int i = 0; i < ui->listWidgetDistances->count(); ++i) {
        QListWidgetItem* item = ui->listWidgetDistances->item(i);
        // Check if this item was highlighted before locking
        if (item->background() == QColor(Qt::yellow)) {
            item->setBackground(Qt::yellow);  // Set the highlighted background again
        }
    }

    // Step 4: Optional message box to notify the user
    QMessageBox::information(this, "List Unlocked", "You can now select and modify items.");
}


void MainWindow::on_importButton_clicked() {
    // Construct the full path to newcampuses.csv in the executable's directory.
    QString appDir = QCoreApplication::applicationDirPath();
    QString csvFile = appDir + "/newcampuses.csv";
    
    if (dbManager->importNewCampuses(csvFile)) {
        ui->statusbar->showMessage("New campuses imported successfully.", 3000);
        // Refresh the college combo box to include any newly imported campuses (starts with first college in list selected).
        updateCollegeComboBox();
    } else {
        ui->statusbar->showMessage("Failed to import new campuses.", 3000);
    }
}