#pragma once
#include <QObject>
#include <QMap>

struct coordinateNode {
    QString strName;
    QString strProj4;
};

// for coordinate json
class coordinate :public QObject
{
    Q_OBJECT
private:
    coordinate();
    ~coordinate() = default;

    coordinate(const coordinate &coor) = delete;
    const coordinate& operator = (const coordinate &coor) = delete;

public:

    static coordinate& GetInstance();

    QStringList GetCountryList();
    QList<std::pair<QString, QString> > GetCoordinateList(QString country);

private:
    void ReadJson();

private:
    QMap<QString, QList<coordinateNode>> m_mapCoordinate;
};

