#ifndef COLLEGEMODEL_H
#define COLLEGEMODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QAbstractListModel>
#include <QMap>
#include <QVariantList>


struct Souvenir {
    QString name;
    double price;
};

struct College {
    QString name;
    QMap<QString, double> distances;  // Distance to other colleges
    QVector<Souvenir> souvenirs;
    College() = default;
};

class CollegeModel : public QAbstractListModel {
    Q_OBJECT
    // Change QStringList to QVariantList in the Q_PROPERTY to match the actual type
    Q_PROPERTY(QVariantList shortestTrip READ getShortestTrip NOTIFY shortestTripChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DistanceRole
    };

    explicit CollegeModel(QObject *parent = nullptr);

    void addCollege(const College &college);
    Q_INVOKABLE void setReferenceCollege(const QString &collegeName);
    Q_INVOKABLE void calculateShortestTrip(const QString &startCollege);

    // Change return type from QStringList to QVariantList
    QVariantList getShortestTrip() const {
        return m_shortestTrip;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantList getSouvenirs(const QString &collegeName) const;

signals:
    void shortestTripChanged();

private:
    QVector<College> m_colleges;
    QString m_referenceCollege;
    QVariantList m_shortestTrip;  // Stores the shortest trip data
};

#endif // COLLEGEMODEL_H
