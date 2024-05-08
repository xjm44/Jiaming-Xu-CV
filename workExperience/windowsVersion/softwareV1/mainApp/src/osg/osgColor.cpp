#include "../include/osgColor.h"

osgColor::osgColor()
{}

osgColor::~osgColor()
{}

osg::ref_ptr<osg::Vec4Array> osgColor::getColor(int x, bool rgb)
{

	if (rgb) {

		auto colorPos = pointcloud_colors.begin();
		std::advance(colorPos, x);
		return *colorPos;
	}
	else {

		auto colorPos = pointcloud_colors_none.begin();
		std::advance(colorPos, x);
		return *colorPos;
	}

}

void osgColor::storeColors(osg::ref_ptr<osg::Vec4Array> a)
{
	pointcloud_colors.push_back(a);
}

void osgColor::storeNoColor(osg::ref_ptr<osg::Vec4Array> a)
{
	pointcloud_colors_none.push_back(a);
}


std::vector<float> osgColor::getIntensity(int x)
{
	auto intensityPos = pointcloud_intensity.begin();
	std::advance(intensityPos, x);
	return *intensityPos;

}

void osgColor::storeIntensity(std::vector<float> a)
{
	pointcloud_intensity.push_back(a);

}

void osgColor::storeMinMax(pcl::PointXYZRGB min_pt, pcl::PointXYZRGB max_pt)
{
	pointcloud_MinMax.push_back({ min_pt, max_pt });
}

void osgColor::getMinMax(int x, pcl::PointXYZRGB& min_pt, pcl::PointXYZRGB& max_pt)
{
	auto it = pointcloud_MinMax.begin();
	std::advance(it, x);
	min_pt = it->first;
	max_pt = it->second;
}

void osgColor::remove_colors(int index)
{
	auto colorPos = pointcloud_colors.begin();
	std::advance(colorPos, index);
	pointcloud_colors.erase(colorPos);

	auto colorIPos = pointcloud_intensity.begin();
	std::advance(colorIPos, index);
	pointcloud_intensity.erase(colorIPos);

	auto colorNPos = pointcloud_colors_none.begin();
	std::advance(colorNPos, index);
	pointcloud_colors_none.erase(colorNPos);

	auto colorMMPos = pointcloud_MinMax.begin();
	std::advance(colorMMPos, index);
	pointcloud_MinMax.erase(colorMMPos);
}
