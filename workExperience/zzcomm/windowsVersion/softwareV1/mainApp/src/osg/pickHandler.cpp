#include "../include/pickHandler.h"



bool pickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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
				osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(ptWorldIntersectPointFirst, 0.003);
				osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(sphere);
				sd->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));



				osgView->gp->addChild(sd.release());
				osgView->no_of_redpoints++;
				



				auto cloudPos = osgView->cloudsData.begin();             // TO BE DONE ============================= currently cloudsdata get the index of cloud parents, not sub-group
				std::advance(cloudPos, osgView->current_clouddata_pos); 
				(*cloudPos)->setProperty(ptWorldIntersectPointFirst.x(), ptWorldIntersectPointFirst.y(), ptWorldIntersectPointFirst.z());

				if (!(*cloudPos)->first_point) {

					(*cloudPos)->setFirstPoint(ptWorldIntersectPointFirst.x(), ptWorldIntersectPointFirst.y(), ptWorldIntersectPointFirst.z());

					(*cloudPos)->first_point = true;
				}
				else {
					(*cloudPos)->setSecondPoint(ptWorldIntersectPointFirst.x(), ptWorldIntersectPointFirst.y(), ptWorldIntersectPointFirst.z());
					(*cloudPos)->distance_res = (*cloudPos)->getDistance();
				}

				osgView->point_check = true;

				cout << "PC: ";
				cout << ptWorldIntersectPointFirst.x();
				cout << " ";
				cout << ptWorldIntersectPointFirst.y();
				cout << " ";
				cout << ptWorldIntersectPointFirst.z();
				cout << " \n ";
			}
			else {
				
			}

			
		}else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) {

			


		}

		
	}
	case osgGA::GUIEventAdapter::SCROLL: {

	}
	default:
		return false;
	}
}

pickHandler::pickHandler(osgview* ov)
{
	osgView = ov;
}

pickHandler::~pickHandler()
{}
