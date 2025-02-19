#include "collegemodel.h"

CollegeModel::CollegeModel(QObject *parent)
    : QAbstractListModel(parent), m_referenceCollege("") {}

void CollegeModel::addCollege(const College &college) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_colleges.append(college);
    endInsertRows();
}

void CollegeModel::setReferenceCollege(const QString &collegeName) {
    m_referenceCollege = collegeName;
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
