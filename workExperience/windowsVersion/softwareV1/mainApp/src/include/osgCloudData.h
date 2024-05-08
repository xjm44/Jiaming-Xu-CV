#ifndef OSGCLOUDDATA_H
#define OSGCLOUDDATA_H

#include "windowLogin.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/Export>
#include <osgGA/GUIEventAdapter>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>
#include <osg/ShapeDrawable>
#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/TexGen>
#include <osg/MatrixTransform>
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osg/Node>
#include <osg/Fog>
#include <osg/Depth>
#include <osg/Texture2D>
#include <osgUtil/Simplifier>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/SmoothingVisitor>
#include <osg/Point>

#include <QMessageBox>
#include <string>
#include <array>
#include <QTreeWidgetItem>
#include <math.h>

class osgCloudData
{

public:
    osgCloudData();
    ~osgCloudData();

    void setProperty(QString name); /// cloud name
    void setProperty(float x,float y, float z); // cloud point's coordinate
    void setProperty(QString name, float value); // distance between 2 points OR cloud points

    float getPropertyFloat(QString name); // using 'name' to differentiate what property to get
    QString getPropertyQString(QString name); // using 'name' to differentiate what property to get

    float getDistance(); // dist btw 2 points
    void setFirstPoint(float x, float y, float z);
    void setSecondPoint(float x, float y, float z);
   
    int count = 0;    // no. of points clicked
    bool first_point = false;
    float distance_res=0.0;  // distance result

    

private:

    float x,y,z;
    float x1, y1, z1;
    float x2, y2, z2;
    QString cloudName;
    float distance, cloudPoints;
};

#endif // OSGCLOUDDATA_H