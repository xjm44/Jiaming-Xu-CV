#ifndef OSGLOD_H
#define OSGLOD_H

#include "windowLogin.h"
#include "osgview.h"

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

#include <QMessageBox>
#include <string>
#include <array>
#include <QTreeWidgetItem>



class osgLodHandler :public osgGA::GUIEventHandler
{

public:
    osgLodHandler::osgLodHandler(osgview* osgView);
    ~osgLodHandler();
    

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void setCurrentLod(osg::ref_ptr<osg::Switch> gp);

private:
    osgview* osgView;
    void beforeLod();
    void afterLod();
    osg::ref_ptr<osg::Switch> lod_gp;
};

#endif // OSGLOD_H