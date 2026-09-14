#ifndef OSGCAPTURE_H
#define OSGCAPTURE_H

#include <osgViewer/Viewer>
#include <osgViewer/ViewerBase>
#include <osgViewer/GraphicsWindow>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Image>
#include <osg/BufferObject>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <iostream>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osg/RenderInfo>



class CaptureCallback :public osg::Camera::DrawCallback
{

public:
    CaptureCallback();
    CaptureCallback(osg::ref_ptr<osg::Image> image);
    ~CaptureCallback();

    virtual void operator()(const osg::Camera& camera) const;

    osg::ref_ptr<osg::Image> getOsgImage();
    int width;
    int height;
    void setW(int w);
    void setH(int h);

private:
    osg::ref_ptr<osg::Image> image_ = new osg::Image();

};


#endif // OSGCAPTURE_H