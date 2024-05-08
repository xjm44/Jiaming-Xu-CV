#include "../include/osgScreenShot.h"



screenshotHandler::screenshotHandler(int h, int w, osgview* osgView, string filePath)
{
    ov = osgView;
    captureCallbackObj = new CaptureCallback();
    captureCallbackObj->setW(w);
    captureCallbackObj->setH(h);
    this->setCaptureCallback(captureCallbackObj);
    this->filePath = filePath;

}
screenshotHandler::~screenshotHandler()
{
}
CaptureCallback* screenshotHandler::getCaptureCallback() {
    return captureCallbackObj;
}

bool screenshotHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

    if (viewer == NULL)
    {
        return false;
    }


    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::DOUBLECLICK:
    {
        int button = ea.getButton();
        if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {

            std::cout << filePath + "\n";

            string f;

            char filename[128]; 
            sprintf(filename, "ScreenShot_%1d.jpg", ov->screenshotIndex);
            //std::cout << filename<< std::endl;
            ov->screenshotIndex++;

            f = filePath + "/" + filename;
            osgDB::writeImageFile(*(captureCallbackObj->getOsgImage()), f);
            ov->ss_taken = true;

           


        }

    }

    default:
        return false;
    }
}

void screenshotHandler::setCaptureCallback(CaptureCallback* captureCallbackParam)
{
    this->captureCallbackObj = captureCallbackParam;
}