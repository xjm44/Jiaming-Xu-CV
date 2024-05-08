#include "../include/osgPlaneExtraction.h"



bool planeExtraction::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);

	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
	{
		int button = ea.getButton();
		if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {

			osg::ref_ptr<osgUtil::PolytopeIntersector> picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, ea.getX() - 4.0, ea.getY() - 4.0, ea.getX() + 4.0, ea.getY() + 4.0);
			osgUtil::IntersectionVisitor iv(picker.get());
			viewer->getCamera()->accept(iv);

			if (picker->containsIntersections())
			{
				osg::Vec3f ptWorldIntersectPointFirst = picker->getFirstIntersection().intersectionPoints[0];

				this->x = ptWorldIntersectPointFirst.x();
				this->y = ptWorldIntersectPointFirst.y();
				this->z = ptWorldIntersectPointFirst.z();
				osgView->plane_x = ptWorldIntersectPointFirst.x();
				osgView->plane_y = ptWorldIntersectPointFirst.y();
				osgView->plane_z = ptWorldIntersectPointFirst.z();

				osgView->plane_check = true;
				
			}
			else {

			}

		}

	}
	default:
		return false;
	}
}

planeExtraction::planeExtraction(osgview* ov)
{
	osgView = ov;
}

planeExtraction::~planeExtraction()
{}

