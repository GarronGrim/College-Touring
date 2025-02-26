#include "collegemodel.h"
#include <iostream>
using namespace std;

CollegeModel::CollegeModel(QObject *parent)
    : QAbstractListModel(parent), m_referenceCollege("") {}

void CollegeModel::addCollege(const College &college) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_colleges.append(college);
    endInsertRows();
}

void CollegeModel::setReferenceCollege(const QString &collegeName) {
    m_referenceCollege = collegeName;
    calculateShortestTrip(collegeName);
    emit dataChanged(index(0), index(rowCount() - 1));
}

int CollegeModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_colleges.size();
}

QVariant CollegeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_colleges.size())
        return QVariant();

    const College &college = m_colleges.at(index.row());
    switch (role) {
    case NameRole:
        return college.name;
    case DistanceRole:
        return college.distances.value(m_referenceCollege, 0.0);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CollegeModel::roleNames() const {
    return {
        { NameRole, "name" },
        { DistanceRole, "distance" }
    };
}

QVariantList CollegeModel::getSouvenirs(const QString &collegeName) const {
    for (const auto &college : m_colleges) {
        if (college.name == collegeName) {
            QVariantList list;
            for (const auto &souvenir : college.souvenirs) {
                QVariantMap map;
                map["name"] = souvenir.name;
                map["price"] = souvenir.price;
                list.append(map);
            }
            return list;
        }
    }
    return {};
}

void CollegeModel::calculateShortestTrip(const QString &startCollege) {
    // First, find the reference college
    College *referenceCollege = nullptr;
    for (auto &college : m_colleges) {
        if (college.name == startCollege) {
            referenceCollege = &college;
            break;
        }
    }

    if (!referenceCollege) {
        qWarning() << "Reference college not found!";
        return;
    }

    // Retrieve distances from the reference college to other colleges
    QVector<QPair<QString, double>> collegeDistances;

    for (const auto &college : m_colleges) {
        if (college.name != startCollege && referenceCollege->distances.contains(college.name)) {
            double distance = referenceCollege->distances[college.name];
            collegeDistances.append(qMakePair(college.name, distance));
        }
    }
    


    // Sort colleges based on their distance to the reference college
    std::sort(collegeDistances.begin(), collegeDistances.end(), [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
        return a.second < b.second;  // Sort by distance
    });

    qWarning() << "distances " << collegeDistances;
    
    // Prepare the sorted trip list for the view
    m_shortestTrip.clear();
    for (const auto &pair : collegeDistances) {
        m_shortestTrip.append(pair.first + " - " + QString::number(pair.second) + " km");
    }

    beginResetModel();
    endResetModel();
    emit shortestTripChanged();  // Notify the view that the data has changed
}