#include "../include/osgLod.h"



bool osgLodHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);




	switch (ea.getEventType())
	{

	case osgGA::GUIEventAdapter::NONE:
	{
		cout << "none \n";
		//afterLod();
		return false;

	}
	case osgGA::GUIEventAdapter::SCROLL:
	{
		//switch (ea.getScrollingMotion()) {
		//	case osgGA::GUIEventAdapter::SCROLL_UP:
		//	{
		//		cout << "scrolled up \n";
		//		beforeLod();
		//		break;
		//	}
		//	case osgGA::GUIEventAdapter::SCROLL_DOWN:
		//	{
		//		cout << "scrolled down\n";
		//		beforeLod();
		//		break;
		//	}

		//	default:
		//		return false;

		//}
		//beforeLod();
		cout << "scrolled \n";
		return false;

		
	}
	case osgGA::GUIEventAdapter::DRAG:
	{
		cout << "dragged \n";
		beforeLod();
		return false;

	}
	case osgGA::GUIEventAdapter::RELEASE:
	{
		cout << "released \n";
		afterLod();
		return false;

	}



	default:
		return false;
	}



}

osgLodHandler::osgLodHandler(osgview* osgView)
{
	this->osgView = osgView;
}

osgLodHandler::~osgLodHandler()
{}


void osgLodHandler::afterLod() {
	lod_gp ->setValue(0, true);
	lod_gp->setValue(1, false);

}

void osgLodHandler::beforeLod() {
	lod_gp->setValue(0, false);
	lod_gp->setValue(1, true);
}
void osgLodHandler::setCurrentLod(osg::ref_ptr<osg::Switch> gp) {
	lod_gp = gp;
}