#ifndef OSGSCREENSHOT_H
#define OSGSCREENSHOT_H

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
#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>
#include <OpenThreads/ScopedLock>
#include "osgCapture.h"
#include "osgview.h"
#include "osgAlgorithm.h"


class screenshotHandler :public osgGA::GUIEventHandler
{
public:
    screenshotHandler(int h, int w, osgview* osgView, string filePath);
    ~screenshotHandler();


    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    void setCaptureCallback(CaptureCallback* captureCallbackParam);
    CaptureCallback* getCaptureCallback();

private:

    int _screenCaptureSequence;
    CaptureCallback* captureCallbackObj = nullptr;
    osgview* ov;
    string filePath;
    osgAlgorithm* osgAlgo;
};


#endif // OSGSCREENSHOT_H