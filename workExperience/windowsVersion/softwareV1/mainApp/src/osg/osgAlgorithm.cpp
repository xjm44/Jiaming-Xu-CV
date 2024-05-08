#include "../include/osgAlgorithm.h"


osgAlgorithm::osgAlgorithm() {}
osgAlgorithm::~osgAlgorithm(){}


void osgAlgorithm::prin(const char* strOutputString, ...)

{

    char strBuffer[4096] = { 0 };

    va_list vlArgs;

    va_start(vlArgs, strOutputString);

    _vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);

    //vsprintf(strBuffer,strOutputString,vlArgs);

    va_end(vlArgs);

    OutputDebugString(strBuffer);

}

void prin(const char* strOutputString, ...)

{

    char strBuffer[4096] = { 0 };

    va_list vlArgs; 

    va_start(vlArgs, strOutputString);

    _vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);

    //vsprintf(strBuffer,strOutputString,vlArgs);

    va_end(vlArgs);

    OutputDebugString(strBuffer);

}




int osgAlgorithm::wenjian(string path)
{
    //string path = "F:/PCD_data/x/yyy2";

    if (0 != _access(path.c_str(), 0))
    {

        _mkdir(path.c_str());

    }


    return 0;

}

pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::downsampling(int x, QString filepath) {

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>());
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_sampling(new pcl::PointCloud<pcl::PointXYZRGB>());
    /*int cnt = 0;
    for (auto& file : vec) {
        cnt++;
        fprintf(stderr, "\t%s\t%d\%\n", file.c_str(), int(float(cnt) / vec.size() * 100));
        pcl::PCDReader reader;
        pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>());
        reader.read(pcd_path1 + std::string(file.c_str()), *cloud);
        *cloud_all += *cloud;
    }*/
    if (x == 1) {
        pcl::PCDReader reader;
        reader.read<pcl::PointXYZRGB>(filepath.toStdString(), *cloud);
    }
    else {
        pcl::PLYReader reader;
        reader.read<pcl::PointXYZRGB>(filepath.toStdString(), *cloud);
    }


    pcl::UniformSampling<pcl::PointXYZRGB> sor;

    sor.setInputCloud(cloud);
    sor.setRadiusSearch(0.01f);
    sor.filter(*cloud_sampling);
    //pcl::PLYWriter writer;
    //writer.write<pcl::PointXYZRGB>("table_scene_lms400_inliers.ply", *cloud_sampling, false);

    return cloud_sampling;
}


pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::read(int x, QString filepath) {

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZRGB>());
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr read(new pcl::PointCloud<pcl::PointXYZRGB>());

    if (x == 1) {
        pcl::PCDReader reader;
        reader.read<pcl::PointXYZRGB>(filepath.toStdString(), *cloud0);
    }
    else {
        pcl::PLYReader reader;
        reader.read<pcl::PointXYZRGB>(filepath.toStdString(), *cloud0);
    }

    return cloud0;

}

//========================================================================================================================================


pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::voxel_downsampling(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, float x) {



    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZRGB>);


    pcl::VoxelGrid<pcl::PointXYZRGB> sor;
    sor.setInputCloud(cloud_zhen);          
    sor.setLeafSize(x, x, x); 
    sor.filter(*cloud_filtered);  

    return cloud_filtered;

}


pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::uniform_downsampling(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, float x) {


    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_sampling(new pcl::PointCloud<pcl::PointXYZRGB>());

    pcl::UniformSampling<pcl::PointXYZRGB> sor;


    sor.setInputCloud(cloud_zhen);
    sor.setRadiusSearch(x);
    sor.filter(*cloud_sampling);

    //pcl::PLYWriter writer;
    //writer.write<pcl::PointXYZRGB>("table_scene_lms400_inliers.ply", *cloud_sampling, false);


    return cloud_sampling;

}



//======================================================================================================================================



pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::statistical_outlierremoval(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, int n, float e) {



    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZRGB>);


    pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB> outrem;
    outrem.setInputCloud(cloud_zhen);
    outrem.setMeanK(n);
    outrem.setStddevMulThresh(e);

    outrem.filter(*cloud_out);

    return cloud_out;

}


pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::r_outlierremoval(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, int n, float r) {


    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_rfilter(new pcl::PointCloud<pcl::PointXYZRGB>);


    pcl::RadiusOutlierRemoval<pcl::PointXYZRGB> sor;
    sor.setInputCloud(cloud_zhen);
    sor.setMinNeighborsInRadius(n);
    sor.setRadiusSearch(r);
    sor.setNegative(false);
    sor.filter(*cloud_rfilter);

    return cloud_rfilter;


}


//===================================================================================================================================

//MLS  
pcl::PointCloud<pcl::PointXYZRGB>::Ptr  osgAlgorithm::mls(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, float a, int b, float c){



    pcl::PointCloud<pcl::PointXYZRGB>::Ptr mls_points{ new pcl::PointCloud<pcl::PointXYZRGB>() };

    pcl::MovingLeastSquares<pcl::PointXYZRGB, pcl::PointXYZRGB> MLS;
    pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZRGB>());
    MLS.setSearchMethod(tree);
    MLS.setSearchRadius(a);//0.1
    MLS.setComputeNormals(false);
    MLS.setInputCloud(cloud_zhen);
    MLS.setPolynomialOrder(b);
    MLS.setSqrGaussParam(c);
    MLS.process(*mls_points);

    return mls_points;
}


//=================================================================================================================================

pcl::PointCloud<pcl::PointXYZRGB>::Ptr  osgAlgorithm::mls_upsampling(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, float a, float b, float c) {

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr upsampling_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

    pcl::MovingLeastSquares<pcl::PointXYZRGB, pcl::PointXYZRGB> filter;
    filter.setInputCloud(cloud_zhen);

    pcl::search::KdTree<pcl::PointXYZRGB>::Ptr kdtree;
    filter.setSearchMethod(kdtree);

    filter.setSearchRadius(a);

    // Upsampling , DISTINCT_CLOUD, RANDOM_UNIFORM_DENSITY
    filter.setUpsamplingMethod(pcl::MovingLeastSquares<pcl::PointXYZRGB, pcl::PointXYZRGB>::SAMPLE_LOCAL_PLANE);

    filter.setUpsamplingRadius(b);

    filter.setUpsamplingStepSize(c);

    filter.process(*upsampling_cloud);

    return upsampling_cloud;


}


//merge

pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::merge(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_1, pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_2)
{
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_3(new pcl::PointCloud<pcl::PointXYZRGB>);
    (*cloud_3) = (*cloud_1) + (*cloud_2);
    return cloud_3;

}



//====================================================================================================================

pcl::PointCloud<pcl::Normal>::Ptr osgAlgorithm::CalculateNormal(pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_zhen)
{

    pcl::StopWatch time;
    std::cout << "begin compute normal.... " << std::endl;
    pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> normalEstimation;                  
    normalEstimation.setInputCloud(cloud_zhen);                                 
    pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZRGB>);          
    normalEstimation.setSearchMethod(tree);
    pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);   

    normalEstimation.setKSearch(10);                    
    //normalEstimation.setRadiusSearch(0.03);  
    normalEstimation.compute(*normals);          

    std::cout << "success compute normal time:  " << time.getTimeSeconds() << std::endl;
    return normals;
}




//===========================================================================================================================


pcl::PolygonMesh  osgAlgorithm::mesh_3(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, std::string filepath, double r, int a) {



    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_out_obj(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZRGB>);

    //DownSampledPoints(cloud_in, cloud_out_obj);
    // OutlierRemoval(cloud_out_obj, cloud_out_obj);
    // SmoothPointcloud(cloud_out_obj, cloud_out);
    cloud_out = cloud_zhen;

    // cloud_out->height = 1;
    // cloud_out->width = cloud_out->points.size();
    // cout << "point cloud size = " << cloud_out->points.size() << endl;
    // cloud_out->is_dense = false;
    // pcl::visualization::CloudViewer viewer("viewer");
    // viewer.showCloud(cloud_out);
    // while (!viewer.wasStopped())
    // {

    // }

    pcl::StopWatch time;
    pcl::PointCloud<pcl::Normal>::Ptr normals;
    normals = CalculateNormal(cloud_out);
    std::cout << "begin  mesh..." << std::endl;

    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    pcl::concatenateFields(*cloud_out, *normals, *cloud_with_normals);


    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    tree2->setInputCloud(cloud_with_normals);


    pcl::GreedyProjectionTriangulation<pcl::PointXYZRGBNormal> gp3;   
    pcl::PolygonMesh triangles; 

    gp3.setSearchRadius(r); 
    gp3.setMu(a); 
    gp3.setMaximumNearestNeighbors(100);    

    gp3.setMinimumAngle(M_PI / 18); 
    gp3.setMaximumAngle(2 * M_PI / 3);

    gp3.setMaximumSurfaceAngle(M_PI / 4); 
    gp3.setNormalConsistency(false); 

    gp3.setInputCloud(cloud_with_normals);    
    gp3.setSearchMethod(tree2);   
    gp3.reconstruct(triangles); 
    cloud_with_normals->width = cloud_with_normals->height = 0;
    std::cout << "success triangles, time(s) " << time.getTimeSeconds() << std::endl;



    pcl::io::savePLYFileBinary(filepath, triangles);

    //std::string path1;
    //path1 = path + "/mesh.ply";
    //pcl::io::savePLYFileBinary(path1, triangles);



    return triangles;

}



pcl::PolygonMesh  osgAlgorithm::mesh_possion(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, std::string filepath) {

    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointXYZRGBNormal>); 
    pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);


    //   pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZRGB>);
    //   tree->setInputCloud(cloud_zhen);
    //   n.setInputCloud(cloud_zhen);
    //   n.setSearchMethod(tree);
    //   n.setKSearch(20);
    //   n.compute(*normals); 
    normals = CalculateNormal(cloud_zhen);

    pcl::concatenateFields(*cloud_zhen, *normals, *cloud_with_normals);


    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    tree2->setInputCloud(cloud_with_normals);

    pcl::Poisson<pcl::PointXYZRGBNormal> pn;
    pn.setConfidence(false); 
    pn.setDegree(2); 
    pn.setDepth(8); 
    pn.setIsoDivide(8); 
    pn.setManifold(false);
    pn.setOutputPolygons(false);
    pn.setSamplesPerNode(3.0);
    pn.setScale(1.25); 
    pn.setSolverDivide(8); 
    //pn.setIndices();


    pn.setSearchMethod(tree2);
    pn.setInputCloud(cloud_with_normals);

    pcl::PolygonMesh mesh;

    pn.performReconstruction(mesh);


    pcl::io::savePLYFileBinary(filepath, mesh);


    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D viewer"));
    viewer->setBackgroundColor(0, 0, 0);
    viewer->addPolygonMesh(mesh, "my");
    viewer->addCoordinateSystem(50.0);
    viewer->initCameraParameters();
    while (!viewer->wasStopped()) {
        viewer->spinOnce(100);
        //    boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    }

    return mesh;
}


pcl::PolygonMesh  osgAlgorithm::mesh_cube(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, std::string filepath) {


    pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> n;
    pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
    //    pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZRGB>);
    //    tree->setInputCloud(cloud_zhen);
    //    n.setInputCloud(cloud_zhen);
    //    n.setSearchMethod(tree);
    //    n.setKSearch(20);
    n.compute(*normals); 
    normals = CalculateNormal(cloud_zhen);



    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    pcl::concatenateFields(*cloud_zhen, *normals, *cloud_with_normals);
    //* cloud_with_normals = cloud + normals



    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    tree2->setInputCloud(cloud_with_normals);


    pcl::MarchingCubes<pcl::PointXYZRGBNormal>* mc;
    mc = new pcl::MarchingCubesHoppe<pcl::PointXYZRGBNormal>();

    pcl::PolygonMesh mesh;

    //MarchingCubes
    mc->setIsoLevel(0.0f);
    mc->setGridResolution(50, 50, 50);
    mc->setPercentageExtendGrid(0.0f);


    mc->setInputCloud(cloud_with_normals);

 
    mc->reconstruct(mesh);
    //"F:/PCD_data/shengcheng/cube.ply"

    pcl::io::savePLYFileBinary(filepath, mesh);


    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D viewer"));
    viewer->setBackgroundColor(255, 255, 255);
    viewer->addPolygonMesh(mesh, "my");
    viewer->addCoordinateSystem(50.0);
    viewer->initCameraParameters();
    while (!viewer->wasStopped()) {
        viewer->spinOnce(100);
        //    boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    }


    return mesh;
}



//==================================================================

int osgAlgorithm::plyview(std::string filepath, pcl::PolygonMesh& pcl_mesh)
{

    //const std::string kPlyFilePath = filepath;

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_src(new pcl::PointCloud<pcl::PointXYZRGB>());
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_tmp(new pcl::PointCloud<pcl::PointXYZ>());


    if (-1 == pcl::io::loadPLYFile(filepath, *cloud_src)) {
        std::cout << "load ply file failed. please check it." << std::endl;
        return -1;
    }


    pcl::PolygonMesh poly_mesh;

    if (-1 == pcl::io::loadPLYFile(filepath, poly_mesh)) {
        std::cout << "load ply file failed. please check it." << std::endl;
        return -1;
    }

    pcl_mesh = poly_mesh;

    pcl::visualization::PCLVisualizer viewer("viewer");


    int v1;
    int v2;
    //x_min, y_min, x_max, y_max, viewport
    viewer.createViewPort(0.0, 0.0, 0.5, 1.0, v1);
    viewer.createViewPort(0.5, 0.0, 1.0, 1.0, v2);

    viewer.addText("point cloud", 10, 10, 20, 1, 0, 0, "viewport_v1", v1);
    viewer.addText("mesh", 10, 10, 20, 0, 1, 0, "viewport_v2", v2);


    viewer.addCoordinateSystem(0.5);    


    viewer.setBackgroundColor(0.2, 0.2, 0.2);     // r,g,b 


    viewer.addPointCloud(cloud_src, "cloud_src", v1);


    viewer.addPolygonMesh(poly_mesh, "mesh_src", v2);


    // viewer.setRepresentationToWireframeForAllActors();


    while (!viewer.wasStopped()) {
        viewer.spinOnce(100);
        //boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    }

    return 0;

}

//std::vector<PNC> pcl2cgal(const Cloud& cloud)
Point_set osgAlgorithm::pcl2cgal(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_zhen)
//std::vector<xyzrgb> pcl_2_cgal(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_zhen)

{
    Point_set points;


    for (size_t i = 0; i < cloud_zhen->size(); i++) {

        points.insert(Point(cloud_zhen->points[i].x, cloud_zhen->points[i].y, cloud_zhen->points[i].z));
    }


    //CGAL::IO::write_PLY("F:/PCD_data/shengcheng/c/cccgal.ply", points);
    // CGAL::IO::write_XYZ("write_XYZ.xyz", points);

    return points;
}


// .off mesh

CGAL::Surface_mesh<K::Point_3> osgAlgorithm::load_off(std::string path) {

    C_Mesh mesh;

    PMP::IO::read_polygon_mesh(path, mesh);


    return mesh;

}


CGAL::Surface_mesh<K::Point_3> osgAlgorithm::load_off0(std::string path) {

    std::ifstream file(path, ios::in);
    CGAL::Surface_mesh<K::Point_3> mesh;
    file >> mesh;
    file.close();
    return mesh;

}



//int convert_mesh_from_PCL_to_CGAL(pcl::PolygonMesh::Ptr PCL_mesh, CGAL_Mesh& CGAL_mesh)
//int f5(pcl::PolygonMesh PCL_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh);
//int osgAlgorithm::f5(pcl::PolygonMesh pcl_mesh,  C_Mesh& CGAL_mesh)

int osgAlgorithm::mesh_PCL_to_CGAL(pcl::PolygonMesh pcl_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh)
//int osgAlgorithm::f5(pcl::PolygonMesh pcl_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh)
{


    // pcl::PolygonMesh::Ptr PCL_mesh_zhen;
    //PCL_mesh_zhen = (pcl::PolygonMesh::Ptr)&pcl_mesh;
    //pcl::PolygonMesh* ptr_pcl_mesh = &pcl_mesh;

    pcl::PolygonMesh::Ptr PCL_mesh_zhen(new pcl::PolygonMesh);
    *PCL_mesh_zhen = pcl_mesh;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr mesh_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::fromPCLPointCloud2(PCL_mesh_zhen->cloud, *mesh_cloud);

    // clear and reserve the
    CGAL_mesh.clear();
    int n = mesh_cloud->size();
    int f = PCL_mesh_zhen->polygons.size();
    int e = 0;
    CGAL_mesh.reserve(n, 2 * f, e);

    //copy the vertices
    double x, y, z;
    for (int i = 0; i < mesh_cloud->size(); i++)
    {
        K::Point_3 p;
        x = mesh_cloud->points[i].x;
        y = mesh_cloud->points[i].y;
        z = mesh_cloud->points[i].z;
        p = K::Point_3(x, y, z);
        CGAL_mesh.add_vertex(p);
    }

    // copy the faces
    std::vector <int> vertices;
    for (int i = 0; i < PCL_mesh_zhen->polygons.size(); i++)
    {
        vertices.resize(3);
        vertices[0] = PCL_mesh_zhen->polygons[i].vertices[0];
        vertices[1] = PCL_mesh_zhen->polygons[i].vertices[1];
        vertices[2] = PCL_mesh_zhen->polygons[i].vertices[2];
        CGAL_mesh.add_face(C_Mesh::Vertex_index(vertices[0]),
            C_Mesh::Vertex_index(vertices[1]),
            C_Mesh::Vertex_index(vertices[2]));
    }

    //CGAL::IO::write_polygon_mesh("F:/PCD_data/shengcheng/CGAL.off", CGAL_mesh, CGAL::parameters::stream_precision(17));
    return 0;
}

int osgAlgorithm::off2obj(string input_path, string output_path)
{
    float vertex[10000][3];
    int surface[10000][3];

    char k;
    int vertex_num;
    int surface_num;
    int other_num;
    int i, j;
    int s;
    ifstream fin;
    ofstream fout;
    fin.open(input_path);
    while (fin.fail())
    {
        cout << "Fail to open the off file!" << endl;
        exit(1);
    }

    if (output_path == "A") {
        string path2;
        path2 = input_path;
        path2 = path2.replace(path2.length() - 3, 3, "obj");
        output_path = path2;
    }


    fout.open(output_path);

    while (fout.fail())
    {
        cout << "Fail to open the obj file!" << endl;
        exit(1);
    }
    do
    {
        cout << fin.get();
    } while (fin.get() != '\n');
    fin >> vertex_num >> surface_num >> other_num;
    cout << vertex_num;
    for (i = 0; i < vertex_num; i++)
    {
        for (j = 0; j < 3; j++)
        {
            fin >> vertex[i][j];
        }
    }
    for (i = 0; i < surface_num; i++)
    {
        fin >> s;
        cout << s << endl;
        for (j = 0; j < 3; j++)
        {
            fin >> surface[i][j];
        }
    }

    for (i = 0; i < vertex_num; i++)
    {
        fout.put('v');
        fout.put(' ');
        for (j = 0; j < 3; j++)
        {
            fout << vertex[i][j];
            fout.put(' ');
        }
        fout.put('\n');

    }
    fout.put('\n');

    for (i = 0; i < surface_num; i++)
    {
        fout.put('f');
        fout.put(' ');
        for (j = 0; j < 3; j++)
        {
            fout << (surface[i][j] + 1);
            fout.put(' ');
        }
        fout.put('\n');
    }
    fin.close();
    fout.close();
    cout << "end" << endl;
    return 0;
}



//CGAL_mesh
int osgAlgorithm::mesh(Point_set p, int a, std::string path)
{
    PointNormal_set points;
    points = p;
    //std::string fname ;
    //if (argc < 2)
    //{
    //    std::cerr << "Usage: " << argv[0] << " [input.xyz/off/ply/las]" << std::endl;
    //    std::cerr << "Running " << argv[0] << " data/kitten.xyz -1\n";
    //}

    //std::ifstream stream(fname, std::ios_base::binary);

    //if (!stream)
    //{
    //    std::cerr << "Error: cannot read file " << fname << std::endl;
    //    return EXIT_FAILURE;
    //}

    //stream >> points;//input===========================================

    //std::cout << "Read " << points.size() << " point(s)" << std::endl;
    //if (points.empty())
    //    return EXIT_FAILURE;
    typename PointNormal_set::iterator rout_it = CGAL::remove_outliers<CGAL::Sequential_tag>
        (points,
            24, // Number of neighbors considered for evaluation
            points.parameters().threshold_percent(5.0)); // Percentage of points to remove
    points.remove(rout_it, points.end());

    std::cout << points.number_of_removed_points()
        << " point(s) are outliers." << std::endl;
    // Applying point set processing algorithm to a CGAL::PointNormal_set_3
    // object does not erase the points from memory but place them in
    // the garbage of the object: memory can be freeed by the user.
    points.collect_garbage();
    // Compute average spacing using neighborhood of 6 points
    double spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>(points, 6);
    // Simplify using a grid of size 2 * average spacing
    typename PointNormal_set::iterator gsim_it = CGAL::grid_simplify_point_set(points, 2. * spacing);
    points.remove(gsim_it, points.end());

    std::cout << points.number_of_removed_points()
        << " point(s) removed after simplification." << std::endl;
    points.collect_garbage();
    CGAL::jet_smooth_point_set<CGAL::Sequential_tag>(points, 24);


    int reconstruction_choice = a;

    if (reconstruction_choice == 0) // Poisson
    {
        CGAL::jet_estimate_normals<CGAL::Sequential_tag>
            (points, 24); // Use 24 neighbors
        // Orientation of normals, returns iterator to first unoriented point
        typename PointNormal_set::iterator unoriented_points_begin =
            CGAL::mst_orient_normals(points, 24); // Use 24 neighbors
        points.remove(unoriented_points_begin, points.end());
        CGAL::Surface_mesh<Point> output_mesh;
        CGAL::poisson_surface_reconstruction_delaunay
        (points.begin(), points.end(),
            points.point_map(), points.normal_map(),
            output_mesh, spacing);
        std::ofstream f(path, std::ios_base::binary);
        CGAL::IO::set_binary_mode(f);
        CGAL::IO::write_PLY(f, output_mesh);
        f.close();
    }
    if (reconstruction_choice == 1) // Advancing front
    {

        typedef std::array<std::size_t, 3> Facet; // Triple of indices
        std::vector<Facet> facets;
        // The function is called using directly the points raw iterators
        CGAL::advancing_front_surface_reconstruction(points.points().begin(),
            points.points().end(),
            std::back_inserter(facets));

        std::cout << facets.size()
            << " facet(s) generated by reconstruction." << std::endl;
        // copy points for random access
        std::vector<Point> vertices;
        vertices.reserve(points.size());
        std::copy(points.points().begin(), points.points().end(), std::back_inserter(vertices));
        CGAL::Surface_mesh<Point> output_mesh;
        CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(vertices, facets, output_mesh);
        CGAL::IO::write_polygon_mesh(path, output_mesh, CGAL::parameters::stream_precision(17));
        //std::ofstream f(path);
        //f << output_mesh;
        //f.close();
    }
    if (reconstruction_choice == 2) // Scale space
    {
        CGAL::Scale_space_surface_reconstruction_3<K> reconstruct
        (points.points().begin(), points.points().end());
        // Smooth using 4 iterations of Jet Smoothing
        reconstruct.increase_scale(4, CGAL::Scale_space_reconstruction_3::Jet_smoother<K>());
        // Mesh with the Advancing Front mesher with a maximum facet length of 0.5
        reconstruct.reconstruct_surface(CGAL::Scale_space_reconstruction_3::Advancing_front_mesher<K>(0.5));


        //yinjian=====begin path2 to path1
        string path2;
        path2 = path;
        path2 = path2.replace(path2.length() - 3, 3, "off");

        std::ofstream f(path2); //yangjian   path2=path
        f << "OFF" << std::endl << points.size() << " "
            << reconstruct.number_of_facets() << " 0" << std::endl;
        for (PointNormal_set::Index idx : points)
            f << points.point(idx) << std::endl;
        for (const auto& facet : CGAL::make_range(reconstruct.facets_begin(), reconstruct.facets_end()))
            f << "3 " << facet << std::endl;

        f.close();

        CGAL::Surface_mesh<K::Point_3> cgal_mesh;
        PMP::IO::read_polygon_mesh(path2, cgal_mesh);
        CGAL::IO::write_polygon_mesh(path, cgal_mesh, CGAL::parameters::stream_precision(17));

        remove(path2.c_str());

        //yinjian=====end

    }
    else // Handle error
    {
        //std::cerr << "Error: invalid reconstruction id: " << reconstruction_choice << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}


//CGAL mesh===================================================================================================

Polyhedron osgAlgorithm::hole_fill(CGAL::Surface_mesh<K::Point_3> mesh)
{
    Polyhedron poly;

    CGAL::copy_face_graph(mesh, poly);

    //if (!PMP::IO::read_polygon_mesh(filename, poly))
   // {
    //    std::cerr << "Invalid input." << std::endl;
    //    return 1;
   // }


    // Incrementally fill the holes
    unsigned int nb_holes = 0;
    for (Halfedge_handle h : halfedges(poly))
    {
        if (h->is_border())
        {
            std::vector<Facet_handle>  patch_facets;
            std::vector<Vertex_handle> patch_vertices;
            bool success = std::get<0>(PMP::triangulate_refine_and_fair_hole(poly,
                h,
                std::back_inserter(patch_facets),
                std::back_inserter(patch_vertices),
                CGAL::parameters::vertex_point_map(get(CGAL::vertex_point, poly))
                .geom_traits(K())));
            std::cout << " Number of facets in constructed patch: " << patch_facets.size() << std::endl;
            std::cout << " Number of vertices in constructed patch: " << patch_vertices.size() << std::endl;
            std::cout << " Fairing : " << (success ? "succeeded" : "failed") << std::endl;
            nb_holes++;
        }
    }
    std::cout << std::endl;
    std::cout << nb_holes << " holes have been filled" << std::endl;
    std::ofstream out("F:/PCD_data/shengcheng/fff.off");
    out.precision(17);
    out << poly << std::endl;
    return poly;
}



bool is_small_hole(halfedge_descriptor h, C_Mesh& mesh, double max_hole_diam, int max_num_hole_edges)
{
    int num_hole_edges = 0;
    CGAL::Bbox_3 hole_bbox;
    for (halfedge_descriptor hc : CGAL::halfedges_around_face(h, mesh))
    {
        const Point& p = mesh.point(target(hc, mesh));
        hole_bbox += p.bbox();
        ++num_hole_edges;

        if (num_hole_edges > max_num_hole_edges) return false;
        if (hole_bbox.xmax() - hole_bbox.xmin() > max_hole_diam) return false;
        if (hole_bbox.ymax() - hole_bbox.ymin() > max_hole_diam) return false;
        if (hole_bbox.zmax() - hole_bbox.zmin() > max_hole_diam) return false;
    }
    return true;
}


//CGAL::Surface_mesh<K::Point_3> osgAlgorithm::hole_fill1(CGAL::Surface_mesh<K::Point_3> mesh, double max_hole_diam = -1.0, int max_num_hole_edges = -1)
CGAL::Surface_mesh<K::Point_3> osgAlgorithm::smallhole_fill(CGAL::Surface_mesh<K::Point_3> mesh, double r, int n) // r = 0.1 n = 10
{

    // Incrementally fill the holes that are no larger than given diameter
    // and with no more than a given number of edges (if specified).

    /*
        const std::string filename = (argc > 1) ? argv[1] : CGAL::data_file_path("data/mech-holes-shark.off");
        Mesh mesh;
        if (!PMP::IO::read_polygon_mesh(filename, mesh))
        {
            std::cerr << "Invalid input." << std::endl;
            return 1;
        }
    */


    double max_hole_diam = r;
    int max_num_hole_edges = n;
    unsigned int nb_holes = 0;
    std::vector<halfedge_descriptor> border_cycles;


    PMP::extract_boundary_cycles(mesh, std::back_inserter(border_cycles));
    for (halfedge_descriptor h : border_cycles)
    {
        if (!is_small_hole(h, mesh, max_hole_diam, max_num_hole_edges))
            continue;
        std::vector<face_descriptor>  patch_facets;
        std::vector<vertex_descriptor> patch_vertices;
        //triangulate_refine_and_fair_hole(PolygonMesh& pmesh,typename boost::graph_traits<PolygonMesh>::halfedge_descriptor border_halfedge, FaceOutputIterator face_out, VertexOutputIterator vertex_out, const NamedParameters& np = parameters::default_values())
        auto np = CGAL::parameters::default_values();

        bool success = std::get<0>(PMP::triangulate_refine_and_fair_hole(mesh, h, std::back_inserter(patch_facets), std::back_inserter(patch_vertices), np));

        std::cout << "* Number of facets in constructed patch: " << patch_facets.size() << std::endl;
        std::cout << "  Number of vertices in constructed patch: " << patch_vertices.size() << std::endl;
        std::cout << "  Is fairing successful: " << success << std::endl;
        nb_holes++;
    }
    std::cout << nb_holes << " " << std::endl;

    //CGAL::IO::write_polygon_mesh("F:/PCD_data/shengcheng/CGAL.off", CGAL_mesh, CGAL::parameters::stream_precision(17));
    return mesh;

}



//====================================================================================================

CGAL::Surface_mesh<K::Point_3> osgAlgorithm::mesh_shapesmooth(C_Mesh mesh, int a, double b)
//CGAL::Surface_mesh<K::Point_3> osgAlgorithm::mesh_shapesmooth(std::string path)
{

    // C_Mesh mesh;


     //std::string filename = path;

    // PMP::IO::read_polygon_mesh(filename, mesh);


     //CGAL::draw(mesh);
    const unsigned int nb_iterations = a;//5
    const double time = b;//0.3
    std::set<C_Mesh::Vertex_index> constrained_vertices;
    for (C_Mesh::Vertex_index v : vertices(mesh))
    {
        if (is_border(v, mesh))
            constrained_vertices.insert(v);
    }
    //std::cout << "Constraining: " << constrained_vertices.size() << " border vertices" << std::endl;
    CGAL::Boolean_property_map<std::set<C_Mesh::Vertex_index> > vcmap(constrained_vertices);
    //std::cout << "Smoothing shape... (" << nb_iterations << " iterations)" << std::endl;
    PMP::smooth_shape(mesh, time, CGAL::parameters::number_of_iterations(nb_iterations)
        .vertex_is_constrained_map(vcmap));
    CGAL::IO::write_polygon_mesh("F:/PCD_data/c/shapesmooth.off", mesh, CGAL::parameters::stream_precision(17));
    //std::cout << "Done!" << std::endl;
    //CGAL::draw(mesh);

    return mesh;

}



//mesh ===========================================================================================================================
CGAL::Surface_mesh<K::Point_3> osgAlgorithm::mesh_3jiaosmooth(CGAL::Surface_mesh<K::Point_3> mesh, int a)// input mesh  
{

    /*
        const std::string filename = argc > 1 ? argv[1] : CGAL::data_file_path("meshes/anchor_dense.off");

        if (!PMP::IO::read_polygon_mesh(filename, mesh))
        {
            std::cerr << "Invalid input." << std::endl;
            return 1;
        }

   */

   // Constrain edges with a dihedral angle over 60
    typedef boost::property_map<C_Mesh, CGAL::edge_is_feature_t>::type EIFMap;
    EIFMap eif = get(CGAL::edge_is_feature, mesh);
    PMP::detect_sharp_edges(mesh, 60, eif);
    int sharp_counter = 0;
    for (edge_descriptor e : edges(mesh))
        if (get(eif, e))
            ++sharp_counter;
    //std::cout << sharp_counter << " sharp edges" << std::endl;
    //const unsigned int nb_iterations = a;
    int nb_iterations = a;
    //std::cout << "Smoothing mesh... (" << nb_iterations << " iterations)" << std::endl;
    // Smooth with both angle and area criteria + Delaunay flips
    PMP::angle_and_area_smoothing(mesh, CGAL::parameters::number_of_iterations(nb_iterations)
        .use_safety_constraints(false) // authorize all moves
        .edge_is_constrained_map(eif));
    //CGAL::IO::write_polygon_mesh("mesh_smoothed.off", mesh, CGAL::parameters::stream_precision(17));
    //std::cout << "Done!" << std::endl;


    return mesh;

}



//mesh===========================================================================================================================

CGAL::Surface_mesh<K::Point_3> osgAlgorithm::mesh_simp(CGAL::Surface_mesh<K::Point_3> mesh, double a) //a=0.1
{

    C_Mesh surface_mesh = mesh;

    //std::ifstream is(path);
    //is >> surface_mesh;


    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    // In this example, the simplification stops when the number of undirected edges
    // drops below 10% of the initial count
    //double stop_ratio = 0.1;
    double stop_ratio = a;
    SMS::Count_ratio_stop_predicate<C_Mesh> stop(stop_ratio);

    int r = SMS::edge_collapse(surface_mesh, stop);

    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();

    std::cout << "\nFinished!\n" << r << " edges removed.\n" << surface_mesh.number_of_edges() << " final edges.\n";
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms" << std::endl;

    //CGAL::IO::write_polygon_mesh("out.off", surface_mesh, CGAL::parameters::stream_precision(17));

    return surface_mesh;
}


// remeshing
struct halfedge2edge
{
    halfedge2edge(const C_Mesh& m, std::vector<edge_descriptor>& edges)
        : m_mesh(m), m_edges(edges)
    {}
    void operator()(const halfedge_descriptor& h) const
    {
        m_edges.push_back(edge(h, m_mesh));
    }
    const C_Mesh& m_mesh;
    std::vector<edge_descriptor>& m_edges;
};

CGAL::Surface_mesh<K::Point_3> osgAlgorithm::remeshing(CGAL::Surface_mesh<K::Point_3> mesh, double a, int b)// a = 1  b = 5
{

    //CGAL::draw(mesh, "source mesh");
    double target_edge_length = a;
    unsigned int nb_iter = b;
    std::cout << "Split border...";
    std::vector<edge_descriptor> border;
    PMP::border_halfedges(faces(mesh), mesh, boost::make_function_output_iterator(halfedge2edge(mesh, border)));
    PMP::split_long_edges(border, target_edge_length, mesh);
    std::cout << "done." << std::endl;
    std::cout << "Start remeshing of " << "mesh"
        << " (" << num_faces(mesh) << " faces)..." << std::endl;
    PMP::isotropic_remeshing(faces(mesh), target_edge_length, mesh,
        CGAL::parameters::number_of_iterations(nb_iter)
        .protect_constraints(true)); //i.e. protect border, here
    std::cout << "Remeshing done." << std::endl;
    //CGAL::draw(mesh, "remeshing mesh");
    return mesh;
}


//int osgAlgorithm::regiongrowing(PointCloud<PointXYZRGB>::Ptr cloud, float x , float y , float z )
vector<int> osgAlgorithm::regiongrowing(PointCloud<PointXYZRGB>::Ptr cloud, float x, float y, float z, float a, float b)
{

  //PointCloud<PointXYZRGB>::Ptr cloud(new PointCloud<PointXYZRGB>);

  //std::string path;
  //path = "F:/PCD_data/shengcheng/yuan.pcd";

  //float x = 0, y = 0, z = 0;
  //x = 0.5536;
  //y = -2.6098;
  //z = -0.1851;

    int k = -1;

    //io::loadPCDFile(path, *cloud);
    search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
    PointCloud<Normal>::Ptr normal_(new PointCloud<Normal>);
    NormalEstimation<PointXYZRGB, Normal>ne;
    tree->setInputCloud(cloud);
    ne.setInputCloud(cloud);
    ne.setSearchMethod(tree);
    ne.setKSearch(10);
    ne.compute(*normal_);

    //3********************************************************************************
    int point_num = cloud->points.size();
    vector<int>nebor_index;
    vector<float>nebor_dis;
    int nebor_size(10);//int nebor_size=10
    vector<vector<int>>point_nebor;
    point_nebor.resize(point_num, nebor_index);
    for (int i_point = 0; i_point < point_num; i_point++)
    {
        if (abs(cloud->points[i_point].x - x) < 0.001 && abs(cloud->points[i_point].y - y) < 0.001 && abs(cloud->points[i_point].z - z) < 0.001) {
            k = i_point;
        }
        tree->nearestKSearch(cloud->points[i_point], nebor_size, nebor_index, nebor_dis);

        point_nebor[i_point].swap(nebor_index);//swap()

    }
    if (k == -1) {
        return vector<int>();
    }


    //return 0;
    //}

    //4**********************************************************************************
    vector<pair<float, int>>point_curvature_index;
    //point_curvature_index.front().first; // curvature
    //point_curvature_index.front().second;// index

    for (int i_point = 0; i_point < point_num; i_point++)
    {
        //point_curvature_index
        point_curvature_index.push_back(make_pair(normal_->points[i_point].curvature, i_point));
    }
    sort(point_curvature_index.begin(), point_curvature_index.end());
    //5*************************************************************************
    //float normal_threshold = cosf(10.0 / 180.0 * M_PI);
    float normal_threshold = cosf(a / 180.0 * M_PI);/
    float curvature_threshold = b;
    int counter_(0), segment_label(1);

    //int seed_index_orginal = point_curvature_index[0].second;
    int seed_index_orginal = k;

    vector<int>point_label;
    vector<int>seg_ave_num; 
    point_label.resize(point_num, 0);

    queue<int>seed;
    seed.push(seed_index_orginal);
    int counter_1(1);
    point_label[seed_index_orginal] = segment_label;
    while (!seed.empty())
    {
        int cur_seed = seed.front();
        //int cur_seed = k;
        seed.pop();
        int K_nebor(0);
        while (K_nebor < nebor_size)
        {

            int index_nebor = point_nebor[cur_seed][K_nebor];
            if (point_label[index_nebor] != 0)
            {
                K_nebor++;
                continue;
            }
            bool is_a_seed = false;

            Map<Vector3f> curr_seed(static_cast<float*>(normal_->points[cur_seed].normal));
            Map<Vector3f>seed_nebor(static_cast<float*>(normal_->points[index_nebor].normal));
            float dot_normal = fabsf(curr_seed.dot(seed_nebor));
            if (dot_normal < normal_threshold)
            {
                is_a_seed = false;
            }
            else if (normal_->points[index_nebor].curvature > curvature_threshold)
            {
                is_a_seed = false;
            }
            else
            {
                is_a_seed = true;
            }
            if (!is_a_seed)
            {
                K_nebor++;
                continue;
            }
            point_label[index_nebor] = segment_label;
            counter_1++;
            if (is_a_seed)
            {
                seed.push(index_nebor);
            }
            K_nebor++;
        }
    }

    //segment_label++;  
    // counter_ += counter_1; 

    seg_ave_num.push_back(point_num - counter_1);
    seg_ave_num.push_back(counter_1); 

    // for (int i_seg = 0; i_seg < point_num; i_seg++)
    // {
    //     int index_ = point_curvature_index[i_seg].second;
    //    if (point_label[index_] == -1)
    //     {
    //         seed_index_orginal = index_;
    //         break;
    //     }
    // }
    //}
    //prin("=============================================");
    //prin("%d %d ", counter_1, (point_num - counter_1));
    //OutputDebugPrintf( (counter_1) ,(point_num - counter_1));



    //************************************************
    vector<PointIndices>cluster_;
    PointIndices segments;
    //int seg_num = seg_ave_num.size();
    int seg_num = 2;
    cluster_.resize(seg_num, segments);

    for (int i_seg = 0; i_seg < seg_num; i_seg++) {
        cluster_[i_seg].indices.resize(seg_ave_num[i_seg], 0);
    }
    vector<int> counter;
    counter.resize(seg_num, 0);
    for (int i_point = 0; i_point < point_num; i_point++)
    {

        int segment_index = point_label[i_point];
        int nebor_idx = counter[segment_index]; 
        cluster_[segment_index].indices[nebor_idx] = i_point;
        counter[segment_index] += 1;
    }

    //***************************************

    vector<PointIndices>clusters;
    //int min_number = 6000, max_number = 1000000;
    for (int i_seg = 0; i_seg < seg_num; i_seg++)
    {
        //if (cluster_[i_seg].indices.size() > min_number && cluster_[i_seg].indices.size() < max_number)
        //{
        clusters.push_back(cluster_[i_seg]);
        //}


    }


    ////********************************************************
    //visualization::PCLVisualizer::Ptr viewer(new visualization::PCLVisualizer("view"));
    //PointCloud<PointXYZRGB>::Ptr color_point(new PointCloud<PointXYZRGB>);
    ////srand(time(nullptr));
    //vector<unsigned char>color;
    //for (int i_segment = 0; i_segment < clusters.size(); i_segment++)
    //    //for (int i_segment = 0; i_segment < 2; i_segment++)
    //{
    //    color.push_back(static_cast<unsigned char>(rand() % 256));
    //    color.push_back(static_cast<unsigned char>(rand() % 256));
    //    color.push_back(static_cast<unsigned char>(rand() % 256));
    //}

    //int color_index = 0;
    //for (int i_seg = 0; i_seg < clusters.size(); i_seg++)
    //    //for (int i_seg = 0; i_seg < 2; i_seg++)
    //{
    //    int clusters_size = clusters[i_seg].indices.size();
    //    for (int i_idx = 0; i_idx < clusters_size; i_idx++)
    //    {
    //        PointXYZRGB point;
    //        point.x = cloud->points[clusters[i_seg].indices[i_idx]].x;
    //        point.y = cloud->points[clusters[i_seg].indices[i_idx]].y;
    //        point.z = cloud->points[clusters[i_seg].indices[i_idx]].z;
    //        point.r = color[3 * color_index];
    //        point.g = color[3 * color_index + 1];
    //        point.b = color[3 * color_index + 2];
    //        color_point->push_back(point);
    //    }
    //    color_index++;
    //}
    //viewer->addPointCloud(color_point);
    //viewer->spin();


    vector<int> indices = clusters[1].indices;
    return indices;

}


PointCloud<PointXYZRGB>::Ptr osgAlgorithm::manualpolygontool(PointCloud<PointXYZRGB>::Ptr cloud, PointCloud<PointXYZRGB>::Ptr ployin, int k) {

    //PointCloud<PointXYZRGB>::Ptr osgAlgorithm::manualpolygontool(PointCloud<PointXYZRGB>::Ptr cloud, int k ) {
      //PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>());
    PointCloud<PointXYZRGB>::Ptr cloudout(new PointCloud<PointXYZRGB>());
    //cloud = cloud_zhen;

    //PointCloud<PointXYZRGB>::Ptr ployin(new PointCloud<PointXYZRGB>());
    PointCloud<PointXYZRGB>::Ptr ploygon(new PointCloud<PointXYZRGB>());


    //PCDReader RD;
    //RD.read("F:/PCD_data/shengcheng/yuan.pcd", *cloud);
    /*ployin->push_back(PointXYZ(0.1, 0.1, 0));
    ployin->push_back(PointXYZ(0.1, -0.1, 0));
    ployin->push_back(PointXYZ(-0.1, 0.1, 0));
    ployin->push_back(PointXYZ(-0.1, -0.1, 0));
    ployin->push_back(PointXYZ(-0.2, -0.2, 0));
    ployin->push_back(PointXYZ(0.15, 0.1, 0));*/

    /*
    ployin->push_back(pcl::PointXYZRGB(0.917,  -2.557, -0.116 , 0, 0, 0));
    ployin->push_back(pcl::PointXYZRGB(0.167,  -2.653, 0.396  , 0, 0, 0));
    ployin->push_back(pcl::PointXYZRGB(-0.395, -2.734, -0.426 , 0, 0, 0));
    ployin->push_back(pcl::PointXYZRGB(0.327,  -2.653, -0.95  , 0, 0, 0));
    */


    /*
    ployin->push_back(pcl::PointXYZRGB(-0.807, -3.725, -1.039, 0, 0, 0));
    ployin->push_back(pcl::PointXYZRGB(-0.904, -2.372, -1.106, 0, 0, 0));
    ployin->push_back(pcl::PointXYZRGB(-0.957, -2.142, 0.564, 0, 0, 0));
    ployin->push_back(pcl::PointXYZRGB(-0.846, -3.589, 0.427, 0, 0, 0));
    */

    ConvexHull<PointXYZRGB> HULL;
    HULL.setInputCloud(ployin);
    HULL.setDimension(2);     
    std::vector<pcl::Vertices> polygons;
    //std::vector<uint32_t> vertices;
    HULL.reconstruct(*ploygon, polygons);  

    pcl::CropHull<pcl::PointXYZRGB> SURF_HULL;
    SURF_HULL.setDim(2);
    SURF_HULL.setInputCloud(cloud);         
    SURF_HULL.setHullIndices(polygons);     
    SURF_HULL.setHullCloud(ploygon);       
    SURF_HULL.setCropOutside(false);       
    if (k == 1) {
        SURF_HULL.setCropOutside(true);
    }
    SURF_HULL.filter(*cloudout);


    //boost::shared_ptr<pcl::visualization::PCLVisualizer> for_visualizer_v(new pcl::visualization::PCLVisualizer("crophull display"));
    //for_visualizer_v->setBackgroundColor(255, 255, 255);

    //int v1(0);
    ////for_visualizer_v->createViewPort(0.0, 0.0, 0.33, 1, v1);
    //for_visualizer_v->createViewPort(0.0, 0.0, 0.5, 1, v1);
    //for_visualizer_v->setBackgroundColor(255, 255, 255, v1);
    //for_visualizer_v->addPointCloud(cloud, "cloud", v1);
    //for_visualizer_v->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, "cloud");

    //for_visualizer_v->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "cloud");
    //for_visualizer_v->addPolygon<pcl::PointXYZRGB>(ploygon, 0, .069 * 255, 0.2 * 255, "ployin", v1);

    ///*
    //int v2(0);
    //for_visualizer_v->createViewPort(0.33, 0.0, 0.66, 1, v2);
    //for_visualizer_v->setBackgroundColor(255, 255, 255, v2);
    //for_visualizer_v->addPointCloud(ploygon, "surface_hull", v2);
    //for_visualizer_v->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, "surface_hull");
    //for_visualizer_v->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 8, "surface_hull");
    //for_visualizer_v->addPolygon<pcl::PointXYZ>(ploygon, 0, .069 * 255, 0.2 * 255, "ploygon", v2);
    //*/

    //int v3(0);
    ////for_visualizer_v->createViewPort(0.66, 0.0, 1, 1, v3);
    //for_visualizer_v->createViewPort(0.5, 0.0, 1, 1, v3);
    //for_visualizer_v->setBackgroundColor(255, 255, 255, v3);
    //for_visualizer_v->addPointCloud(cloudout, "cloudout", v3);
    //for_visualizer_v->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, "cloudout");
    //for_visualizer_v->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "cloudout");

    //while (!for_visualizer_v->wasStopped())
    //{

    //    for_visualizer_v->spinOnce(1000);
    //}


    return cloudout;
}


std::vector<int> osgAlgorithm::getindex(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_zhen, PointCloud<PointXYZRGB>::Ptr keypoints)
{
    pcl::KdTreeFLANN<pcl::PointXYZRGB> kdtree;
    kdtree.setInputCloud(cloud_zhen);
    std::vector<float>pointNKNSquareDistance; 
    std::vector<int> pointIdxNKNSearch;

    std::vector<int> indices;

    for (size_t i = 0; i < (*keypoints).size(); i++)
    {
        kdtree.nearestKSearch((*keypoints).points[i], 1, pointIdxNKNSearch, pointNKNSquareDistance);
        // cout<<"the distance is:"<<pointNKNSquareDistance[0]<<endl;
        // cout<<"the indieces is:"<<pointIdxNKNSearch[0]<<endl;

        indices.push_back(pointIdxNKNSearch[0]);

    }

    return indices;
}



pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::del(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, vector<int> index)
{



    pcl::PointCloud<pcl::PointXYZRGB>::iterator r_index = cloud->begin();

    for (int i = index.size() - 1; i > 0; i--)
    {
        r_index = cloud->begin() + index[i];
        cloud->erase(r_index);

    }

    return cloud;


}

pcl::PointCloud<pcl::PointXYZRGB>::Ptr osgAlgorithm::del2(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, vector<int> index)
{

    int n = (*cloud).size();
    vector<int>I = {};
    //I.reserve(n);
    I.resize(n);
    for (int i = 0; i < n; i++)
    {
        I[i] = i;
    }

    int n1 = index.size() - 1;

    for (int i = n1; i >= 0; i--)
    {
        //int a = index[i];
        I.erase(I.begin() + index[i]);
    }

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud2(new PointCloud<PointXYZRGB>);
    pcl::copyPointCloud(*cloud, I, *cloud2);
    cloud = cloud2;

    return cloud;


}


//std::vector<int> getIndices(pointcloud::Ptr cloudin, pointcloud keypoints)

int osgAlgorithm::f1()
{


    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudin(new PointCloud<PointXYZ>);

    std::string path;
    path = "F:/PCD_data/yuan.pcd";



    io::loadPCDFile(path, *cloudin);



    PointCloud<PointXYZ> keypoints;

    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
    kdtree.setInputCloud(cloudin);
    std::vector<float>pointNKNSquareDistance; 
    std::vector<int> pointIdxNKNSearch;

    std::vector<int> indices;



    keypoints.push_back(PointXYZ(0.485, -2.618, 0.028));
    keypoints.push_back(PointXYZ(0.917, -2.557, -0.116));
    keypoints.push_back(PointXYZ(0.167, -2.653, 0.396));
    keypoints.push_back(PointXYZ(-0.395, -2.734, -0.426));
    keypoints.push_back(PointXYZ(0.327, -2.653, -0.95));

    for (size_t i = 0; i < keypoints.size(); i++)
    {
        kdtree.nearestKSearch(keypoints.points[i], 1, pointIdxNKNSearch, pointNKNSquareDistance);
        // cout<<"the distance is:"<<pointNKNSquareDistance[0]<<endl;
        // cout<<"the indieces is:"<<pointIdxNKNSearch[0]<<endl;

        //prin(" % d", pointIdxNKNSearch[0]);

        indices.push_back(pointIdxNKNSearch[0]);

    }

    //prin(" % d ", indices[1]);
    //prin(" % d % d % d % d", indices[0], indices[1], indices[2], indices[3], indices[4]);

    return 0;
}

// extract vertices which are at most k (inclusive)
// far from vertex v in the graph of edges
void extract_k_ring(Vertex_handle v,
    int k,
    std::vector<Vertex_handle>& qv)
{
    std::map<Vertex_handle, int>  D;
    qv.push_back(v);
    D[v] = 0;
    std::size_t current_index = 0;
    int dist_v;
    while (current_index < qv.size() && (dist_v = D[qv[current_index]]) < k)
    {
        v = qv[current_index++];
        Polyhedron::Halfedge_around_vertex_circulator e(v->vertex_begin()), e_end(e);
        do {
            Vertex_handle new_v = e->opposite()->vertex();
            if (D.insert(std::make_pair(new_v, dist_v + 1)).second)
                qv.push_back(new_v);
        } while (++e != e_end);
    }
}




Polyhedron osgAlgorithm::mesh_refine_fair(CGAL::Surface_mesh<K::Point_3> mesh)
{

    Polyhedron poly;


    //std::string path;
    //path = "F:/PCD_data/x/yuan_af.off";

    //C_Mesh mesh;

    //PMP::IO::read_polygon_mesh(path, mesh);

    CGAL::copy_face_graph(mesh, poly);


    std::vector<Polyhedron::Facet_handle>  new_facets;
    std::vector<Vertex_handle> new_vertices;
    PMP::refine(poly, faces(poly),
        std::back_inserter(new_facets),
        std::back_inserter(new_vertices),
        CGAL::parameters::density_control_factor(2.));


    std::ofstream refined_off("F:/PCD_data/x/refined.off");


    refined_off.precision(17);
    refined_off << poly;
    refined_off.close();


    std::cout << "Refinement added " << new_vertices.size() << " vertices." << std::endl;

    Polyhedron::Vertex_iterator v = poly.vertices_begin();
    std::advance(v, 82/*e.g.*/);
    std::vector<Vertex_handle> region;
    extract_k_ring(v, 12/*e.g.*/, region);
    bool success = PMP::fair(poly, region);


    std::cout << "Fairing : " << (success ? "succeeded" : "failed") << std::endl;
    std::ofstream faired_off("F:/PCD_data/x/refined+faired.off");


    faired_off.precision(17);
    faired_off << poly;
    faired_off.close();


    return poly;
}


int testMain0()
{

    Polyhedron poly;

    std::string path;
    path = "F:/PCD_data/x/yuan_af.off";

    C_Mesh mesh;

    PMP::IO::read_polygon_mesh(path, mesh);

    CGAL::copy_face_graph(mesh, poly);


    std::vector<Polyhedron::Facet_handle>  new_facets;
    std::vector<Vertex_handle> new_vertices;
    PMP::refine(poly, faces(poly),
        std::back_inserter(new_facets),
        std::back_inserter(new_vertices),
        CGAL::parameters::density_control_factor(2.));


    std::ofstream refined_off("F:/PCD_data/x/refined.off");


    refined_off.precision(17);
    refined_off << poly;
    refined_off.close();


    std::cout << "Refinement added " << new_vertices.size() << " vertices." << std::endl;

    Polyhedron::Vertex_iterator v = poly.vertices_begin();
    std::advance(v, 82/*e.g.*/);
    std::vector<Vertex_handle> region;
    extract_k_ring(v, 12/*e.g.*/, region);
    bool success = PMP::fair(poly, region);


    std::cout << "Fairing : " << (success ? "succeeded" : "failed") << std::endl;

    std::ofstream faired_off("F:/PCD_data/x/faired.off");


    faired_off.precision(17);
    faired_off << poly;
    faired_off.close();


    return 0;
}



/*
//int convert_mesh_from_CGAL_to_PCL(CGAL::Surface_mesh<K::Point_3> CGAL_mesh, pcl::PolygonMesh::Ptr old_PCL_mesh, pcl::PolygonMesh::Ptr PCL_mesh)
int convert_mesh_from_CGAL_to_PCL(CGAL::Surface_mesh<K::Point_3> CGAL_mesh, pcl::PolygonMesh::Ptr old_PCL_mesh, pcl::PolygonMesh::Ptr PCL_mesh)
{
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr mesh_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::fromPCLPointCloud2(old_PCL_mesh->cloud, *mesh_cloud);

    int i = 0;
    BOOST_FOREACH(Point_set v, vertices(CGAL_mesh))
    {
        mesh_cloud->points[i].x = CGAL_mesh[v].point.x();
        mesh_cloud->points[i].y = CGAL_mesh[v].point.y();
        mesh_cloud->points[i].z = CGAL_mesh[v].point.z();
        i++;
    }

    //BOOST_FOREACH(CGAL_vertex v, vertices(CGAL_mesh))
    //BOOST_FOREACH(CGAL_face f, faces(CGAL_mesh))

    pcl::toPCLPointCloud2(*mesh_cloud, PCL_mesh->cloud);

    return 0;
}

*/



/*
//int convert_mesh_from_PCL_to_CGAL(pcl::PolygonMesh::Ptr PCL_mesh, CGAL_Mesh& CGAL_mesh)
//int f5(pcl::PolygonMesh PCL_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh);
//int osgAlgorithm::f5(pcl::PolygonMesh pcl_mesh,  C_Mesh& CGAL_mesh)

//int osgAlgorithm::convert_mesh_from_PCL_to_CGAL(pcl::PolygonMesh pcl_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh)
int osgAlgorithm::f5(pcl::PolygonMesh pcl_mesh, CGAL::Surface_mesh<K::Point_3>& CGAL_mesh)
{


   // pcl::PolygonMesh::Ptr PCL_mesh_zhen;
   //PCL_mesh_zhen = (pcl::PolygonMesh::Ptr)&pcl_mesh;
   //pcl::PolygonMesh* ptr_pcl_mesh = &pcl_mesh;

    pcl::PolygonMesh::Ptr PCL_mesh_zhen(new pcl::PolygonMesh);
    *PCL_mesh_zhen = pcl_mesh;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr mesh_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::fromPCLPointCloud2(PCL_mesh_zhen->cloud, *mesh_cloud);

    // clear and reserve the
    CGAL_mesh.clear();
    int n = mesh_cloud->size();
    int f = PCL_mesh_zhen->polygons.size();
    int e = 0;
    CGAL_mesh.reserve(n, 2 * f, e);

    //copy the vertices
    double x, y, z;
    for (int i = 0; i < mesh_cloud->size(); i++)
    {
        K::Point_3 p;
        x = mesh_cloud->points[i].x;
        y = mesh_cloud->points[i].y;
        z = mesh_cloud->points[i].z;
        p = K::Point_3(x, y, z);
        CGAL_mesh.add_vertex(p);
    }

    // copy the faces
    std::vector <int> vertices;
    for (int i = 0; i < PCL_mesh_zhen->polygons.size(); i++)
    {
        vertices.resize(3);
        vertices[0] = PCL_mesh_zhen->polygons[i].vertices[0];
        vertices[1] = PCL_mesh_zhen->polygons[i].vertices[1];
        vertices[2] = PCL_mesh_zhen->polygons[i].vertices[2];
        CGAL_mesh.add_face(C_Mesh::Vertex_index(vertices[0]),
            C_Mesh::Vertex_index(vertices[1]),
            C_Mesh::Vertex_index(vertices[2]));
    }

    return 0;
}

*/
//void extract_k_ring(Vertex_handle v,
//    int k,
//    std::vector<Vertex_handle>& qv)
//{
//    std::map<Vertex_handle, int>  D;
//    qv.push_back(v);
//    D[v] = 0;
//    std::size_t current_index = 0;
//    int dist_v;
//    while (current_index < qv.size() && (dist_v = D[qv[current_index]]) < k)
//    {
//        v = qv[current_index++];
//        Polyhedron::Halfedge_around_vertex_circulator e(v->vertex_begin()), e_end(e);
//        do {
//            Vertex_handle new_v = e->opposite()->vertex();
//            if (D.insert(std::make_pair(new_v, dist_v + 1)).second)
//                qv.push_back(new_v);
//        } while (++e != e_end);
//    }
//}
