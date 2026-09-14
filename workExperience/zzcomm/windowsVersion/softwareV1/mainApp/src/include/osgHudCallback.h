#ifndef OSGHUDCALLBACK_H
#define OSGHUDCALLBACK_H
#include <Windows.h>
#include <iostream>
#include <osg/Node>
#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/Referenced>



class HudCallback :public osg::NodeCallback
{
public:
	HudCallback(osgViewer::Viewer* viewer);
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
private:
	osg::ref_ptr<osgViewer::Viewer> m_viewer;
};

#endif // OSGHUDCALLBACK_H