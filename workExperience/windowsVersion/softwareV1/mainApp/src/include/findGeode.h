#pragma once

#include <iostream>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/NodeVisitor>
#include <osg/Node>
#include <osg/Group>
#include <osgSim/DOFTransform>
#include <osgUtil/Optimizer>
#include <osg/NodeVisitor>
#include <iostream>
#include <vector>
#include <codecvt>


class findNode :
	public osg::NodeVisitor
{
public:
	findNode();
	findNode(const std::string name) :
		osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) //Set traverse mode
	{
		resultNode = NULL;
		this->name = name;
	}

	virtual void apply(osg::Node& searchNode)
	{
		if (searchNode.getName() == name)
		{
			osg::Geode* dynamicTry = dynamic_cast<osg::Geode*>(&searchNode);

			if (dynamicTry)
			{
				resultNode = dynamicTry;
			}
		}
		traverse(searchNode);
	}

	osg::Geode* getNode()
	{
		return resultNode;
	}
private:
	osg::Geode* resultNode;
	std::string name;
};