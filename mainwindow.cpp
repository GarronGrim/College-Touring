#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), listLocked(false)
{
    ui->setupUi(this);

    // Initialize DatabaseManager (assumes campus.db is in the working directory).
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
    populateColleges();

    // Connect signals:
    connect(ui->comboBoxColleges, &QComboBox::currentTextChanged,
            this, &MainWindow::onCollegeChanged);
    connect(ui->listWidgetDistances, &QListWidget::itemClicked,
            this, &MainWindow::onDistanceItemClicked);
    connect(ui->lockButton, &QPushButton::clicked,
            this, &MainWindow::onLockButtonClicked);
}

// Destructor
MainWindow::~MainWindow() {
    delete dbManager;
    delete ui;
}

void MainWindow::populateColleges() {
    std::vector<QString> colleges = dbManager->getColleges();
    ui->comboBoxColleges->clear();
    for (const auto &college : colleges) {
        ui->comboBoxColleges->addItem(college);
    }
    if (!colleges.empty()) {
        // Display distances for the first college by default.
        updateDistanceList(colleges.front());
    }
}

void MainWindow::onCollegeChanged(const QString &college) {
    updateDistanceList(college);
}

std::vector<std::pair<QString, double>> MainWindow::calculateShortestTrip(DatabaseManager* dbManager, const QString& startCollege) {
    // Get direct distances from the start college to all other colleges.
    std::vector<std::pair<QString, double>> distances = dbManager->getDistances(startCollege);

    distances.insert(distances.begin(), {startCollege, 0.0});

    return distances;
}

void MainWindow::updateDistanceList(const QString &college) {
    ui->listWidgetDistances->clear();
    
    // Retrieve distances (for each college, the distance from the reference college).
    std::vector<std::pair<QString, double>> distances = calculateShortestTrip(dbManager, college);
    
    // Sort the list in ascending order based on the distance.
    std::sort(distances.begin(), distances.end(), [](const auto &a, const auto &b) {
        return a.second < b.second;
    });
    
    // Sum all the distances.
    double totalDistance = 0.0;
    for (const auto &pair : distances) {
        totalDistance += pair.second;
        QString text = QString("%1 - %2 miles").arg(pair.first).arg(pair.second);
        QListWidgetItem *item = new QListWidgetItem(text);
        ui->listWidgetDistances->addItem(item);
    }
    
    // Update the total distance label.
    ui->labelTotalDistance->setText(QString("Total Distance: %1 miles").arg(totalDistance));
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