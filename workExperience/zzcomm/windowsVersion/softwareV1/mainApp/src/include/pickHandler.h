#ifndef HANDLER_H
#define HANDLER_H

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



class pickHandler :public osgGA::GUIEventHandler
{

public:
    pickHandler::pickHandler(osgview* ov);
    ~pickHandler();
    osgview* osgView;

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

};

#endif // HANDLER_H