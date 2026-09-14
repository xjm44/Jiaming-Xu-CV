#ifndef OSGALGORITHM_H
#define OSGALGORITHM_H

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Polygon_mesh_processing/smooth_shape.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/angle_and_area_smoothing.h>
#include <CGAL/Polygon_mesh_processing/detect_features.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Polygon_mesh_processing/refine.h>
#include <CGAL/Polygon_mesh_processing/fair.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/poisson_surface_reconstruction.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Scale_space_surface_reconstruction_3.h>
#include <CGAL/Scale_space_reconstruction_3/Jet_smoother.h>
#include <CGAL/Scale_space_reconstruction_3/Advancing_front_mesher.h>
#include <CGAL/draw_surface_mesh.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <fstream>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <boost/iterator/function_output_iterator.hpp>
#include <pcl/filters/crop_hull.h>

#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <QString>
#include <pcl/filters/uniform_sampling.h>
#include <QMessageBox>
#include <pcl/visualization/cloud_viewer.h>  
#include <pcl/filters/radius_outlier_removal.h> 
#include <pcl/common/common.h>  
#include <iostream>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/voxel_grid.h>

#include "atlbase.h"
#include "atlstr.h"
#include <boost/lexical_cast.hpp>

#include <pcl/features/normal_3d.h>
#include <vector>
#include <ctime>
#include <pcl/io/obj_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/surface/mls.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/convolution_3d.h>
#include <pcl/filters/median_filter.h>

#include <pcl/search/kdtree.h>

#include <pcl/point_cloud.h>
#include <pcl/common/time.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/surface/poisson.h>  
#include <pcl/geometry/polygon_mesh.h> //MESH
#include <pcl/surface/gp3.h> 
#include <boost/thread/thread.hpp>
#include <stdio.h>
#include <string.h>
#include <string>
#include <pcl/surface/marching_cubes_hoppe.h>

#include <pcl/conversions.h>

#include <pcl/conversions.h>

#include <utility>  // std::tuple
#include <set>
#include <cstdlib>
#include <Eigen/dense>
#include <cmath>
#include <ctime>
#include <Eigen/StdVector>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
#include "pcl/surface/convex_hull.h"


#include <direct.h>
#include <cstdio>

//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef K::Point_3 Point3;
//typedef CGAL::Surface_mesh<K::Point_3>                       C_Mesh;


using namespace std;
using namespace pcl;

using namespace Eigen;


// CGAL using declarations
/*
using KK = CGAL::Simple_cartesian<float>;

using Point = KK::Point_3;
using Vector = KK::Vector_3;
using Color = std::array<unsigned char, 3>;

using PNC = std::tuple<Point, Vector, Color>;
*/




// PCL using declarations
//using Cloud = pcl::PointCloud<pcl::PointXYZRGBNormal>;

typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;

typedef CGAL::Surface_mesh<K::Point_3>                        C_Mesh;
typedef K::Point_3 Point;				
using Vector = K::Vector_3;
typedef K::Vector_3 Vector_3;
typedef K::Sphere_3 Sphere_3;

typedef CGAL::Point_set_3<Point> Point_set;	
typedef CGAL::Point_set_3<Point, Vector_3> PointNormal_set;

typedef K::FT FT;


using Color = std::array<unsigned char, 3>;


//using xyzrgbnormal = std::tuple<Point, Vector, Color>; 
using xyzrgb = std::tuple<Point, Color>;

typedef C_Mesh::Vertex_index vertex_descriptor;
typedef C_Mesh::Face_index face_descriptor;
typedef C_Mesh::Halfedge_index halfedge_descriptor;
typedef C_Mesh::Edge_index edge_descriptor;

typedef boost::graph_traits<C_Mesh>::vertex_descriptor        vertex_descriptor;
typedef boost::graph_traits<C_Mesh>::halfedge_descriptor      halfedge_descriptor;
typedef boost::graph_traits<C_Mesh>::face_descriptor          face_descriptor;
typedef boost::graph_traits<C_Mesh>::edge_descriptor          edge_descriptor;

typedef CGAL::Polyhedron_3<K>                               Polyhedron;
typedef Polyhedron::Vertex_handle                           Vertex_handle;
typedef Polyhedron::Halfedge_handle                           Halfedge_handle;
typedef Polyhedron::Facet_handle                              Facet_handle;

namespace PMP = CGAL::Polygon_mesh_processing;
namespace SMS = CGAL::Surface_mesh_simplification;


class osgAlgorithm
{

public:

    void prin(const char* strOutputString, ...);
    int f1();

    int wenjian(string path);

    //int remove(const char * filename);
    //char *savePath = "/home";
    //remove(savePath);

    osgAlgorithm();
    ~osgAlgorithm();

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr downsampling(int x, QString filepath); // down-sampling the point cloud. x=0, .ply  x=1, .pcd
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr read(int x, QString filepath); // just read
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr uniform_downsampling(pcl::PointCloud<pcl::PointXYZRGB>::Ptr could_zhen, float x = 0.03);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr voxel_downsampling(pcl::PointCloud<pcl::PointXYZRGB>::Ptr could_zhen, float x = 0.05);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr statistical_outlierremoval(pcl::PointCloud<pcl::PointXYZRGB>::Ptr could_zhen, int n = 10, float e = 3);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr r_outlierremoval(pcl::PointCloud<pcl::PointXYZRGB>::Ptr could_zhen, int n = 5, float r = 0.05);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr mls(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen,float a = 0.1, int b =2 , float c=0.005); // a=searchradius b=polynomialorder c=sqrgaussparam
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr mls_upsampling(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, float a = 0.03, float b = 0.03, float c = 0.02);//mls a=searchradius b=upsamplingradius c=stepsize
    pcl::PointCloud<pcl::Normal>::Ptr CalculateNormal(pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_zhen);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr merge(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_1, pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_2);

    //int osgAlgorithm::plyview(std::string filepath);

    //pcl mesh
    pcl::PolygonMesh mesh_3(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, std::string path, double r = 0.1, int a = 3);
    pcl::PolygonMesh mesh_possion(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, std::string filepath);
    pcl::PolygonMesh mesh_cube(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, std::string filepath);
    int plyview(std::string filepath, pcl::PolygonMesh& pcl_mesh);



    std::vector<int> getindex(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, PointCloud<PointXYZRGB>::Ptr keypoints);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr del(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, vector<int> index);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::del2(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, vector<int> index);


    vector<int> regiongrowing(PointCloud<PointXYZRGB>::Ptr cloud, float x, float y, float z, float a = 6, float b = 0.4);
    PointCloud<PointXYZRGB>::Ptr manualpolygontool(PointCloud<PointXYZRGB>::Ptr cloud, PointCloud<PointXYZRGB>::Ptr ployin, int k = 0);
 


    //CGAL
    //CGAL::IO::read_PLY(path, cgal_cloud);//cgal_cloud
    //CGAL::copy_face_graph(mesh, poly);


    Point_set pcl2cgal(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_zhen);
    CGAL::Surface_mesh<K::Point_3> load_off(std::string path);
    CGAL::Surface_mesh<K::Point_3> load_off0(std::string path);
    int mesh_PCL_to_CGAL(pcl::PolygonMesh PCL_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh);


    int off2obj(string input, string output = "A");


    //CGAL mesh
    //PMP::IO::read_polygon_mesh(path, cgal_mesh);
    int mesh(Point_set p, int a, std::string path);


    Polyhedron hole_fill(CGAL::Surface_mesh<K::Point_3> mesh);
    CGAL::Surface_mesh<K::Point_3> smallhole_fill(CGAL::Surface_mesh<K::Point_3> mesh, double r = 0.1, int n = 10);
    CGAL::Surface_mesh<K::Point_3> mesh_3jiaosmooth(CGAL::Surface_mesh<K::Point_3> mesh, int a = 10);

    CGAL::Surface_mesh<K::Point_3> mesh_shapesmooth(C_Mesh mesh, int a = 5, double b = 0.3);

    CGAL::Surface_mesh<K::Point_3> mesh_simp(CGAL::Surface_mesh<K::Point_3> mesh, double a = 0.1);
    CGAL::Surface_mesh<K::Point_3> remeshing(CGAL::Surface_mesh<K::Point_3> mesh, double a = 1, int b = 5);
    Polyhedron mesh_refine_fair(CGAL::Surface_mesh<K::Point_3> mesh);


    
private:
    

};

#endif // OSGALGORITHM_H