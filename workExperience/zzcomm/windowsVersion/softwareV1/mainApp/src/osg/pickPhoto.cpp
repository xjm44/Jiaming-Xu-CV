#include "../include/pickPhoto.h"



bool pickPhoto::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);


	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
	{
		int button = ea.getButton();
		if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {

			osg::ref_ptr<osgUtil::PolytopeIntersector> picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, ea.getX() - 50.0, ea.getY() - 50.0, ea.getX() + 50.0, ea.getY() + 50.0);
			osgUtil::IntersectionVisitor iv(picker.get());
			viewer->getCamera()->accept(iv);

			//if (picker->containsIntersections())
			//{

			//	osgView->photo_check = true;
			//}


		}

	}
	case osgGA::GUIEventAdapter::MOVE:
	{
		cout << "move";

	}
	default:
		return false;
	}
}

pickPhoto::pickPhoto(osgview* ov)
{
	osgView = ov;
}

pickPhoto::~pickPhoto()
{}
