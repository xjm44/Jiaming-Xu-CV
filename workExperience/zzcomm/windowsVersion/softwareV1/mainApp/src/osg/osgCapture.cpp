#include "../include/osgCapture.h"


CaptureCallback::CaptureCallback()
{

    std::cout << "=----0---=" << std::endl;
}

CaptureCallback::CaptureCallback(osg::ref_ptr<osg::Image> image)
{
    image_ = image;

    std::cout << "=-------=" << std::endl;
}

CaptureCallback::~CaptureCallback() {}

void CaptureCallback::operator()(const osg::Camera& camera) const
{
    //image
    image_->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

    image_->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);
}


 
osg::ref_ptr<osg::Image> CaptureCallback::getOsgImage()
{
    return this->image_;
}

void CaptureCallback::setW(int w) {
    width = w;
}
void CaptureCallback::setH(int h) {
    height = h;
}