#ifndef OSGVIEW_H
#define OSGVIEW_H

#include "windowLogin.h"
#include "osgAlgorithm.h"
#include "osgColor.h"
#include "osgCloudData.h"
#include "colormap.h"
#include "osgHudCallback.h"
#include "findGeode.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/Export>
#include <osgGA/GUIEventAdapter>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>
#include <osg/ShapeDrawable>
#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/TexGen>
#include <osg/MatrixTransform>
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osg/Node>
#include <osg/Fog>
#include <osg/Depth>
#include <osg/Texture2D>
#include <osgUtil/Simplifier>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/SmoothingVisitor>
#include <osg/Point>
#include <osg/PositionAttitudeTransform>
#include <QMessageBox>
#include <string>
#include <array>
#include <QTreeWidgetItem>
#include <pcl/filters/uniform_sampling.h>
#include <corecrt_io.h>
#include <osg/LineWidth>
#include <osg/AutoTransform>
#include <osgUtil/DelaunayTriangulator>
#include "osgDB/WriteFile"
#include "osgDB/ReadFile"
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Dragger>
#include <osgManipulator/ScaleAxisDragger>
#include <osgManipulator/Selection>
#include <pcl/octree/octree_pointcloud_voxelcentroid.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3                             Point1;
typedef CGAL::Surface_mesh<Point1>             C_Mesh1;
typedef CGAL::Polyhedron_3<K>                  Polyhedron;
namespace PMP = CGAL::Polygon_mesh_processing;
using namespace std;
using namespace pcl;


class osgview 
{

public:
    osgview();
    ~osgview();

    // file to point cloud (process) ================================
    
    // read point cloud file
    void readPcd();
    void readPly();
    
    int receivePointCloud(QString filename); // check dragged file
    void changeFilepath(QString f); // make the file path readable
    QString file;  // point cloud file path
    bool checkCloudFile = false;
    bool canShowName = false; // add cloud path(name) in the UI
    int count; // no. of clouds
    int mesh_count; // no. of mesh obj
    int extract_plane_count; // no. of extracted plane
    int treat_plane_count; // no. of treated cloud
    int merged_cloud; // no. of merged cloud
    //================================================================


    // UI ===========================================================

    QTreeWidgetItem* cloudItem; // the clouds on the tree widget

    osgViewer::Viewer* pViewer;

    std::list<osgCloudData*> cloudsData; // stores clouds' individual properties
    int current_clouddata_pos = 0; // parent cloud, eg. Cloud 1

    osg::Vec4 setColor(int r, int g, int b); // return current background color

    bool point_check = false;
    bool photo_check = false;

    void remove_points();    // clicking -> red points
    int no_of_redpoints = 0;


    osgColor* osgcolor; // for changing color of the clouds
    void show_color(int x, int parentindex, int cloudindex, ColorMap cmap, QString check); //0: rgb, 1: intensity , 2: z ,3: y , 4: x , 5: none

    float plane_x, plane_y, plane_z; // plane extraction
    bool plane_check = false;
    std::vector<int> all_plane={};

    //================================================================


    // store as geode =================================================
    osg::ref_ptr<osg::Group> gp_all = new osg::Group; // store everything                     =======SO FAR===== 0:gp 1:gp_temp 3:coordinate 4: center point ====
    osg::ref_ptr<osg::Switch> gp = new osg::Switch; // store lists of clouds
    osg::ref_ptr<osg::Group> gp_temp = new osg::Group; // to store lists of temporary (trimmed/extracted) clouds
    osg::ref_ptr<osg::Geode> geode1; // each cloud
    void getGeode(); // return geode to display point cloud
    //================================================================

    osg::ref_ptr<osg::Geode> createGeode(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, const char* name); // for function(s) that need(s) a node to be created

    osg::Camera* osgview::makeCoordinate(osgViewer::Viewer* viewer, int width, int height); // coordinate
    osg::Camera* osgview::makeCenterX(); // center x


    osgAlgorithm* osgAlgo; // for downsampling ... (other functions that need to be done in future)

    osg::ref_ptr<osg::LOD> applyLOD(osg::ref_ptr<osg::Geode> node); // LoD     ... TO BE DONE

    // store file(s) inserted
    std::vector<QString> files;
    // store cloud(s) inserted
    std::vector<pcl::PointCloud<pcl::PointXYZRGB>::Ptr> all_clouds;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr temp_cloud; // to copy other cloud temporarily


    // for trim=============
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;
    int trim_count = 0; // must more than 2 pts to be cutted
    //======================

    int screenshotIndex; // for screenshot index
    bool ss_taken = false;

    osg::ref_ptr<osg::Switch> current_lod_gp;

private:

    QString filepath; // point cloud file path

    // to differentiate files
    bool ifpcd = false;
    bool ifply = false;
    //=======================

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud; // use pcl to get the points
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered; // down-sampling the point cloud
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen;
    pcl::PointCloud<pcl::PointNormal> cloud_normal;//normal
    pcl::PointCloud<pcl::PointXYZI> cloudIntensity; // use pcl to get the points

    pcl::PolygonMesh pcl_mesh;//pcl mesh
    pcl::PolygonMesh::Ptr pcl_mesh_zhen;//pcl mesh
    CGAL::Surface_mesh<K::Point_3> cgal_mesh;//cgal mesh
    //std::vector<xyzrgb> cgal_rgbcloud;
    //std::vector<Point> cgal_cloud;
    Point_set cgal_cloud;//cgal
    Polyhedron cgal_poly;// cgal polygon

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud1;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud2; 
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud3;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud4;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud5;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud0;


    
};


#endif // OSGVIEW_H