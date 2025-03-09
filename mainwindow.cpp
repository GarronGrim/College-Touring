#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TripPlanner.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QListWidgetItem>
#include <algorithm>   
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), listLocked(false)
{
    ui->setupUi(this);

    // Initialize DatabaseManager (assumes campus.db is in the working directory (build)).
    dbManager = new DatabaseManager("campus.db");

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
    connect(ui->comboBoxColleges, &QComboBox::currentTextChanged,
            this, &MainWindow::onCollegeChanged);
    connect(ui->listWidgetDistances, &QListWidget::itemClicked,
            this, &MainWindow::onDistanceItemClicked);
    connect(ui->lockButton, &QPushButton::clicked,
            this, &MainWindow::onLockButtonClicked);
}

MainWindow::~MainWindow() {
    delete dbManager;
    delete ui;
}

void MainWindow::onCollegeChanged(const QString &college) {
    updateDistanceList(college);
}

void MainWindow::updateCollegeComboBox() {
    ui->comboBoxColleges->clear();
    std::vector<QString> colleges = dbManager->getColleges();
    for (const QString &college : colleges) {
        ui->comboBoxColleges->addItem(college);
    }
    if (!colleges.empty()) {
        updateDistanceList(colleges.front());
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
        QListWidgetItem *item = new QListWidgetItem(tripPath[0] + " (Start, 0 miles)");
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

    // Update the total distance label with the summed distance (should match totalDistance if computed similarly).
    ui->labelTotalDistance->setText(QString("Total Distance: %1 miles").arg(summedDistance));
}

void MainWindow::onDistanceItemClicked(QListWidgetItem *item) {
    if (listLocked)
        return;
    // Toggle the background color between yellow and white.
    QColor currentColor = item->background().color();
    if (currentColor == QColor(Qt::yellow))
        item->setBackground(Qt::white);
    else
        item->setBackground(Qt::yellow);
}

void MainWindow::onLockButtonClicked() {
    listLocked = true;
    ui->listWidgetDistances->setDisabled(true);
    QMessageBox::information(this, "List Locked",
                             "All items have been locked and cannot be clicked.");
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