#include "mainwindow.h"
#include "DatabaseManager.h"
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
    connect(ui->listWidgetSouvenirs, &QListWidget::itemClicked,
            this, &MainWindow::onSouvenirItemClicked);
    connect(ui->lockButton, &QPushButton::clicked,
            this, &MainWindow::onLockButtonClicked);
    connect(ui->maintenanceButton, &QPushButton::clicked,
            this, &MainWindow::onMaintenanceButtonClicked);
    connect(ui->listWidgetSouvenirs, &QListWidget::itemClicked,
            this, &MainWindow::onSouvenirItemClicked);
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
        updateSouvenirsList(colleges.front());
    }
}

void MainWindow::onCollegeChanged(const QString &college) {
    updateDistanceList(college);
    updateSouvenirsList(college);
}

std::vector<std::pair<QString, double>> MainWindow::calculateShortestTrip(DatabaseManager* dbManager, const QString& startCollege) {
    // Get direct distances from the start college to all other colleges.
    vector<pair<QString, double>> distances = dbManager->getDistances(startCollege);

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

void MainWindow::updateSouvenirsList(const QString &college) {
    ui->listWidgetSouvenirs->clear();

    // Retrieve souvenirs (for the selected college).
    std::vector<std::pair<QString, double>> souvenirs = dbManager->getSouvenirs(college);

    // Iterate through the souvenirs and display them in the list.
    for (const auto &souvenir : souvenirs) {
        QString text = QString("%1 - $%2").arg(souvenir.first).arg(souvenir.second, 0, 'f', 2);  // Formatting price to 2 decimal places
        QListWidgetItem *item = new QListWidgetItem(text);
        ui->listWidgetSouvenirs->addItem(item);
    }
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

void MainWindow::onSouvenirItemClicked(QListWidgetItem *item) {
    if (listLocked)
        return;

    vector<pair<QString, double>> souvenirs;

    bool acceptable;
    QString quantity = QInputDialog::getText(this, "Enter Quantity : No More Than 3 Of Each Item", "Enter How Many You'd Like To Purchase", 
                                                QLineEdit::Normal, "", &acceptable);

    if (acceptable && !quantity.isEmpty()) {
        bool isNumber;
        int quantityValue = quantity.toInt(&isNumber);

        if (isNumber && quantityValue >= 1 && quantityValue <= 3) {
            QMessageBox::information(this, "Added To Cart", "You Are All Set!");
        } else {
            QMessageBox::warning(this, "Invalid Amount", "Try Again! Please enter a number between 1 and 3.");
        }
    }
}

QStringList MainWindow::parseCSVLine(const QString &line) {
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
                inQuotes = !inQuotes;  // Toggle the inQuotes flag
            }
        } else if (c == ',' && !inQuotes) {
            // Field separator found outside of quotes.
            result.append(current);
            current.clear();
        } else {
            // Append the character to the current field.
            current.append(c);
        }
    }

    // Append the last field (in case the line doesn't end with a comma)
    result.append(current);

    return result;
}


void MainWindow::onMaintenanceButtonClicked() {
    bool correct;
    QString password = QInputDialog::getText(this, "Enter Password", "Enter 4-Digit Maintenance Password:",
                                             QLineEdit::Password, "", &correct);

    if (correct && !password.isEmpty()) {
        if (password == "1234") {
            QMessageBox::information(this, "Access Granted", "You now have access to modify the database.");

            // Handle college distances CSV
            QString distanceFilePath = QFileDialog::getOpenFileName(this, "Open Distance File", "", "CSV Files (*.csv)");
            if (!distanceFilePath.isEmpty()) {
                QFile distanceFile(distanceFilePath);
                if (distanceFile.open(QIODevice::ReadOnly)) {
                    QTextStream in(&distanceFile);
                    in.readLine();  // Skip header line
                    while (!in.atEnd()) {
                        QString line = in.readLine();
                        QStringList columns = parseCSVLine(line);

                        if (columns.size() == 3) {
                            QString startCollege = columns[0].trimmed();
                            QString endCollege = columns[1].trimmed();
                            double distance = columns[2].toDouble();

                            dbManager->addCollegeDistanceMaintenance(startCollege, endCollege, distance);
                        }
                    }
                    distanceFile.close();
                    QMessageBox::information(this, "Distance File Imported", "The college distances have been added or updated.");
                } else {
                    QMessageBox::warning(this, "File Error", "Failed to open the distance file.");
                }
            } else {
                QMessageBox::warning(this, "File Error", "No distance file selected.");
            }

            // Handle souvenirs CSV
            QString souvenirFilePath = QFileDialog::getOpenFileName(this, "Open Souvenir File", "", "CSV Files (*.csv)");
            if (!souvenirFilePath.isEmpty()) {
                QFile souvenirFile(souvenirFilePath);
                if (souvenirFile.open(QIODevice::ReadOnly)) {
                    QTextStream in(&souvenirFile);
                    in.readLine();  // Skip header line
                    while (!in.atEnd()) {
                        QString line = in.readLine();
                        QStringList columns = parseCSVLine(line);

                        if (columns.size() == 3) {
                            QString collegeName = columns[0].trimmed();
                            QString souvenirName = columns[1].trimmed();
                            double price = columns[2].toDouble();

                            dbManager->addSouvenirMaintenance(collegeName, souvenirName, price);
                        }
                    }
                    souvenirFile.close();
                    QMessageBox::information(this, "Souvenir File Imported", "The souvenirs have been added or updated.");
                } else {
                    QMessageBox::warning(this, "File Error", "Failed to open the souvenir file.");
                }
            } else {
                QMessageBox::warning(this, "File Error", "No souvenir file selected.");
            }

            // Ask the user if they want to manipulate existing data
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Modify Data", "Would you like to modify existing college or souvenir data?",
                                          QMessageBox::Yes | QMessageBox::No);
            
            if (reply == QMessageBox::Yes) {
                openEditDataDialog();
            }

        } else {
            QMessageBox::warning(this, "Access Denied", "Incorrect password!");
        }
    }
}


void MainWindow::openEditDataDialog() {
    bool ok;
    QStringList options = { "Edit College Names", "Edit Distances", "Edit Souvenir Names & Prices", "Delete Entries" };
    
    QString choice = QInputDialog::getItem(this, "Edit Data", "Select what you want to modify:", options, 0, false, &ok);

    if (ok && !choice.isEmpty()) {
        if (choice == "Edit College Names") {
            editCollegeNames();
        } else if (choice == "Edit Distances") {
            editDistances();
        } else if (choice == "Edit Souvenir Names & Prices") {
            editSouvenirs();
        } else if (choice == "Delete Entries") {
            deleteEntries();
        }
    }
}

void MainWindow::editCollegeNames() {
    bool ok;
    QString oldName = QInputDialog::getText(this, "Edit College Name", "Enter existing college name:", QLineEdit::Normal, "", &ok);

    if (ok && !oldName.isEmpty()) {
        QString newName = QInputDialog::getText(this, "Edit College Name", "Enter new college name:", QLineEdit::Normal, "", &ok);

        if (ok && !newName.isEmpty()) {
            dbManager->updateCollegeName(oldName, newName);
            QMessageBox::information(this, "Updated", "College name updated successfully!");
        }
    }
}

void MainWindow::editDistances() {
    bool ok;
    QString startCollege = QInputDialog::getText(this, "Edit Distance", "Enter start college:", QLineEdit::Normal, "", &ok);
    QString endCollege = QInputDialog::getText(this, "Edit Distance", "Enter end college:", QLineEdit::Normal, "", &ok);

    if (ok && !startCollege.isEmpty() && !endCollege.isEmpty()) {
        double newDistance = QInputDialog::getDouble(this, "Edit Distance", "Enter new distance:", 0, 0, 10000, 2, &ok);

        if (ok) {
            dbManager->updateDistance(startCollege, endCollege, newDistance);
            QMessageBox::information(this, "Updated", "Distance updated successfully!");
        }
    }
}


void MainWindow::editSouvenirs() {
    bool ok;
    QString college = QInputDialog::getText(this, "Edit Souvenir", "Enter college name:", QLineEdit::Normal, "", &ok);
    QString souvenir = QInputDialog::getText(this, "Edit Souvenir", "Enter souvenir name:", QLineEdit::Normal, "", &ok);

    if (ok && !college.isEmpty() && !souvenir.isEmpty()) {
        double newPrice = QInputDialog::getDouble(this, "Edit Souvenir", "Enter new price:", 0, 0, 10000, 2, &ok);

        if (ok) {
            dbManager->updateSouvenir(college, souvenir, newPrice);
            QMessageBox::information(this, "Updated", "Souvenir price updated successfully!");
        }
    }
}


void MainWindow::deleteEntries() {
    bool ok;
    QStringList options = { "Delete College", "Delete Distance", "Delete Souvenir" };
    
    QString choice = QInputDialog::getItem(this, "Delete Data", "Select what you want to delete:", options, 0, false, &ok);

    if (ok && !choice.isEmpty()) {
        if (choice == "Delete College") {
            QString college = QInputDialog::getText(this, "Delete College", "Enter college name:", QLineEdit::Normal, "", &ok);
            if (ok) dbManager->deleteCollege(college);
        } else if (choice == "Delete Distance") {
            QString startCollege = QInputDialog::getText(this, "Delete Distance", "Enter start college:", QLineEdit::Normal, "", &ok);
            QString endCollege = QInputDialog::getText(this, "Delete Distance", "Enter end college:", QLineEdit::Normal, "", &ok);
            if (ok) dbManager->deleteDistance(startCollege, endCollege);
        } else if (choice == "Delete Souvenir") {
            QString college = QInputDialog::getText(this, "Delete Souvenir", "Enter college name:", QLineEdit::Normal, "", &ok);
            QString souvenir = QInputDialog::getText(this, "Delete Souvenir", "Enter souvenir name:", QLineEdit::Normal, "", &ok);
            if (ok) dbManager->deleteSouvenir(college, souvenir);
        }
    }
}