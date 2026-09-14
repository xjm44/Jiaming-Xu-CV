

#include "../include/osgview.h"
#include <pcl/compression/compression_profiles.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>  
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/compression/octree_pointcloud_compression.h>

#include <stdio.h>
#include <sstream>
#include <stdlib.h>

osgview::osgview()
    :cloud(new pcl::PointCloud<pcl::PointXYZRGB>), osgcolor(new osgColor)
{
    count = 1; 
    mesh_count = 1;
    extract_plane_count = 1;
    treat_plane_count = 1;
    merged_cloud = 1;
    screenshotIndex = 0;
}

void osgview::readPcd() {


    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud00(new pcl::PointCloud<pcl::PointXYZRGB>);

    if (
        pcl::io::loadPCDFile<pcl::PointXYZRGB>(filepath.toStdString(), *cloud00) == -1)  // load the file
    {
        QMessageBox::information(NULL, "Error", "Cannot read this .pcd file!");
    }
    else {

        
        checkCloudFile = true;

        cloud00 = (osgAlgo->read(1, filepath));


        //pcl::io::savePCDFileBinary("12456.pcd", *cloud00); 

        //cloud = (osgAlgo->downsampling(1, filepath));

        //cloud = (osgAlgo->voxel_downsampling(cloud_zhen));  
        //cloud = (osgAlgo->statistical_outlierremoval(cloud_zhen));
        //cloud = (osgAlgo->uniform_downsampling(cloud_zhen));
        //cloud = (osgAlgo->r_outlierremoval(cloud_zhen));
        //cloud = (osgAlgo->mls(cloud_zhen));

        //osgAlgo->mesh_3(cloud, "E:\\vsTest\\ConsoleApplication1\\ConsoleApplication1\\10_mesh1.ply");
        //osgAlgo->mesh_cube(cloud, "E:\\vsTest\\ConsoleApplication1\\ConsoleApplication1\\10_mesh2.ply");
        //osgAlgo->mesh_possion(cloud, "E:\\vsTest\\ConsoleApplication1\\ConsoleApplication1\\10_mesh3.ply");

        //cgal_cloud = osgAlgo->pcl2cgal(cloud_zhen);
        //osgAlgo->mesh(cgal_cloud, 1);
        //osgAlgo->mesh(cgal_cloud, 2);



        files.push_back(filepath);
        all_clouds.push_back(cloud00);

    }
   
    //cloud->points.shrink_to_fit();
    ifpcd = false;
}

void osgview::readPly() {
    //QMessageBox::information(NULL, "Error", filepath);

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud00(new pcl::PointCloud<pcl::PointXYZRGB>);

    if (
        pcl::io::loadPLYFile<pcl::PointXYZRGB>(filepath.toStdString(), *cloud00) == -1
        )  // load the file
    {
        QMessageBox::information(NULL, "Error", "Cannot read this .ply file!");
    }
    else {
       

        checkCloudFile = true;

        //cloud = all_clouds.at(all_clouds.size() - 1);
        /*pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud00(new pcl::PointCloud<pcl::PointXYZRGB>);
        *cloud = *cloud00;*/


        cloud00 = (osgAlgo->read(0, filepath));



        //cloud = (osgAlgo->downsampling(0, filepath));

        //cloud = (osgAlgo->voxel_downsampling(cloud,0.04));
        //cloud = (osgAlgo->statistical_outlierremoval(cloud,10,3));
        //cloud = (osgAlgo->uniform_downsampling(cloud_zhen));
        //cloud = (osgAlgo->statistical_outlierremoval(cloud_zhen));
        //cloud = (osgAlgo->r_outlierremoval(cloud_zhen));
        //cloud = (osgAlgo->mls(cloud));
        // 
        //pcl::PLYWriter writer;
        //writer.write<pcl::PointXYZRGB>("E:\\vsTest\\ConsoleApplication1\\ConsoleApplication1\\office_tr.ply", *cloud,false);

        //cout <<
        //    cloud00->points.size();
        //cout << "\n";

        files.push_back(filepath);
        all_clouds.push_back(cloud00);

    }
    //cloud->points.shrink_to_fit();
    ifply = false;
}

int osgview::receivePointCloud(QString f) {


    filepath = f;
    if (filepath.isEmpty()) { QMessageBox::information(NULL, "Error", "Empty File! Please Retry."); }
    else {
        if (filepath.toStdString().substr(filepath.toStdString().find_last_of(".") + 1) == "ply") {
            ifply = true;
            //QMessageBox::information(NULL, "Error", "ly");
            //label->setText("Loading...");            
            changeFilepath(filepath);
            return 1;
        }
        else if (filepath.toStdString().substr(filepath.toStdString().find_last_of(".") + 1) == "pcd") {
            //QMessageBox::information(NULL, "Error", "cd");
            ifpcd = true;
            //label->setText("Loading...");
            changeFilepath(filepath);
            return 1;
        }

        else {
            QMessageBox::information(NULL, "Error", "Only .pcd/.ply file accepted.");
        }
    }
    return -99;

}

osg::ref_ptr<osg::LOD> osgview::applyLOD(osg::ref_ptr<osg::Geode> node)
{


    //  problem(s): 
        //      - geode accept simplifier?


    // create 4 different LODs ===================================       need to create a label to show "loading" and "loaded"
    //QMessageBox::information(NULL, "a", "test");

    osg::ref_ptr<osg::Geode> node1 = dynamic_cast<osg::Geode*>(
        node->clone(osg::CopyOp::DEEP_COPY_ALL));

    osg::ref_ptr<osg::Geode> node2 = dynamic_cast<osg::Geode*>(
        node->clone(osg::CopyOp::DEEP_COPY_ALL));

    osg::ref_ptr<osg::Geode> node3 = dynamic_cast<osg::Geode*>(
        node->clone(osg::CopyOp::DEEP_COPY_ALL));

    osg::ref_ptr<osg::Geode> nodee = dynamic_cast<osg::Geode*>(
        node->clone(osg::CopyOp::DEEP_COPY_ALL));

    /*osgUtil::Simplifier simplifier;
    simplifier.setSampleRatio(0.8);
    node1->accept(simplifier);
    simplifier.setSampleRatio(0.5);
    node2->accept(simplifier);
    simplifier.setSampleRatio(0.1);
    node3->accept(simplifier);*/

    //============================================================
    
    //QMessageBox::information(NULL, "a", "done");
    osg::ref_ptr<osg::LOD> lod = new osg::LOD;
    // 0-550 range
    lod->addChild(node3, 550.0f, FLT_MAX);
    lod->addChild(node2, 350.0f, 550.0f);
    lod->addChild(node1, 120.0f, 350.0f);



    //simplifier.setSampleRatio(0.2);
    //simplifier.setMaximumError(2.0);

    //osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
    //pat->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
    //pat->addChild(node);
    //pat->accept(simplifier);


    //node->accept(simplifier);


    lod->addChild(node.get(), 0.0f, 100.0f);



    return lod;
}


osg::Vec4 osgview::setColor(int r, int g, int b)
{
    return osg::Vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}



void osgview::getGeode()
{
   
    // Intensity============================================================================================
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloudIntensity(new pcl::PointCloud<pcl::PointXYZI>); 
    // =====================================================================================================
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud123(new pcl::PointCloud<pcl::PointXYZRGB>);

    if (ifply) {
        readPly();
        pcl::io::loadPLYFile<pcl::PointXYZI>(filepath.toStdString(), *cloudIntensity); // Intensity
       
    }
    if (ifpcd) {
        readPcd();
        pcl::io::loadPCDFile<pcl::PointXYZI>(filepath.toStdString(), *cloudIntensity); // Intensity
     
    }

    if (checkCloudFile) {


        cloud123 = all_clouds.at(all_clouds.size() - 1);

        osgCloudData *cloudProperty = new osgCloudData; // store properties of each cloud
   
        remove_points(); //remove clicked red points
             
        geode1 = osg::ref_ptr<osg::Geode>(new osg::Geode());

        osg::ref_ptr<osg::Geometry> geometry1(new osg::Geometry());
        osg::ref_ptr<osg::Vec3Array> vertices1(new osg::Vec3Array());
        osg::ref_ptr<osg::Vec4Array> colors1(new osg::Vec4Array()); // rgb color
        osg::ref_ptr<osg::Vec4Array> colors_none(new osg::Vec4Array()); // color none
        std::vector<float> intensity_values; // store intensity values of each cloud


        for (int i = 0; i < cloud123->points.size(); i++) {

            vertices1->push_back(osg::Vec3(cloud123->points[i].x, cloud123->points[i].y, cloud123->points[i].z));
            
            // RGB============================================================================================
            uint32_t rgb_val_1;
            memcpy(&rgb_val_1, &(cloud123->points[i].rgb), sizeof(uint32_t));

            uint32_t red, green, blue;
            blue = rgb_val_1 & 0x000000ff;
            rgb_val_1 = rgb_val_1 >> 8;
            green = rgb_val_1 & 0x000000ff;
            rgb_val_1 = rgb_val_1 >> 8;
            red = rgb_val_1 & 0x000000ff;

            colors1->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
            // ===============================================================================================

            // store colors
            intensity_values.push_back(cloudIntensity->points[i].intensity);
            colors_none->push_back(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
          
        }

        geometry1->setVertexArray(vertices1.get());
        geometry1->setColorArray(colors1.get());
        geometry1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry1->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

        geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices1->size()));
        //geometry1->setCullingActive(true);
        osgUtil::SmoothingVisitor smv;
        smv.smooth(*geometry1);
        


        osg::StateSet* ss = geometry1->getOrCreateStateSet();
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));


        geode1->addDrawable(geometry1.get());


        // =========================== Face Front when inserted the new cloud =================================
        osg::Vec3d viewDirection;
        osg::Vec3d upDirection;
        viewDirection.set(0.0, -1.0, 0.0);
        upDirection.set(0.0, 1.0, 0.0);
        osg::Vec3d center = geode1->getBound().center();
        double viewDistance = 4.0 * geode1->getBound().radius();
        osg::Vec3d eyePoint = center + viewDirection * viewDistance;
        pViewer->getCameraManipulator()->setHomePosition(eyePoint, center, upDirection);
        pViewer->home();
        // ====================================================================================================

        geode1->setName("unmoved");

        osg::ref_ptr<osg::Switch> sub_gp = new osg::Switch; // store clouds as sub-group inside gp
        sub_gp->addChild(geode1);


        //osgDB::writeNodeFile(*geode1,std::string("temp.osg"));
        //osg::ref_ptr<osg::Node> node1 = osgDB::readNodeFile("temp.osg");
        //sub_gp->addChild(applyLOD(node1->asGeode()));

        //sub_gp->addChild(osgDB::readNodeFile("cow.osg"));
        
        //if (count == 2) {
        //    //sub_gp->addChild(osgDB::readNodeFile("cow.osg"));
        //    sub_gp->addChild(geode1);
        //}
        //else {
            //sub_gp->addChild(osgDB::readNodeFile("cessna.osg"));
        //}

        gp->addChild(sub_gp);
        //gp_all->addChild(gp);



        osgUtil::Optimizer optimizer;
        optimizer.optimize(gp);
        
        file = filepath;
        checkCloudFile = false;
        canShowName = true;




        // set cloud's property ================================================
        std::string str = filepath.toStdString();
        str = str.substr(str.find_last_of("\\")+1);
        QString x = QString::fromStdString(str);
        cloudProperty->setProperty(x);
        cloudProperty->setProperty("points", cloud123->points.size());
        cloudsData.push_back(cloudProperty);
        //======================================================================
        
        // store colors===========================
        osgcolor->storeColors(colors1);
        osgcolor->storeIntensity(intensity_values);
        osgcolor->storeNoColor(colors_none);

        pcl::PointXYZRGB min_pt, max_pt;
        pcl::getMinMax3D(*cloud, min_pt, max_pt);
        osgcolor->storeMinMax(min_pt, max_pt);
        //========================================

    }
    //return geode1;
}


void osgview::changeFilepath(QString f) {

    if(f[0] == 'f' && f[1] == 'i' && f[2] == 'l' && f[3] == 'e' && f[4] == ':' && f[5] == '/' && f[6] == '/' && f[7] == '/'){
        int index = 8; // remove file:/.......
        QString str = f.mid(index).simplified();
        QString strv2 = str.replace('/', '\\');
        filepath = strv2;
        //filepath = str;
    }
    else if (f[0] == 'f' && f[1] == 'i' && f[2] == 'l' && f[3] == 'e' && f[4] == ':' && f[5] == '/' && f[6] == '/') {
        int index =5; // remove file:/.......
        QString str = f.mid(index).simplified();
        QString strv2 = str.replace('/', '\\');
        filepath = strv2;
    }
    else {
        filepath = f.simplified();
    }

    getGeode();
}



void osgview::remove_points()
{

    for (int i = 0; i < no_of_redpoints; i++) {
        int index = gp->getNumChildren() - 1;
        gp->removeChild(index, 1);
    }
    no_of_redpoints = 0;

}

void osgview::show_color(int x, int parentindex, int cloudindex, ColorMap cmap, QString check)  //0: rgb, 1: intensity , 2: z ,3: y , 4: x , 5: none
{
                                                                                     // ====== TO BE DONE: COLOR READING FROM PARENT INDEX FOR NOW===============    
    osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(parentindex)->asSwitch();

    //QMessageBox::about(NULL,"A","A");
    //cloudindex = 0;
    //osg::PagedLOD* lod = (osg::PagedLOD*)(sub_gp->getChild(cloudindex));
    //osg::Geode* geode = (osg::Geode*)(lod->getChild(cloudindex));
    //QMessageBox::about(NULL, "A", "A11");
 

    osg::Geode* geode;
    if (sub_gp->getChild(cloudindex)->getName() == "unmoved") {
        geode = (osg::Geode*)sub_gp->getChild(cloudindex);
    }
    else if (sub_gp->getChild(cloudindex)->getName() == "moved") {
       // geode = (osg::Geode*)((osg::MatrixTransform*)((osg::Group*)((osg::MatrixTransform*)sub_gp->getChild(cloudindex))->getChild(0))->getChild(0))->getChild(0); // matrix - group - matrix - geode
        std::string x = "movedgeode";
        findNode* finder = new findNode(x);
        //sub_gp->accept(*finder);
        finder->apply(*sub_gp->getChild(cloudindex)->asNode());
        //if (finder->getNode() != NULL) { cout << "hi___________"; };
        geode = finder->getNode();
        if (!geode) {    // not a point cloud being moved
            cout << "meshed";
            return;
        }
    }
    else { // temporary
        geode = (osg::Geode*)sub_gp->getChild(cloudindex);
    }


    osg::Geometry* geometry = geode->getDrawable(0)->asGeometry();
    osg::ref_ptr<osg::Vec4Array> colors1(new osg::Vec4Array()); // cloud color

    if (x == 0) {
        /*geometry->setColorArray(osgcolor->getColor(cloudindex, true).get());
        geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);*/

    }else if (x == 1) {

        std::vector<float> intensities = osgcolor->getIntensity(parentindex);

        if (cmap == CMAP_blue_green) { //================================================GREEN BLUE=================================

            std::vector<float>::iterator iter;

            for (iter = intensities.begin(); iter != intensities.end(); iter++)
            {
                float v = *iter;

                float red, green, blue;
                red = 0.0f;

                if (v / 100.0 < 0) {
                    v = 0.0f;
                }
                if (v / 100.0 > 1) {
                    v = 1.0f;
                }
                green = (v / 100.0f);
                blue = 1 - (v / 100.0f);

                colors1->push_back(osg::Vec4f(0.0f, (float)green, (float)blue, 1.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);


        }
        else  if (cmap == CMAP_dip) { //================================================DIP=================================

            std::vector<float>::iterator iter;

            for (iter = intensities.begin(); iter != intensities.end(); iter++)
            {
                float v = *iter;

                float red, green, blue;
                v = v / 100.0f;

                if (v < 0) {
                    v = 0.0f;
                }
                if (v  > 1) {
                    v = 1.0f;
                }

                if (v >= 0.0f && v < 0.33f) {
                    red = ((v / 0.33f) * (255.0f - 129.0f) + 129.0f) / 255.0f;
                    green = ((v / 0.33f) * 68.0f) / 255.0f;
                    blue = 0.0f;
                }
                else if (v >= 0.33f && v < 0.66) {
                    red = 1.0f;
                    green = (((0.66f - v) / 0.33f) * (255.0f - 68.0f) + 68.0f) / 255.0f;
                    blue = 0.0f;
                }
                else {
                    red = 1.0f;
                    green = 1.0f;
                    blue = (v / 1.0f);
                }

                colors1->push_back(osg::Vec4f((float)red, (float)green, (float)blue, 1.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        }
        else  if (cmap == CMAP_grey) { //================================================GREY=================================

            std::vector<float>::iterator iter;

            for (iter = intensities.begin(); iter != intensities.end(); iter++)
            {
                float v = *iter;

                if (v / 100.0 < 0) {
                    v = 0.0f;
                }
                if (v / 100.0 > 1) {
                    v = 1.0f;
                }

                colors1->push_back(osg::Vec4f(v / 100.0f, v / 100.0f, v / 100.0f, 0.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        else  if (cmap == CMAP_BGYR) { //================================================BGYR=================================
         

            std::vector<float>::iterator iter;

            for (iter = intensities.begin(); iter != intensities.end(); iter++)
            {
                float v = *iter;
                float red, green, blue;
                
                v = v / 100.0f;
                if (v  < 0) {
                    v = 0.0f;
                }
                if (v  > 1) {
                    v = 1.0f;
                }
                

                if (v < 0.25) {
                    red = 0;
                    green = 4 * v;
                    blue = 1;
                }
                else if (v < 0.5) {
                    red = 0;
                    blue = 2 - 4 * v;
                    green = 1;
                }
                else if (v < 0.75) {
                    red = 4 * v - 2;
                    blue = 0;
                    green = 1;
                }
                else {
                    green = 4 * (1 - v);
                    blue = 0;
                    red = 1;
                }

                colors1->push_back(osg::Vec4f(red, green ,blue, 1.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }

        else {
            geometry->setColorArray(osgcolor->getColor(parentindex, true).get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }

    }
    else if (x == 2) {

        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*> (geometry->getVertexArray()); // get the vertices as a Vec3Array

        color_f c{}; float v;
        pcl::PointXYZRGB min_pt, max_pt;
        osgcolor->getMinMax(parentindex, min_pt, max_pt);
        float zmin = min_pt.z, zmax = max_pt.z;

        if(cmap==CMAP_none) {
            geometry->setColorArray(osgcolor->getColor(parentindex, true).get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        else {
            for (auto vertex : *vertices) {
                v = (vertex.z() - zmin) / (zmax - zmin);
                c = getRGB(v, cmap);

                colors1->push_back(osg::Vec4f(c.r, c.g, c.b, 1.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        


    }
    else if (x == 3) {
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*> (geometry->getVertexArray()); // get the vertices as a Vec3Array

        color_f c{}; float v;
        pcl::PointXYZRGB min_pt, max_pt;
        osgcolor->getMinMax(parentindex, min_pt, max_pt);
        float ymin = min_pt.y, ymax = max_pt.y;

        if (cmap == CMAP_none) {
            geometry->setColorArray(osgcolor->getColor(parentindex, true).get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        else {
            for (auto vertex : *vertices) {
                v = (vertex.y() - ymin) / (ymax - ymin);
                c = getRGB(v, cmap);

                colors1->push_back(osg::Vec4f(c.r, c.g, c.b, 1.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }

    }
    else if (x == 4) {

        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*> (geometry->getVertexArray()); // get the vertices as a Vec3Array

        color_f c{}; float v;
        pcl::PointXYZRGB min_pt, max_pt;
        osgcolor->getMinMax(parentindex, min_pt, max_pt);
        float xmin = min_pt.x, xmax = max_pt.x;

        if (cmap == CMAP_none) {
            geometry->setColorArray(osgcolor->getColor(parentindex, true).get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        else {
            for (auto vertex : *vertices) {
                v = (vertex.x() - xmin) / (xmax - xmin);
                c = getRGB(v, cmap);

                colors1->push_back(osg::Vec4f(c.r, c.g, c.b, 1.0f));
            }
            geometry->setColorArray(colors1.get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }

    }
    else if (x == 5) {

        //geometry->setColorArray(osgcolor->getColor(cloudindex, false).get());
        //geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    }
}

osgview::~osgview()
{}






osg::Camera* osgview::makeCoordinate(osgViewer::Viewer* viewer, int width, int height)
{


    //osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

    //osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    //v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    //v->push_back(osg::Vec3(25.0f, 0.0f, 0.0f));
    //v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    //v->push_back(osg::Vec3(0.0f, 25.0f, 0.0f));
    //v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
    //v->push_back(osg::Vec3(0.0f, 0.0f, 25.0f));
    //geom->setVertexArray(v.get());


 
    //osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    //c->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 
    //c->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); //x red
    //c->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    //c->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); //y green
    //c->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); 
    //c->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); //z blue

    //geom->setColorArray(c.get());
    //geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);


    //geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2)); //X
    //geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 2, 2)); //Y
    //geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 4, 2)); //Z


    //osg::ref_ptr<osgText::Text> pTextXAuxis = new osgText::Text;
    //pTextXAuxis->setText(L"X");
    //pTextXAuxis->setFont("Fonts/simhei.ttf");
    //pTextXAuxis->setAxisAlignment(osgText::Text::SCREEN);
    //pTextXAuxis->setCharacterSize(35);
    //pTextXAuxis->setPosition(osg::Vec3(25.0f, 0.0f, 0.0f));
    //pTextXAuxis->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

    //osg::ref_ptr<osgText::Text> pTextYAuxis = new osgText::Text;
    //pTextYAuxis->setText(L"Y");
    //pTextYAuxis->setFont("Fonts/simhei.ttf");
    //pTextYAuxis->setAxisAlignment(osgText::Text::SCREEN);
    //pTextYAuxis->setCharacterSize(35);
    //pTextYAuxis->setPosition(osg::Vec3(0.0f, 25.0f, 0.0f));
    //pTextYAuxis->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));


    //osg::ref_ptr<osgText::Text> pTextZAuxis = new osgText::Text;
    //pTextZAuxis->setText(L"Z");
    //pTextZAuxis->setFont("Fonts/simhei.ttf");
    //pTextZAuxis->setAxisAlignment(osgText::Text::SCREEN);
    //pTextZAuxis->setCharacterSize(35);
    //pTextZAuxis->setPosition(osg::Vec3(0.0f, 0.0f, 25.0f));
    //pTextZAuxis->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

    //osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    //geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    //geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.5), osg::StateAttribute::ON);


    //geode->addDrawable(geom.get());
    //geode->addDrawable(pTextXAuxis.get());
    //geode->addDrawable(pTextYAuxis.get());
    //geode->addDrawable(pTextZAuxis.get());


    osg::ref_ptr<osg::Camera> hudCamera = new osg::Camera;
    hudCamera->setProjectionMatrixAsOrtho(-10, width, -10, height, 1, 100);
    hudCamera->setViewMatrix(osg::Matrix::identity());
    hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    //hudCamera->setViewport(200, 200, 500, 500);
    //hudCamera->setClearColor(osg::Vec4(1, 0.5, 0.5, 0.5));
    hudCamera->setRenderOrder(osg::Camera::POST_RENDER);

    osg::Node* coor = osgDB::readNodeFile("axes.osgt");
    coor->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::MatrixTransform* mt = new osg::MatrixTransform;
    mt->addChild(coor);
    mt->setMatrix(osg::Matrix::scale(osg::Vec3(width / 18, width / 18, width / 18)) * osg::Matrix::translate(osg::Vec3(25, 30, 1)));
    //mt->addChild(geode.release()); // own drawing
    //mt->setMatrix(osg::Matrix::scale(osg::Vec3(4.5, 4.5, 4.5)) * osg::Matrix::translate(osg::Vec3(25, 30, 1)));
    mt->setUpdateCallback(new HudCallback(viewer)); // able to move
    hudCamera->addChild(mt);
    hudCamera->setViewMatrixAsLookAt(osg::Vec3(0, 0, 1), osg::Vec3(0,0,0), osg::Vec3(0, 1, 0)); // camera pos (look at)

    return hudCamera.release();

}

osg::Camera* osgview::makeCenterX() {

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;


    osg::ref_ptr<osg::Vec3Array> vex = new osg::Vec3Array;
    vex->push_back(osg::Vec3(-3.0, 0.0, 3.0));
    vex->push_back(osg::Vec3(3.0, 0.0, -3.0));
    vex->push_back(osg::Vec3(-3.0, 0.0, -3.0));
    vex->push_back(osg::Vec3(3.0, 0.0, 3.0));

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    geometry->setVertexArray(vex);


    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

    colors->push_back(osg::Vec4f(1.0, 1.0, 1.0, 0));
    colors->push_back(osg::Vec4f(1.0, 1.0, 1.0, 0));
    colors->push_back(osg::Vec4f(1.0, 1.0, 1.0, 0));
    colors->push_back(osg::Vec4f(1.0, 1.0, 1.0, 0));
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 4);
    geometry->addPrimitiveSet(primitiveSet);


    osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(1.0);
    osg::StateSet* ss = geometry->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setAttribute(lw);

    geode->addDrawable(geometry);


    // HUD 
    osg::ref_ptr < osg::Camera > camera = new osg::Camera;
    camera->setProjectionMatrixAsOrtho(-180, 200, -200, 200, 0, 200);
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());

    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setAllowEventFocus(false);
    camera->setRenderOrder(osg::Camera::POST_RENDER);

    osg::MatrixTransform* mt = new osg::MatrixTransform;
    mt->addChild(geode);
    mt->setMatrix(osg::Matrix::scale(osg::Vec3(0.8, 0.8, 0.8)));
    camera->addChild(mt);
    camera->setViewMatrixAsLookAt(osg::Vec3(0, 1, 0), osg::Vec3(0, 0, 0), osg::Vec3(0, 0, 1)); // camera pos (look at)

    return camera.release();
}

osg::ref_ptr<osg::Geode> osgview::createGeode(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, const char* name) {

    osg::ref_ptr<osg::Geode> geode1 = new osg::Geode(); // new point cloud
    osg::ref_ptr<osg::Geometry> geometry1(new osg::Geometry());
    osg::ref_ptr<osg::Vec3Array> vertices1(new osg::Vec3Array());
    osg::ref_ptr<osg::Vec4Array> colors1(new osg::Vec4Array()); // rgb color

    if (cloud) {

        for (int i = 0; i < cloud->points.size(); i++) {

            vertices1->push_back(osg::Vec3(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z));

            // RGB============================================================================================
            uint32_t rgb_val_1;
            memcpy(&rgb_val_1, &(cloud->points[i].rgb), sizeof(uint32_t));

            uint32_t red, green, blue;
            blue = rgb_val_1 & 0x000000ff;
            rgb_val_1 = rgb_val_1 >> 8;
            green = rgb_val_1 & 0x000000ff;
            rgb_val_1 = rgb_val_1 >> 8;
            red = rgb_val_1 & 0x000000ff;

            colors1->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
            // ===============================================================================================

        }

        geometry1->setVertexArray(vertices1.get());
        geometry1->setColorArray(colors1.get());
        geometry1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry1->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

        geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices1->size()));
        osgUtil::SmoothingVisitor smv;
        smv.smooth(*geometry1);

        osg::StateSet* ss = geometry1->getOrCreateStateSet();
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));

        geode1->addDrawable(geometry1.get());
        geode1->setName(name);
    }
    else {

    }
    return geode1;
}

//void osgview::getGeode()
//{
//    // Intensity============================================================================================
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloudIntensity(new pcl::PointCloud<pcl::PointXYZI>); 
//    // =====================================================================================================
//
// 
//    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud123(new pcl::PointCloud<pcl::PointXYZRGB>);
//    if (ifply) {
//        readPly();
//        pcl::io::loadPLYFile<pcl::PointXYZI>(filepath.toStdString(), *cloudIntensity); // Intensity
//    }
//    if (ifpcd) {
//        readPcd();
//        pcl::io::loadPCDFile<pcl::PointXYZI>(filepath.toStdString(), *cloudIntensity); // Intensity
//    }
//
//    if (checkCloudFile) {
//
// 
//        cloud123 = all_clouds.at(all_clouds.size() - 1);
//        osgCloudData *cloudProperty = new osgCloudData; // store properties of each cloud
//   
//        remove_points(); //remove clicked red points
//
//
//        osg::ref_ptr<osg::Geode> geode0 = osg::ref_ptr<osg::Geode>(new osg::Geode());
//
//        osg::ref_ptr<osg::Geometry> geometry0(new osg::Geometry());
//        osg::ref_ptr<osg::Vec3Array> vertices0(new osg::Vec3Array());
//        osg::ref_ptr<osg::Vec4Array> colors0(new osg::Vec4Array()); // rgb color
//        osg::ref_ptr<osg::Vec4Array> colors_none(new osg::Vec4Array()); // color none
//        std::vector<float> intensity_values0; // store intensity values of each cloud
//        for (int i = 0; i < cloud0->points.size(); i++) {
//
//            vertices0->push_back(osg::Vec3(cloud0->points[i].x, cloud0->points[i].y, cloud0->points[i].z));
//
//            // RGB============================================================================================
//            uint32_t rgb_val_1;
//            memcpy(&rgb_val_1, &(cloud0->points[i].rgb), sizeof(uint32_t));
//
//            uint32_t red, green, blue;
//            blue = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            green = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            red = rgb_val_1 & 0x000000ff;
//
//            colors0->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
//            // ===============================================================================================
//
//            // store colors
//            //intensity_values.push_back(cloudIntensity->points[i].intensity);
//            colors_none->push_back(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//
//        }
//
//        geometry0->setVertexArray(vertices0.get());
//        geometry0->setColorArray(colors0.get());
//        geometry0->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry0->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry0->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices0->size()));
//        osgUtil::SmoothingVisitor smv;
//        smv.smooth(*geometry0);
//        osg::StateSet* ss = geometry0->getOrCreateStateSet();
//        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//        ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));
//        geode0->addDrawable(geometry0.get());
//             
//
//
//
//        geode1 = osg::ref_ptr<osg::Geode>(new osg::Geode());
//        osg::ref_ptr<osg::Geometry> geometry1(new osg::Geometry());
//        osg::ref_ptr<osg::Vec3Array> vertices1(new osg::Vec3Array());
//        osg::ref_ptr<osg::Vec4Array> colors1(new osg::Vec4Array()); // rgb color
//        osg::ref_ptr<osg::Vec4Array> colors_none1(new osg::Vec4Array()); // color none
//        std::vector<float> intensity_values; // store intensity values of each cloud
//        for (int i = 0; i < cloud1->points.size(); i++) {
//
//            vertices1->push_back(osg::Vec3(cloud1->points[i].x, cloud1->points[i].y, cloud1->points[i].z));
//
//            // RGB============================================================================================
//            uint32_t rgb_val_1;
//            memcpy(&rgb_val_1, &(cloud1->points[i].rgb), sizeof(uint32_t));
//
//            uint32_t red, green, blue;
//            blue = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            green = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            red = rgb_val_1 & 0x000000ff;
//
//            colors1->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
//            // ===============================================================================================
//
//            // store colors
//            //intensity_values.push_back(cloudIntensity->points[i].intensity);
//            colors_none1->push_back(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//          
//        }
//
//        geometry1->setVertexArray(vertices1.get());
//        geometry1->setColorArray(colors1.get());
//        geometry1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry1->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices1->size()));
//        smv.smooth(*geometry1);
//        osg::StateSet* ss0 = geometry1->getOrCreateStateSet();
//        ss0->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//        ss0->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));
//        geode1->addDrawable(geometry1.get());
//
//
//        osg::ref_ptr<osg::Geode> geode11 = osg::ref_ptr<osg::Geode>(new osg::Geode());
//        osg::ref_ptr<osg::Geometry> geometry11(new osg::Geometry());
//        osg::ref_ptr<osg::Vec3Array> vertices11(new osg::Vec3Array());
//        osg::ref_ptr<osg::Vec4Array> colors11(new osg::Vec4Array()); // rgb color
//        for (int i = 0; i < cloud2->points.size() ; i++) {
//
//            vertices11->push_back(osg::Vec3(cloud2->points[i].x, cloud2->points[i].y, cloud2->points[i].z));
//
//            // RGB============================================================================================
//            uint32_t rgb_val_1;
//            memcpy(&rgb_val_1, &(cloud2->points[i].rgb), sizeof(uint32_t));
//
//            uint32_t red, green, blue;
//            blue = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            green = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            red = rgb_val_1 & 0x000000ff;
//
//            colors11->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
//            // ===============================================================================================
//
//            // store colors
//            //intensity_values.push_back(cloudIntensity->points[i].intensity);
//            colors_none->push_back(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//
//        }
//
//        geometry11->setVertexArray(vertices11.get());
//        geometry11->setColorArray(colors11.get());
//        geometry11->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry11->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry11->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices11->size()));
//
//        smv.smooth(*geometry11);
//        osg::StateSet* ss1 = geometry11->getOrCreateStateSet();
//        ss1->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//        ss1->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));
//        geode11->addDrawable(geometry11.get());
//
//
//
//        osg::ref_ptr<osg::Geode> geode111 = osg::ref_ptr<osg::Geode>(new osg::Geode());
//        osg::ref_ptr<osg::Geometry> geometry111(new osg::Geometry());
//        osg::ref_ptr<osg::Vec3Array> vertices111(new osg::Vec3Array());
//        osg::ref_ptr<osg::Vec4Array> colors111(new osg::Vec4Array()); // rgb color
//        for (int i = 0 ; i < cloud3->points.size(); i++) {
//
//            vertices111->push_back(osg::Vec3(cloud3->points[i].x, cloud3->points[i].y, cloud3->points[i].z));
//
//            // RGB============================================================================================
//            uint32_t rgb_val_1;
//            memcpy(&rgb_val_1, &(cloud3->points[i].rgb), sizeof(uint32_t));
//
//            uint32_t red, green, blue;
//            blue = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            green = rgb_val_1 & 0x000000ff;
//            rgb_val_1 = rgb_val_1 >> 8;
//            red = rgb_val_1 & 0x000000ff;
//
//            colors111->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
//            // ===============================================================================================
//
//            // store colors
//            //intensity_values.push_back(cloudIntensity->points[i].intensity);
//            colors_none->push_back(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//
//        }
//
//        geometry111->setVertexArray(vertices111.get());
//        geometry111->setColorArray(colors111.get());
//        geometry111->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry111->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
//        geometry111->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices111->size()));
//
//        smv.smooth(*geometry111);
//        osg::StateSet* ss11 = geometry111->getOrCreateStateSet();
//        ss11->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//        ss11->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));
//        geode111->addDrawable(geometry111.get());
//
//
//
//
//        // =========================== Face Front when inserted the new cloud =================================
//        osg::Vec3d viewDirection;
//        osg::Vec3d upDirection;
//        viewDirection.set(0.0, -1.0, 0.0);
//        upDirection.set(0.0, 1.0, 0.0);
//        osg::Vec3d center = geode1->getBound().center();
//        double viewDistance = 4.0 * geode1->getBound().radius();
//        osg::Vec3d eyePoint = center + viewDirection * viewDistance;
//        pViewer->getCameraManipulator()->setHomePosition(eyePoint, center, upDirection);
//        pViewer->home();
//        // ====================================================================================================
//
//
//
//        osg::ref_ptr<osg::Switch> sub_gp = new osg::Switch; // store clouds as sub-group inside gp
//        osg::ref_ptr<osg::PagedLOD> lod = new osg::PagedLOD;
//
//        //auto bd = geode1->getBound();
//        //auto c = bd.center();
//        //auto r = bd.radius();
//        //lod->setCenter(c);
//        //lod->setRadius(r);
//
//        lod->addChild(geode1, FLT_MIN, 300.0);
//        lod->addChild(geode11, 300.0, 600.0);
//        lod->addChild(geode111, 600.0, FLT_MAX);
//
//
//
//        osg::ref_ptr<osg::Switch> lod_gp = new osg::Switch; // store clouds as sub-group inside gp
//
//        osg::ref_ptr<osg::PagedLOD> lod1 = new osg::PagedLOD;
//        lod1->addChild(geode0, FLT_MIN, FLT_MAX);
//
//
//        lod_gp->addChild(lod, 1);
//        lod_gp->addChild(lod1, 0);
//
//
//        // lod ================
//        current_lod_gp = lod_gp;
//        // ====================
//
//
//
//        sub_gp->addChild(lod_gp);
//
//        gp->addChild(sub_gp);
//        gp_all->addChild(gp);
//
//
//
//        osgUtil::Optimizer optimizer;
//        optimizer.optimize(gp);
//        
//        file = filepath;
//        checkCloudFile = false;
//        canShowName = true;
//
//
//
//
//        // set cloud's property ================================================
//        std::string str = filepath.toStdString();
//        str = str.substr(str.find_last_of("\\")+1);
//        QString x = QString::fromStdString(str);
//        cloudProperty->setProperty(x);
//        cloudProperty->setProperty("points", cloud123->points.size());
//        cloudsData.push_back(cloudProperty);
//        //======================================================================
//        
//        // store colors===========================
//        osgcolor->storeColors(colors1);
//        osgcolor->storeIntensity(intensity_values);
//        osgcolor->storeNoColor(colors_none);
//
//        pcl::PointXYZRGB min_pt, max_pt;
//        pcl::getMinMax3D(*cloud, min_pt, max_pt);
//        osgcolor->storeMinMax(min_pt, max_pt);
//        //========================================
//
//    }
//    //return geode1;
//}



