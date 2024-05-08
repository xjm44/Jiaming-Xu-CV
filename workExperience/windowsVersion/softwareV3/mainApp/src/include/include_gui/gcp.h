#pragma once

#include "atlbase.h"
#include "atlstr.h"

#include <Eigen/StdVector>
#include <Eigen/Core>
#include <Eigen/dense>

#include <pcl/console/parse.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/sample_consensus/sac_model_circle.h>
#include <pcl/sample_consensus/sac_model_circle3d.h>
#include <pcl/visualization/cloud_viewer.h>  
#include <pcl/filters/radius_outlier_removal.h>  
#include <pcl/common/common.h>  
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/surface/mls.h>
#include <pcl/io/obj_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/convolution_3d.h>
#include <pcl/filters/median_filter.h>
#include <pcl/surface/impl/mls.hpp>
#include <pcl/common/time.h>
#include <pcl/surface/poisson.h>  
#include <pcl/geometry/polygon_mesh.h> //MESH
#include <pcl/surface/gp3.h> 
#include <pcl/features/normal_3d_omp.h> 
#include <pcl/search/kdtree.h>
#include <pcl/features/normal_3d.h>
#include <pcl/conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/surface/marching_cubes_hoppe.h>
#include <pcl/surface/marching_cubes_rbf.h>
#include <pcl/io/io.h>
#include <pcl/filters/crop_hull.h>
#include <pcl/surface/concave_hull.h>
#include <pcl/surface/convex_hull.h>
#include <pcl/common/impl/io.hpp>
#include <pcl/search/search.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/filters/uniform_sampling.h>
#include <pcl/io/file_io.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>

#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

#include <boost/lexical_cast.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/thread/thread.hpp>

#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <time.h>
#include <algorithm>
#include <direct.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <utility>  // std::tuple
#include <set>
#include <stdio.h>
#include <iterator>
#include <map>
#include <cstdlib>
#include <chrono>

#include "Yuan.h"

#include <QWidget>
#include <QProcess>
#include <QList>


using namespace std::chrono_literals;
using namespace std;
using namespace pcl;
using namespace Eigen;

namespace pcl {
	struct PointXYZIT   
	{
		float x;
		float y;
		float z;
		float intensity;
		double time;
	};
}
POINT_CLOUD_REGISTER_POINT_STRUCT(
	pcl::PointXYZIT,    
	(float, x, x)
	(float, y, y)
	(float, z, z)
	(float, intensity, intensity)
	(double, time, time)
)



class widget_newProcess;


class gcp : public QWidget
{
	Q_OBJECT

public:
	gcp(widget_newProcess* parent);
	~gcp();
	//void init();

	//
	//pair< std::vector<Yuan>, std::vector<std::string> > get_gcp(std::string path_cloud, std::string path, int a = 1,  
	//	std::string path_cloudline = "N", float r1 = 0.08, float r2 = 0.2, int switch_key = 0, float i_min = 240, float  i_max = 255, 
	//	int zk = 0, float z_min = -1.3, float z_max = -0.8, int min_circle = 250, float d_max_1 = 5, int auto_key = 1, float d_min = 5,
	//	int min_break = 20, float zr = 0.02, int d_max = 100, int d = 10, float rd = 0.10, float r_index = 1,
	//	int gcpIndex=0);

	//int gcpIndex;

	//
	//Yuan add(float x, float y, float z, std::string path_cloud, std::string path, float r1 = 0.1, float r2 = 0.2,
	//	int name = 0, int diedai = 3, std::string path_cloudline = "N", int i_min = 240, int i_max = 255, int switch_key = 1, int d_max_1 = 5, int min_break = 40);


	//int gcp_save(int line_key, vector<Yuan> o, std::string path_txt);
	//vector<Yuan> gcp_load(std::string path_txt);


	//sor
	pcl::PointCloud<pcl::PointXYZI>::Ptr sor(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, int n, float e);

	//// merge ply files
	//void mergeFiles(QStringList,QString);

protected:
	void closeEvent(QCloseEvent* event) override;

protected:

	//pcl::visualization::PCLVisualizer::Ptr simpleVis(pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud);

	//pcl::PointCloud<pcl::PointXYZI>::Ptr vox_dsp(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float x);

	//pcl::PointCloud<pcl::PointXYZI>::Ptr uni_dsp(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float x);
	////MLS  
	//pcl::PointCloud<pcl::PointXYZI>::Ptr  mls(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float r, int n, float g);

	//pcl::PointCloud<pcl::PointXYZI>::Ptr  pth_filter(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float i_min = 0, int i_max = 255); 

	//pcl::PointCloud<pcl::PointXYZI>::Ptr  pth_filter_z(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float z_min = -2, int z_max = 0);

	//pcl::PointCloud<pcl::PointXYZI>::Ptr del(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, vector<int> index);

	//pcl::PointCloud<pcl::PointXYZI>::Ptr del2(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, vector<int> index);

	//bool find_neigbour_radius(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, float radius, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis);
	//bool find_neigbour_radius2(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, float radius, Indices& pt_idx_search, std::vector<float>& pt_sqr_dis);

	//bool find_k_nearest_neigbour(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, int k, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis);

	//bool find_neigbour_radius_t(pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree, pcl::PointXYZIT search_pt, float radius, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis);
	//bool find_k_nearest_neigbour_t(pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree, pcl::PointXYZIT search_pt, int k, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis);


	//pair<double, float> get_time0(float x0, float y0, float z0, pcl::PointCloud<PointXYZI>::Ptr cloud, pcl::PointCloud<PointXYZIT>::Ptr cloud_t);

	//pair<double, vector<float>> get_time_t(float x0, float y0, float z0, pcl::PointCloud<PointXYZIT>::Ptr cloud);

	//pair<double, float> get_time2(float x0, float y0, float z0, pcl::PointCloud<PointXYZIT>::Ptr pathCloud);

	//vector<double> chongfubadian(float x, float y, float z, pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline, float d_max = 5);

	//std::vector<std::string> split(const std::string& str, char delim);


private Q_SLOTS:


Q_SIGNALS:


private:
	//bool manualCheck;
	//float radMax;
	//float radMin;
	//int no_of_gcp;
	//int intensityMax;
	//int intensityMin;
	//int iteration;
	//int circlePts;
	//bool filterDist;
	//int filterDistance;
	//bool filterZCoor;
	//float filterZCoorMax;
	//float filterZCoorMin;


	//bool getManualCheck() { return manualCheck; }
	//float getRadMax() { return radMax; }
	//float getRadMin() { return radMin; }
	//int getNoOfGcp() { return no_of_gcp; }
	//int getIntensityMax() { return intensityMax; }
	//int getIntensityMin() { return intensityMin; }
	//int getIteration() { return iteration; }
	//int getCirclePts() { return circlePts; }
	//bool getFilterDist() { return filterDist; }
	//int getFilterDistance() { return filterDistance; }
	//bool getFilterZCoor() { return filterZCoor; }
	//float getFilterZCoorMax() { return filterZCoorMax; }
	//float getFilterZCoorMin() { return filterZCoorMin; }

private:
	widget_newProcess* parent_;

};
