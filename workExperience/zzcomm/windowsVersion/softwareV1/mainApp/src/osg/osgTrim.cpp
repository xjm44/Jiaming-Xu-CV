#include "../include/osgTrim.h"



bool osgTrim::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);


	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
	{
		int button = ea.getButton();
		if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {

	
			osg::Vec3d eye, center1, up;
			viewer->getCamera()->getViewMatrixAsLookAt(eye,center1,up);
			osg::Vec3d eyeForward = center1 - eye;

			cout << "Eye: x:";
			cout << eyeForward.x();
			cout << "  y: ";
			cout << eyeForward.y();
			cout << "  z: ";
			cout << eyeForward.z();

			


			auto matViewMatrix = viewer->getCamera()->getViewMatrix();
			auto matProjectionMatrix = viewer->getCamera()->getProjectionMatrix();
			auto wndMatrix = viewer->getCamera()->getViewport()->computeWindowMatrix();
			osg::Matrix MVPW = matViewMatrix * matProjectionMatrix * wndMatrix;
			osg::Matrix inverseMVPW = osg::Matrix::inverse(MVPW);
			osg::Vec3 mouseWorld = osg::Vec3(ea.getX(), ea.getY(), 0) * inverseMVPW;

				
			cout << "Empty1: x:";
			cout << mouseWorld.x();
			cout << "  y: ";
			cout << mouseWorld.y();
			cout << "  z: ";
			cout << mouseWorld.z();
			//cout << " normal: ";
			//cout << mouseWorld.normalize();
			cout << " \n ";

	
			osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0, 0, 0), 0.06);
			osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(sphere);
			sd->setColor(osg::Vec4(0.0, 1.0, 0.0, 1.0));
			osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(mouseWorld));
			trans->addChild(sd);
			osgView->gp->addChild(trans.release());
				


			if (eyeForward.x() == 1 || eyeForward.x() == -1) {
				osgView->x.push_back(0);
				osgView->y.push_back(mouseWorld.y());
				osgView->z.push_back(mouseWorld.z());
				cout << "XXX \n ";

			}else if (eyeForward.y() == 1 || eyeForward.y() == -1) {
				osgView->x.push_back(mouseWorld.x());
				osgView->y.push_back(0);
				osgView->z.push_back(mouseWorld.z());
				cout << "YYY \n ";

			}else if (eyeForward.z() == 1 || eyeForward.z() == -1) {
				osgView->x.push_back(mouseWorld.x());
				osgView->y.push_back(mouseWorld.y());
				osgView->z.push_back(0);
				cout << "ZZZ \n ";
				
			}
			else {
				osgView->x.push_back(-999);
				osgView->y.push_back(-999);
				osgView->z.push_back(-999);
				cout << "wrong angle";
			}
			

			osgView->trim_count++;
			osgView->no_of_redpoints++;

		}



	}
	case osgGA::GUIEventAdapter::SCROLL: {
		

	}
	default:
		return false;
	}
}

osgTrim::osgTrim(osgview* ov)
{
	osgView = ov;
}

osgTrim::~osgTrim()
{}
