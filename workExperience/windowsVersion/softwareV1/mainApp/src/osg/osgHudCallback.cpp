#include "../include/osgHudCallback.h"


HudCallback::HudCallback(osgViewer::Viewer* viewer) :m_viewer(viewer)
{
}
void HudCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{

	osg::MatrixTransform* pTM = dynamic_cast<osg::MatrixTransform*>(node);
	if (pTM)
	{
		osg::Camera* camera = m_viewer->getCamera();
		osg::Vec3 translate = pTM->getMatrix().getTrans();
		osg::Vec3 scale = pTM->getMatrix().getScale();
		osg::Matrix mv = camera->getViewMatrix();

		osg::Matrix inv_mv = camera->getInverseViewMatrix(); 
		osg::Quat inv_q = inv_mv.getRotate();
		osg::Quat q = mv.getRotate();


		osg::Quat dq(osg::DegreesToRadians(90.0f), osg::Vec3(1.0f, 0.0f, 0.0f));
		// pTM->setMatrix(osg::Matrix::scale(scale)* osg::Matrix::rotate( dq * q ) * osg::Matrix::translate(translate));
		static osg::Matrix mm = osg::Matrix::rotate(dq);
		mv.setTrans(translate);
		pTM->setMatrix(osg::Matrix::scale(scale) * mv /** mm*/);
	}
}
