#ifndef OSGCOLOR_H
#define OSGCOLOR_H

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

class osgColor
{

public:
    osgColor();
    ~osgColor();

    osg::ref_ptr<osg::Vec4Array> getColor(int x, bool rgb); // point cloud index, if rgb or none
    std::vector<float> getIntensity(int x); // point cloud index
    void osgColor::getMinMax(int x, pcl::PointXYZRGB& min_pt, pcl::PointXYZRGB& max_pt); // get max and min x/y/z values

    void storeColors(osg::ref_ptr<osg::Vec4Array> a); // store rgb only
    void storeNoColor(osg::ref_ptr<osg::Vec4Array> a); // store none (all white)
    void storeIntensity(std::vector<float> a); // store intensity values
    void osgColor::storeMinMax(pcl::PointXYZRGB min_pt, pcl::PointXYZRGB max_pt); // store max and min x/y/z values


    void remove_colors(int index); // when point cloud is deleted
    
    

private:
    std::list<osg::ref_ptr<osg::Vec4Array>> pointcloud_colors; // used to store point cloud color: RGB
    std::list<std::vector<float>> pointcloud_intensity; // used to store point cloud intensity values;
    std::list<osg::ref_ptr<osg::Vec4Array>> pointcloud_colors_none; // used to store point cloud color: none
    std::list<std::pair<pcl::PointXYZRGB, pcl::PointXYZRGB>> pointcloud_MinMax; // used to store point cloud x/y/z values

};

#endif // OSGCOLOR_H