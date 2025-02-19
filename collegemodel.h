#ifndef COLLEGEMODEL_H
#define COLLEGEMODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QAbstractListModel>

struct Souvenir {
    QString name;
    double price;
};

struct College {
    QString name;
    QMap<QString, double> distances;  // Distance to other colleges
    QVector<Souvenir> souvenirs;
};

class CollegeModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DistanceRole
    };

    explicit CollegeModel(QObject *parent = nullptr);

    void addCollege(const College &college);
    Q_INVOKABLE void setReferenceCollege(const QString &collegeName);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantList getSouvenirs(const QString &collegeName) const;

private:
    QVector<College> m_colleges;
    QString m_referenceCollege;
};

#endif // COLLEGEMODEL_H
