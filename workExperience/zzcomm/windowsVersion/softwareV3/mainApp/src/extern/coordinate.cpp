#include "../include/include_process/coordinate.h"

#include <iostream>
#include <utility>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QTextStream>

coordinate& coordinate::GetInstance()
{
    static coordinate instance;
    return instance;
}

coordinate::coordinate()
{
    ReadJson();
}

QStringList coordinate::GetCountryList()
{
    QList<QString> allKeys = m_mapCoordinate.keys();
    return allKeys;
}

QList<std::pair<QString, QString> > coordinate::GetCoordinateList(QString country)
{
    QList<coordinateNode> nodeList = m_mapCoordinate.value(country);
    QList<std::pair<QString, QString> > coordinateList;
    for (auto node : nodeList)
    {
        std::pair<QString, QString> pairData;
        pairData.first = node.strName;
        pairData.second = node.strProj4;

        coordinateList.append(pairData);
    }
    
    return coordinateList;
}

void coordinate::ReadJson()
{
    QString jsonFile = "..\\Lib\\zzcomm\\coordinate.json";
    QFile file(jsonFile);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        std::cout << "Read Json Failed..." << std::endl;
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString strFile = stream.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(strFile.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError
        && doc.isNull())
    {
        std::cout << "Json Wrong Format..." << std::endl;
        return;
    }

    QJsonObject rootObj = doc.object();

    QJsonValue jsonValue = rootObj.value("country");
    if (jsonValue.type() != QJsonValue::Array)
    {
        std::cout << "Json isn't Array..." << std::endl;
        return;
    }

    QJsonArray countryArray = jsonValue.toArray();
    for (int i = 0; i < countryArray.count(); ++i)
    {
        QJsonValue countryValue = countryArray.at(i);

        QJsonObject childObj = countryValue.toObject();
        QString strName = childObj.value("name").toString();

        QList<coordinateNode> tempList;

        QJsonArray coordinateArray = childObj.value("coordinate").toArray();
        for (auto obj : coordinateArray)
        {
            QString geodeticCRS = obj.toObject().value("GeodeticCRS").toString();
            QString proj4 = obj.toObject().value("Proj4").toString();

            coordinateNode node;
            node.strName = geodeticCRS;
            node.strProj4 = proj4;

            tempList.append(node);

        }

        m_mapCoordinate.insert(strName, tempList);
    }
}
