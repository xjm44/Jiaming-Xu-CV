#include "../include/osgCloudData.h"

osgCloudData::osgCloudData()
{}

osgCloudData::~osgCloudData()
{}

void osgCloudData::setProperty(QString name) // cloud name
{
	cloudName = name;
}

void osgCloudData::setProperty(float xx, float yy, float zz) // point coordinate
{
	x = xx;
	y = yy;
	z = zz;

}

void osgCloudData::setProperty(QString name, float value) // dist. btw 2 points
{
	if (name == "distance") {
		distance = value;
	}
	else if (name == "points") {
		cloudPoints = static_cast<int>(value);
	}
}

float osgCloudData::getPropertyFloat(QString name)
{
	if (name == "distance") {
		return distance;
	}
	else if (name == "points") {
		return cloudPoints;
	}
	return 0.0;
}
QString osgCloudData::getPropertyQString(QString name)
{
	if (name == "name") {
		return cloudName;
	}else if (name == "coordinate") {
		QString temp = "X :   " + QString::number(x) + "\n" + "Y :   " + QString::number(y) + "\n" + "Z :   " + QString::number(z);
		return temp;
	}

	return " ";
}

float osgCloudData::getDistance()
{
	float res = sqrt(pow((x1 - x2),2) + pow((y1 - y2), 2) + pow((z1 - z2), 2));
	return res;
}

void osgCloudData::setFirstPoint(float x, float y, float z)
{
	x1 = x;
	y1 = y;
	z1 = z;
}

void osgCloudData::setSecondPoint(float x, float y, float z)
{
	x2 = x;
	y2 = y;
	z2 = z;
}

