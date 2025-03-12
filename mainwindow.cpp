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
    // listWidgetSelectedColleges = findChild<QListWidget*>("listWidgetSelectedColleges");

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
    connect(ui->nextButton, &QPushButton::clicked, 
            this, &MainWindow::onNextButtonClicked);
    connect(ui->listWidgetSouvenirs, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onSouvenirDoubleClicked);
    connect(ui->maintenanceButton, &QPushButton::clicked,
            this, &MainWindow::onMaintenanceButtonClicked);
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
    if (!item)
        return;

    // Prevent toggling the highlight for the starting college
    if (item->data(Qt::UserRole).toString() == "starting") {
        return;
    }

    QMenu contextMenu(this);
    QAction* toggleHighlightAction = new QAction("Select College", this);
    connect(toggleHighlightAction, &QAction::triggered, this, [this, item]() {
        toggleItemHighlight(item);
    });
    contextMenu.addAction(toggleHighlightAction);
    contextMenu.exec(ui->listWidgetDistances->mapToGlobal(pos));
}

void MainWindow::toggleItemHighlight(QListWidgetItem* item) {
    QColor currentColor = item->background().color();
    if (currentColor == QColor(Qt::blue)) {
        item->setBackground(Qt::white);  // Unhighlight the item
    } else {
        item->setBackground(Qt::blue);  // Highlight the item
    }
}

void MainWindow::updateDistanceList(const QString &selectedCollege) {
    ui->listWidgetDistances->clear();

    // Add the starting college at the top.
    QListWidgetItem *referenceItem = new QListWidgetItem(selectedCollege + " - (Start)");
    referenceItem->setData(Qt::UserRole, "reference");
    referenceItem->setFlags(referenceItem->flags() & ~Qt::ItemIsSelectable);
    ui->listWidgetDistances->addItem(referenceItem);

    // Retrieve all colleges from the database.
    std::vector<QString> allColleges = dbManager->getColleges();
    for (const QString &college : allColleges) {
        if (college == selectedCollege)
            continue;  
        double distance = dbManager->getDistance(selectedCollege, college);
        QString displayText = QString("%1 - %2 miles").arg(college).arg(distance);
        QListWidgetItem *item = new QListWidgetItem(displayText);
        ui->listWidgetDistances->addItem(item);
    }
}


void MainWindow::onDistanceItemClicked(QListWidgetItem *item) {
    if (listLocked) {
        // If the list is locked, check if the item is highlighted
        if (item->background() == QColor(Qt::blue)) {
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
        if (item->background() == QColor(Qt::blue))
            highlightedItems.append(item);
    }

    if (highlightedItems.isEmpty()) {
        QMessageBox::information(this, "No Highlights", "No highlighted colleges found.");
        return;
    }

    // If at the end, finalize current college, update display, then reset trip.
    if (currentIndex >= highlightedItems.size() - 1) {
        QString currentCollege = highlightedItems[currentIndex]->text().section(" -", 0, 0).trimmed();
        if (!visitedColleges.contains(currentCollege))
            visitedColleges.append(currentCollege);
        updatePurchasedSouvenirsDisplay();
        currentIndex = -1;
        onUnlockButtonClicked();
        return;
    } else {
        if (currentIndex >= 0) {
            QString currentCollege = highlightedItems[currentIndex]->text().section(" -", 0, 0).trimmed();
            if (!visitedColleges.contains(currentCollege))
                visitedColleges.append(currentCollege);
        }
        currentIndex++;
    }

    ui->listWidgetDistances->setCurrentItem(highlightedItems[currentIndex]);
    QString selectedCollege = highlightedItems[currentIndex]->text().section(" -", 0, 0).trimmed();
    updateSouvenirList(selectedCollege);
    updatePurchasedSouvenirsDisplay();
}


void MainWindow::onLockButtonClicked() {
    // Clear souvenir display.
    ui->listWidgetPurchasedSouvenirs->clear();
    visitedColleges.clear();
    purchases.clear();

    // Get the reference college from the dropdown.
    QString startingCollege = ui->comboBoxColleges->currentText();

    // Gather the highlighted colleges (skip the reference item if it exists).
    std::vector<QString> selectedColleges;
    selectedColleges.push_back(startingCollege);  // Ensure starting college is first

    // Iterate over all items (after the reference) and add highlighted items.
    for (int i = 0; i < ui->listWidgetDistances->count(); i++) {
        QListWidgetItem* item = ui->listWidgetDistances->item(i);
        // Skip the reference college if it’s marked.
        if (item->data(Qt::UserRole).toString() == "reference")
            continue;
        if (item->background() == QColor(Qt::blue)) {
            // When planning the trip, use only the college name (strip any extra info).
            QString collegeName = item->text().section(" -", 0, 0).trimmed();
            selectedColleges.push_back(collegeName);
        }
    }

    if (selectedColleges.size() < 2) {
        QMessageBox::information(this, "Insufficient Selection",
                                 "Please highlight at least one other college before planning a trip.");
        return;
    }

    // Calculate the trip based on the selected (highlighted) colleges.
    TripPlanner planner;
    planner.calculateTrip(selectedColleges, dbManager);
    double totalDistance = planner.getTotalDistance();
    std::vector<QString> tripPath = planner.getPath();

    // Now update the list to show only the planned trip order.
    ui->listWidgetDistances->clear();

    double summedDistance = 0.0;
    if (!tripPath.empty()) {
        // Display the starting college as the first item.
        QListWidgetItem *startItem = new QListWidgetItem(tripPath[0] + " - (Start, 0 miles)");
        // Mark it as reference by storing a custom role.
        startItem->setData(Qt::UserRole, "reference");
        startItem->setBackground(Qt::blue);
        ui->listWidgetDistances->addItem(startItem);
    }
    
    // For each subsequent college, calculate the leg distance and display it.
    for (size_t i = 1; i < tripPath.size(); i++) {
        QString prev = tripPath[i - 1];
        QString curr = tripPath[i];
        double legDistance = dbManager->getDistance(prev, curr);
        summedDistance += legDistance;
        QString itemText = QString("%1 - %2 miles").arg(curr).arg(legDistance);
        QListWidgetItem *item = new QListWidgetItem(itemText);
        item->setBackground(Qt::blue);
        ui->listWidgetDistances->addItem(item);
    }

    ui->labelTotalDistance->setText(QString("Total Distance: %1 miles").arg(summedDistance));

    QMessageBox::information(this, "Trip Planned", "Trip planned successfully.");

    // Update the souvenirs for the starting college.
    if (ui->listWidgetDistances->count() > 0) {
        QListWidgetItem* firstItem = ui->listWidgetDistances->item(0);
        ui->listWidgetDistances->setCurrentItem(firstItem);
        QString collegeName = firstItem->text().section(" -", 0, 0).trimmed();
        updateSouvenirList(collegeName);
    }
}


void MainWindow::onUnlockButtonClicked() {
    listLocked = false;
    ui->listWidgetDistances->setEnabled(true);
    updateDistanceList(ui->comboBoxColleges->currentText());
    ui->labelTotalDistance->setText("Total Distance: 0 miles");
    
    visitedColleges.clear();
    purchases.clear();

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

void MainWindow::onSouvenirDoubleClicked(QListWidgetItem *item) {
    // Parse the item text "SouvenirName - $Price"
    QString text = item->text();
    QString name = text.section(" - $", 0, 0).trimmed();
    double price = text.section(" - $", 1, 1).toDouble();

    // Prompt for a quantity using QInputDialog (default 1, minimum 1)
    bool ok = false;
    int quantity = QInputDialog::getInt(this,
                                        "Buy Souvenir",
                                        "Enter quantity for " + name + ":",
                                        1,     // default value
                                        1,     // minimum value
                                        100,   // maximum value
                                        1,     // step
                                        &ok);
    if (!ok) {
        // User canceled the input dialog.
        return;
    }

    QString college = ui->listWidgetDistances->currentItem()->text();
    college = college.section(" -", 0, 0).trimmed();

    // Create a purchase record with quantity.
    PurchasedSouvenir ps;
    ps.college = college;
    ps.souvenirName = name;
    ps.price = price;
    ps.quantity = quantity;
    purchases.push_back(ps);

    updatePurchasedSouvenirsDisplay();
}

void MainWindow::updatePurchasedSouvenirsDisplay() {
    ui->listWidgetPurchasedSouvenirs->clear();
    double grandTotal = 0.0;

    // For each visited college (tracked in visitedColleges)
    for (const QString &college : visitedColleges) {
        double collegeTotal = 0.0;
        // Iterate over purchases for this college.
        for (const PurchasedSouvenir &ps : purchases) {
            if (ps.college == college) {
                double totalCost = ps.price * ps.quantity;
                QString line = QString("%1: %2 x %3 = $%4")
                                   .arg(college)
                                   .arg(ps.souvenirName)
                                   .arg(ps.quantity)
                                   .arg(totalCost, 0, 'f', 2);
                ui->listWidgetPurchasedSouvenirs->addItem(line);
                collegeTotal += totalCost;
            }
        }
        ui->listWidgetPurchasedSouvenirs->addItem(
            QString("Total for %1: $%2").arg(college).arg(collegeTotal, 0, 'f', 2));
        grandTotal += collegeTotal;
    }
    ui->listWidgetPurchasedSouvenirs->addItem(
        QString("Grand Total: $%1").arg(grandTotal, 0, 'f', 2));
}

void MainWindow::onMaintenanceButtonClicked() {
    bool ok;
    // Prompt for the 4-digit maintenance password (using a password echo mode)
    QString password = QInputDialog::getText(this,
                                             "Maintenance",
                                             "Enter 4-Digit Maintenance Password:",
                                             QLineEdit::Password,
                                             "",
                                             &ok);
    if (!ok) return;  // User canceled

    // Check the password (change "1234" to your desired password)
    if (password != "1234") {
        QMessageBox::warning(this, "Invalid Password", "The maintenance password is incorrect.");
        return;
    }

    // Ask the user if they want to modify existing data
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Maintenance",
                                                              "Would you like to modify existing college or souvenir data?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    // Present the options in a combo box, now with an extra option to drop tables.
    QStringList options;
    options << "Add more colleges"
            << "Edit Souvenir price"
            << "Add Souvenir"
            << "Delete Souvenir"
            << "Drop Tables";
    QString selection = QInputDialog::getItem(this,
                                              "Select Modification",
                                              "Select what you want to modify:",
                                              options,
                                              0,
                                              false,
                                              &ok);
    if (!ok)
        return;

    // Execute the corresponding functionality based on user selection.
    if (selection == "Add more colleges") {
        // Call the same code as on_importButton_clicked.
        on_importButton_clicked();
    }
    else if (selection == "Edit Souvenir price") {
        // Prompt for the college name.
        QString college = QInputDialog::getText(this,
                                                "Edit Souvenir Price",
                                                "Enter college name:");
        if (college.isEmpty()) return;
        
        QString souvenir = QInputDialog::getText(this,
                                                 "Edit Souvenir Price",
                                                 "Enter souvenir name:");
        if (souvenir.isEmpty()) return;
        
        double newPrice = QInputDialog::getDouble(this,
                                                  "Edit Souvenir Price",
                                                  "Enter new price:",
                                                  0, 0, 10000, 2,
                                                  &ok);
        if (!ok) return;
        
        if (dbManager->updateSouvenirPrice(souvenir, newPrice))
            QMessageBox::information(this, "Success", "Souvenir price updated successfully.");
        else
            QMessageBox::warning(this, "Failure", "Failed to update souvenir price.");
    }
    else if (selection == "Add Souvenir") {
        QString college = QInputDialog::getText(this,
                                                "Add Souvenir",
                                                "Enter college name:");
        if (college.isEmpty()) return;
        
        QString souvenir = QInputDialog::getText(this,
                                                 "Add Souvenir",
                                                 "Enter souvenir name:");
        if (souvenir.isEmpty()) return;
        
        double price = QInputDialog::getDouble(this,
                                               "Add Souvenir",
                                               "Enter price:",
                                               0, 0, 10000, 2,
                                               &ok);
        if (!ok) return;
        
        if (dbManager->addSouvenir(college, souvenir, price))
            QMessageBox::information(this, "Success", "Souvenir added successfully.");
        else
            QMessageBox::warning(this, "Failure", "Failed to add souvenir.");
    }
    else if (selection == "Delete Souvenir") {
        QString college = QInputDialog::getText(this,
                                                "Delete Souvenir",
                                                "Enter college name:");
        if (college.isEmpty()) return;
        
        QString souvenir = QInputDialog::getText(this,
                                                 "Delete Souvenir",
                                                 "Enter souvenir name:");
        if (souvenir.isEmpty()) return;
        
        if (dbManager->removeSouvenir(souvenir))
            QMessageBox::information(this, "Success", "Souvenir deleted successfully.");
        else
            QMessageBox::warning(this, "Failure", "Failed to delete souvenir.");
    }
    else if (selection == "Drop Tables") {
        QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                        "Confirm Drop",
                                        "Are you sure you want to drop all tables? This action cannot be undone.",
                                        QMessageBox::Yes | QMessageBox::No);
        if (confirm == QMessageBox::Yes) {
            dbManager->dropTables();
            QMessageBox::information(this, "Success", "Tables dropped successfully.");
        } else {
            QMessageBox::information(this, "Cancelled", "Table drop cancelled.");
        }
    }
}

