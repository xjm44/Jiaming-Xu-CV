#include "../include/gui.h"
#include <QFuture>


gui::gui(QWidget* parent)
    : QFrame(parent)

{
    ui.setupUi(this);

    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    this->setWindowTitle(QString("company"));
    this->setWindowIcon(QIcon("../resources/images/new_logo.png"));

    auto handle = CreateSemaphoreA(NULL, 4, 4, "company"); // for the processing of the bag

    // starting
    setUpProUi();
    ui.btnProcess->click(); // first page is shown all the time
    bringFront();
    readHistoricalData();
    
    connect(&dlg, SIGNAL(enter_signal()), this, SLOT(displayMainwindow())); // from login page (activated successfully)
    connect(globalzzenter, SIGNAL(timer_signal(QString)), this, SLOT(slot_timeError(QString)));
    setAcceptDrops(true); // drag file

    initLoading(); // loading notification

    //lodHandler = new osgLodHandler(osgView);
}


//========================================================================= 3D View ===================================================================================================================
void gui::initOSG() 
{
    osg::ref_ptr<osg::Switch> gp = osgView->gp; // get the group which contains groups of clouds
    osgView->pViewer = osgWidget->getOsgViewer(); // set the view

    if (gp->getNumChildren() == 0) { // load everything beside point cloud(s)

        osgView->pViewer->setCameraManipulator(new osgGA::TrackballManipulator);
        ((osgGA::StandardManipulator*)osgView->pViewer->getCameraManipulator())->setAllowThrow(false); // left mouse move
        ((osgGA::OrbitManipulator*)osgView->pViewer->getCameraManipulator())->setWheelZoomFactor(-0.1);   // scroll up = zoom in
        //osgGA::TrackballManipulator* tm = new osgGA::TrackballManipulator;
        //tm->setAllowThrow(false);
        //tm->setWheelZoomFactor(-0.1);
        //osgView->pViewer->setCameraManipulator(tm);  


        osgView->pViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        osgView->pViewer->getCamera()->setClearColor(osgView->setColor(ui.sliderR->value(), ui.sliderG->value(), ui.sliderB->value()));

        osgView->gp_all->addChild(osgView->gp);
        osgView->gp_all->addChild(osgView->gp_temp);

        osgView->gp_all->addChild(osgView->makeCoordinate(osgView->pViewer,500,500)); // add coordinate
        ui.btnTarget->click();// add center x
        osgView->pViewer->setSceneData(osgView->gp_all);

        
    }
    else {

        if (osgView->canShowName == true) { // successfully loaded the point cloud

            QString temp = QString::number(osgView->count);
            ui.treeWidget1->setSelectionMode(QAbstractItemView::ExtendedSelection);
            QTreeWidgetItem* cloudTitle = new QTreeWidgetItem(ui.treeWidget1, QStringList("Cloud " + temp)); // add name (parent)
            cloudTitle->setFlags(cloudTitle->flags() & ~Qt::ItemIsSelectable);
            add_widgettree(cloudTitle, osgView->file); // add name (child)
            
            osgView->canShowName = false;
            osgView->count++;
            ui.btn3dview->click();
            osgView->pViewer->setSceneData(osgView->gp_all);

            stopLoading();

            // osg view handler
            osgView->pViewer->addEventHandler(lodHandler);
            lodHandler->setCurrentLod(osgView->current_lod_gp);

        }
        else {
            osgView->pViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            osgView->pViewer->getCamera()->setClearColor(osgView->setColor(ui.sliderR->value(), ui.sliderG->value(), ui.sliderB->value()));
        }
    } 
}

void gui::on_setBg_changed() {
    osgView->pViewer = osgWidget->getOsgViewer();
    osgView->pViewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    osgView->pViewer->getCamera()->setClearColor(osgView->setColor(ui.sliderR->value(), ui.sliderG->value(), ui.sliderB->value()));

}


void gui::on_selectCloud_clicked(QTreeWidgetItem* a, int b) { 

    QString name_temp = a->text(0);
    QStringList list = name_temp.split(" ");
    if (list.first() == "Cloud") { // when "Cloud x" is clicked
        cout << "cloud \n";
        ui.treeWidget1->clearSelection();
        osgView->cloudItem = NULL;
        de_select_cloud();
        ui.lastItemClicked->setText(QString(""));
        return;
    }


    if (a->parent() != NULL) {
       
        if (prev_cloud!=NULL && a->data(b,b).toString() == prev_cloud->data(b, b).toString() && a->parent() == prev_cloud->parent()) { // same cloud selected
            if (a->checkState(b) != Qt::Checked) {
                osg::ref_ptr<osg::Switch> gp = osgView->gp;
                osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

                //=================================== Get Cloud Position
                QTreeWidgetItem* p = a->parent();
                int cur_index = p->indexOfChild(a);
                int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
                //===================================

                sub_gp->setValue(cur_index, false);

                //ui.treeWidget1->clearSelection();
                //a->setSelected(true);
                osgView->remove_points();
                ui.cloudCoor->clear();
                

            }
            else if (a->checkState(b) != Qt::Unchecked) {

                osg::ref_ptr<osg::Switch> gp = osgView->gp;
                osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

                //=================================== Get Cloud Position
                QTreeWidgetItem* p = a->parent();
                int cur_index = p->indexOfChild(a);
                int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
                //===================================

                sub_gp->setValue(cur_index, true);

                //ui.treeWidget1->clearSelection();
                //a->setSelected(true);

            }
        }
        else {

            osgView->cloudItem = a;

            osgView->current_clouddata_pos = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());

            show_select_cloud();

            if (a->checkState(b) != Qt::Checked) {

                osg::ref_ptr<osg::Switch> gp = osgView->gp;
                osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

                //=================================== Get Cloud Position
                QTreeWidgetItem* p = a->parent();
                int cur_index = p->indexOfChild(a);
                int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
                //===================================

                sub_gp->setValue(cur_index, false);

                //ui.treeWidget1->clearSelection();
                //a->setSelected(true);

            }
            else if (a->checkState(b) != Qt::Unchecked) {

                osg::ref_ptr<osg::Switch> gp = osgView->gp;
                osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

                //=================================== Get Cloud Position
                QTreeWidgetItem* p = a->parent();
                int cur_index = p->indexOfChild(a);
                int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
                //===================================

                sub_gp->setValue(cur_index, true);

                //ui.treeWidget1->clearSelection();
                //a->setSelected(true);

            }
            
            showCloudProperties();
            prev_cloud = a;
            
            ////=================================== trial ============
            //QTreeWidgetItem* p = osgView->cloudItem->parent();
            //int cur_index = p->indexOfChild(osgView->cloudItem);
            //int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());

            //auto clouds = osgView->all_clouds.begin();
            //std::advance(clouds, parent_index);
            //cout << "parent: " + QString::number(parent_index).toStdString() + " \n ";

            //osgView->temp_cloud = *clouds;

            //cout << "current: pc " + QString::number(osgView->temp_cloud->points.size()).toStdString() + " \n ";
            //cout << "all: pc " + QString::number(osgView->all_clouds.size()).toStdString() + " \n ";

            //cout << sub_gp->getChild(cur_index)->getName()+"___";

            //cout << name_temp.toStdString() + "    name \n";


            //osg::ref_ptr<osg::Switch> gp = osgView->gp;
            //osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(parent_index)->asSwitch();
            //osgView->current_lod_gp = sub_gp->getChild(0)->asSwitch();

            //if (aa1 == 0) {
            //    sub_gp->getChild(0)->asSwitch()->setValue(0, false);
            //    sub_gp->getChild(0)->asSwitch()->setValue(1, true);
            //    aa1 = 1;
            //}
            //else if (aa1 == 1) {
            //    sub_gp->getChild(0)->asSwitch()->setValue(0, true);
            //    sub_gp->getChild(0)->asSwitch()->setValue(1, false);
            //    aa1 = 0;
            //}
            
            //osg::PagedLOD* geode1 = (osg::PagedLOD*)sub_gp->getChild(cur_index); // get geode only
            //cout << geode1->getChild(2)->getName();


            //osg::Node* geode11 = osgDB::readNodeFile("cow.osg");
            //geode1->addChild((osg::Geode*)geode11, FLT_MIN, FLT_MAX);

        }

        
        ui.lastItemClicked->setText(QString(" Last Item Selected: ") + a->text(b));
    }
    else {
        osgView->cloudItem = NULL;
        de_select_cloud();
        ui.lastItemClicked->setText(QString(""));
    } 
}


void gui::showCloudProperties() {                                       // ======================TO BE DONE: only show first cloud properties currently=================          

    auto cloudPos = osgView->cloudsData.begin();
    std::advance(cloudPos, osgView->current_clouddata_pos); // get parent cloud index

    ui.cloudName->setText((*cloudPos)->getPropertyQString("name"));
    long long int tempP = (*cloudPos)->getPropertyFloat("points"); // to remove exponential
    ui.cloudPoints->setText(QString::number(tempP));
    ui.cloudName->setAlignment(Qt::AlignCenter);
    ui.cloudPoints->setAlignment(Qt::AlignCenter);

    // clear previous properties ============================================
    if (ui.cloudCoor->toPlainText() != NULL) {          
        ui.cloudCoor->clear();
        ui.cloudDist->clear();
        ui.btnPoint->setCheckable(false);
        osgView->pViewer->getEventHandlers().clear();
    }
    else if (ui.cloudCoor->toPlainText() == NULL && ui.btnPoint->isCheckable()) {
        ui.btnPoint->setCheckable(false);
        osgView->pViewer->getEventHandlers().clear();
    }

    osgView->remove_points();


    if (ui.btnPhoto->isCheckable()) {
        ui.btnPhoto->setCheckable(false);
        osgView->pViewer->getEventHandlers().clear();
    }

    (*cloudPos)->distance_res = 0.0;
    (*cloudPos)->first_point = false;

}


void gui::on_btndelCloud_clicked() {
  
    ui.btnDel->setChecked(true);

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(NULL, "Confirmation", "Remove this point cloud?",
        QMessageBox::Yes | QMessageBox::No);
        
    if (confirm == QMessageBox::Yes) {
        osg::ref_ptr<osg::Switch> gp = osgView->gp;
        osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

        //=================================== Get Cloud Position
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //===================================


        //==========================    TO BE DONE (lazy way to delete a mesh obj)=========
        bool check_mesh = false;
        bool check_plane = false;
        osg::Geode* geode1 = (osg::Geode*)sub_gp->getChild(cur_index); // get geode only

        if (geode1->getName() == "mesh") {
            check_mesh = true;
            cout << "meshed \n";
        }
        else if (geode1->getName() == "moved") {     // can actually check movedgeode only (delete all moved unoriginal geode)
            std::string x = "movedgeode";            // weird case: moved mesh cant recognise movedgeodemesh name
            finder = new findNode(x);
            //sub_gp->accept(*finder);
            finder->apply(*sub_gp->getChild(cur_index)->asNode());
            if (!finder->getNode()) {
                check_mesh = true;
                cout << "moved geode mesh \n";
            }

            std::string x1 = "movedgeodeextracted";   // lazy way to delete an extracted plane
            finder = new findNode(x1);
            //sub_gp->accept(*finder);
            finder->apply(*sub_gp->getChild(cur_index)->asNode());
            if (finder->getNode()) {
                check_plane = true;
                cout << "moved extracted \n";
            }

            //std::string x2 = "movedgeodemesh";
            //finder = new findNode(x2);
            ////sub_gp->accept(*finder);
            //finder->apply(*sub_gp->getChild(cur_index)->asNode());
            //if (finder->getNode()) {
            //    check_mesh = true;
            //    cout << "moved geode mesh \n";
            //}

        }else if (geode1->getName() == "extracted") { // lazy way to delete an extracted plane
            check_plane = true;
            cout << "extracted \n";
        }
        //=================================================================================



        sub_gp->removeChild(cur_index, 1); // remove from sub-group 
        delete osgView->cloudItem; 
        if (sub_gp->getNumChildren() == 0) {
            gp->removeChild(parent_index,1);
            ui.treeWidget1->takeTopLevelItem(parent_index); // remove from treewidget
        }
        de_select_cloud();

        if (check_mesh || check_plane) {
            cout << "meshed____OR____extracted \n";
        }
        else {
            //====================================================== TO BE DONE: removed all lists (clouddata,colors) ===================
            // 
            // clear properties======================
            auto cloudPos = osgView->cloudsData.begin();
            std::advance(cloudPos, parent_index);
            osgView->cloudsData.erase(cloudPos);
            //========================================

            osgView->osgcolor->remove_colors(parent_index); // delete colors=========

            //============================================================================================================================


            // clear mesh&trimmed====================== TO BE DONE ==============================only parent index
            auto files = osgView->files.begin();
            std::advance(files, parent_index);
            osgView->files.erase(files);

            auto clouds = osgView->all_clouds.begin();
            std::advance(clouds, parent_index);
            osgView->all_clouds.erase(clouds);
            //========================================

            cout << "not meshed \n";

            cout << osgView->files.size();
            cout << " \n ";
            cout << osgView->all_clouds.size();
            cout << " \n ";
        }

        ui.treeWidget1->clearSelection();
    }
}

void gui::slot_pointsize(QString r) {

    if ((ui.treeWidget1->topLevelItemCount() == 0 || osgView->cloudItem == NULL || prev_cloud == NULL)) {

        QMessageBox::information(NULL, "Error", "No point cloud is selected!");
    }
    else {
        // loading notification ============================
        startLoading();
        //==================================================

        //=================================== Get Cloud Position
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //===================================
        osg::ref_ptr<osg::Switch> gp = osgView->gp;
        osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

        osg::StateSet* ss = sub_gp->getChild(cur_index)->getOrCreateStateSet();
        ss->setAttribute(new osg::Point(r.toFloat()), osg::StateAttribute::ON);

        stopLoading();
    }

}

void gui::slot_color(int i) {

    current_color = i; // besides RGB and None

    if ((ui.treeWidget1->topLevelItemCount() == 0 || osgView->cloudItem == NULL || prev_cloud == NULL)) {

        QMessageBox::information(NULL, "Error", "No point cloud is selected!");
    }
    else {
        
        //=================================== Get Cloud Position
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //===================================
        osg::ref_ptr<osg::Switch> gp = osgView->gp;
        osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

        //QMessageBox::about(NULL,"A","A");
        //cloudindex = 0;
        //osg::PagedLOD* lod = (osg::PagedLOD*)(sub_gp->getChild(cur_index));
        //osg::Geode* geode = (osg::Geode*)(lod->getChild(cur_index));
        //QMessageBox::about(NULL, "A", "A11");


        osg::Geode* geode;
        if (sub_gp->getChild(cur_index)->getName() == "unmoved") {
            geode = (osg::Geode*)sub_gp->getChild(cur_index);
        }
        else if (sub_gp->getChild(cur_index)->getName() == "moved") {
            //cout << "moved";

            std::string x = "movedgeode";
            finder = new findNode(x);
            //sub_gp->accept(*finder);
            finder->apply(*sub_gp->getChild(cur_index)->asNode());
            //if (finder->getNode() != NULL) { cout << "hi___________"; };
            geode = finder->getNode();
            
            if (!geode) {    // not a point cloud being moved
                cout << "meshed";
                return;
            }
            
        }else if (sub_gp->getChild(cur_index)->getName() == "mesh") {
            cout << "meshed";
            return;
        }
        else { // temporary
            geode = (osg::Geode*)sub_gp->getChild(cur_index);
        }
       /* cout << sub_gp->getChild(cur_index)->getCompoundClassName()+"\n";
        cout << (((osg::Group*)sub_gp->getChild(cur_index))->getChild(0))->getCompoundClassName() + "\n";
        cout << ((osg::Group*)((osg::MatrixTransform*)sub_gp->getChild(cur_index))->getChild(0))->getChild(0)->getCompoundClassName() + "\n";
        cout << ((osg::MatrixTransform*)((osg::Group*)((osg::MatrixTransform*)sub_gp->getChild(cur_index))->getChild(0))->getChild(0))->getChild(0)->getCompoundClassName() + "\n";*/
      
        osg::Geometry* geometry = geode->getDrawable(0)->asGeometry();

        // ========================================== TO BE DONE: currently, only index 0 (parent) color is stored in the list ===================
        if (i == 0) {
            // loading notification ============================
            startLoading();
            //==================================================

            geometry->setColorArray(osgView->osgcolor->getColor(parent_index, true).get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        else if (i == 5) {
            // loading notification ============================
            startLoading();
            //==================================================

            geometry->setColorArray(osgView->osgcolor->getColor(parent_index, false).get());
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        }
        //=========================================================================================================================================

        stopLoading();
    }
}

void gui::slot_colormap(int i) {

    if ((ui.treeWidget1->topLevelItemCount() == 0 || osgView->cloudItem == NULL || prev_cloud == NULL)) {

        QMessageBox::information(NULL, "Error", "No point cloud is selected!");
    }
    else {
        // loading notification ============================
        startLoading();
        //==================================================

        //=================================== Get Cloud Position
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //===================================
        osg::ref_ptr<osg::Switch> gp = osgView->gp;
        osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

        if (sub_gp->getChild(cur_index)->getName() == "unmoved") {
            osgView->show_color(current_color, parent_index, cur_index, static_cast<ColorMap>(i), QString("unmoved"));
        }
        else if (sub_gp->getChild(cur_index)->getName() == "moved") {
            osgView->show_color(current_color, parent_index, cur_index, static_cast<ColorMap>(i), QString("moved"));
        }
        else { // temporary
            osgView->show_color(current_color, parent_index, cur_index, static_cast<ColorMap>(i), QString("temp"));
        }

        stopLoading();
    }
}


void gui::on_mesh_clicked()
{

    if ((ui.treeWidget1->topLevelItemCount() == 0 || osgView->cloudItem == NULL || prev_cloud == NULL)) {

        QMessageBox::information(NULL, "Error", "No point cloud is selected!");
    }
    else {

        move_cloud(false);

        ui.btnMeClose->setVisible(true);
        ui.btnMeSs->setVisible(true);
        ui.btnMeAf->setVisible(true);
        ui.btnMePoss->setVisible(true);
        ui.btnMeClose->raise();
        ui.btnMeSs->raise();
        ui.btnMeAf->raise();
        ui.btnMePoss->raise();


        ui.btnMeSs->setEnabled(true);
        ui.btnMeAf->setEnabled(true);
        ui.btnMePoss->setEnabled(true);

        ui.btnMesh->setCheckable(true);
        ui.btnMesh->setChecked(true);
    }
}

void gui::on_meclose_clicked() {
    ui.btnMeSs->setCheckable(false);
    ui.btnMeSs->setEnabled(false);
    ui.btnMeAf->setEnabled(false);
    ui.btnMeAf->setCheckable(false);
    ui.btnMePoss->setEnabled(false);
    ui.btnMePoss->setCheckable(false);

    ui.btnMeClose->hide();
    ui.btnMeSs->hide();
    ui.btnMeAf->hide();
    ui.btnMePoss->hide();

    mesh_index = -999;

    ui.treeWidget1->clearSelection();
    move_cloud(true);
    de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 

}

void gui::on_meposs_clicked() {

    if (ui.btnMePoss->isCheckable()) {
    }
    else {

        ui.btnMePoss->setEnabled(false);
        ui.btnMeAf->setEnabled(true);
        ui.btnMeSs->setEnabled(true);

        mesh_index = 0;
        cout << "Po \n";
        to_mesh(mesh_index);
    }
}
void gui::on_meaf_clicked() {

    if (ui.btnMeAf->isCheckable()) {
    }
    else {

        ui.btnMeAf->setEnabled(false);
        ui.btnMePoss->setEnabled(true);
        ui.btnMeSs->setEnabled(true);

        mesh_index = 1;
        cout << "Af \n";
        to_mesh(mesh_index);
    }
}
void gui::on_mess_clicked() {

    if (ui.btnMeSs->isCheckable()) {
    }
    else {

        ui.btnMeSs->setEnabled(false);
        ui.btnMePoss->setEnabled(true);
        ui.btnMeAf->setEnabled(true);

        mesh_index = 2;
        cout << "Ss \n";

        to_mesh(mesh_index);
    }
}

void gui::to_mesh(int index) {

    // set correct path ====================================================
    QTreeWidgetItem* p = osgView->cloudItem->parent();
    int cur_index = p->indexOfChild(osgView->cloudItem);
    int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());

    // pcl file vector ===========================
    auto files = osgView->files.begin();
    std::advance(files, parent_index);
    QDir d = QFileInfo(*files).absoluteDir();
    QString abspath = d.absolutePath();
    QFileDialog fileDialog;
    QString curpath = fileDialog.getExistingDirectory(this, tr("Choose a folder (make sure only English dir path is allowed)"), abspath);
    if (curpath.isEmpty())
    {
        // The user pressed the cancel button so handle this accordingly
        cout << "cancelled \n";
    }
    else {

        // At least one file was selected because the user cannot click the 'open' button unless a file selection has been made so continue as normal
        QString temp("\\mesh%1.obj"); // set name
        temp = curpath + temp.arg(osgView->mesh_count);
        osgView->mesh_count++;
        curpath = temp;
        abspath = curpath;

        // loading notification ============================ 
        startLoading();
        //==================================================

        // pcl clouds vector =========================== TO BE DONE (only parent index now) ==========
        auto clouds = osgView->all_clouds.begin();
        std::advance(clouds, parent_index);
        //============================================================================================

        Point_set cgal_cloud = osgView->osgAlgo->pcl2cgal(*clouds);
        osgView->osgAlgo->mesh(cgal_cloud, index, abspath.toStdString());


        abspath = abspath.replace('/', '\\');
        add_widgettree(p, abspath);// add a new geode into the widget tree ========== 


        osg::ref_ptr<osg::Switch> gp = osgView->gp;
        osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(parent_index)->asSwitch();
        osg::ref_ptr<osg::Geode> geode1 = (osg::Geode*)osgDB::readNodeFile(abspath.toStdString());
        geode1->setName("mesh");
        //osg::StateSet* ss = geode1->getOrCreateStateSet();
        //ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        sub_gp->addChild(geode1);

        // =========================== distance increase a bit further =================================
        osg::Vec3d viewDirection;
        osg::Vec3d upDirection;
        viewDirection.set(0.0, -1.0, 0.0);
        upDirection.set(0.0, 1.0, 0.0);
        get_pov(viewDirection, upDirection, geode1, 5.5);
        // ====================================================================================================

        stopLoading();
    }
}




void gui::on_rnoise_clicked()
{
    if (ui.btnRNoise->isChecked()) {

        ui.btnRClose->setVisible(true);
        ui.btnRPSave->setVisible(true);
        ui.btnRPlane->setVisible(true);
        ui.btnRTrim->setVisible(true);
        ui.btnRClose->raise();
        ui.btnRPSave->raise();
        ui.btnRPlane->raise();
        ui.btnRTrim->raise();

        ui.btnRClose->setEnabled(true);
        ui.btnRPlane->setEnabled(true);


        // plane color
        ui.planeR->setVisible(true);
        ui.planeG->setVisible(true);
        ui.planeB->setVisible(true);
        ui.planeColour->setVisible(true);
        ui.planeR->raise();
        ui.planeG->raise();
        ui.planeB->raise();
        ui.planeColour->raise();


        // set correct path ====================================
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //======================================================
        auto clouds = osgView->all_clouds.begin();
        std::advance(clouds, parent_index);
        osgView->temp_cloud = *clouds;

    }
    move_cloud(false);
    ui.btnRNoise->setCheckable(true);
    ui.btnRNoise->setChecked(true);
}

void gui::on_rclose_clicked() {
    

    if (check_plane_extraction) {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::question(NULL, "Confirmation", "Abortion of the selected plane(s)?",
            QMessageBox::Yes | QMessageBox::No);

        if (confirm == QMessageBox::Yes) {
            //cout << "plane extraction done \n";
            check_plane_extraction = false;

            ui.btnRPSave->setCheckable(false);
            ui.btnRPSave->setEnabled(false);
            ui.btnRPlane->setEnabled(false);
            ui.btnRPlane->setCheckable(false);
            ui.btnRTrim->setEnabled(false);
            ui.btnRTrim->setCheckable(false);

            ui.btnRClose->hide();
            ui.btnRPSave->hide();
            ui.btnRPlane->hide();
            ui.btnRTrim->hide();

            ui.planeR->hide();
            ui.planeG->hide();
            ui.planeB->hide();
            ui.planeColour->hide();


            ui.treeWidget1->clearSelection();
            move_cloud(true);
            de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 


            // remove 'orange' planes
            for (int i = osgView->gp_temp->getNumChildren() - 1; i >= 0; i--) {
                osgView->gp_temp->removeChild(i);
            }

            // empty all stored planes
            osgView->all_plane.clear();
            vector<int>().swap(osgView->all_plane);

            osgView->trim_count = 0; // if only 2 pts for the cut 

            // empty green balls from cut
            osgView->x.clear();
            vector<float>().swap(osgView->x);
            osgView->y.clear();
            vector<float>().swap(osgView->y);
            osgView->z.clear();
            vector<float>().swap(osgView->z);
            osgView->remove_points();


            osgView->pViewer->getEventHandlers().clear();
        }
        else { 
        }

    }
    else {
        ui.btnRPSave->setCheckable(false);
        ui.btnRPSave->setEnabled(false);
        ui.btnRPlane->setEnabled(false);
        ui.btnRPlane->setCheckable(false);
        ui.btnRTrim->setEnabled(false);
        ui.btnRTrim->setCheckable(false);

        ui.btnRClose->hide();
        ui.btnRPSave->hide();
        ui.btnRPlane->hide();
        ui.btnRTrim->hide();

        ui.planeR->hide();
        ui.planeG->hide();
        ui.planeB->hide();
        ui.planeColour->hide();


        ui.treeWidget1->clearSelection();
        move_cloud(true);
        de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud()

        // remove 'orange' planes
        for (int i = osgView->gp_temp->getNumChildren() - 1; i >= 0; i--) {
            osgView->gp_temp->removeChild(i);
        }

        // empty all stored planes
        osgView->all_plane.clear();
        vector<int>().swap(osgView->all_plane);


        // empty green balls from cut
        osgView->x.clear();
        vector<float>().swap(osgView->x);
        osgView->y.clear();
        vector<float>().swap(osgView->y);
        osgView->z.clear();
        vector<float>().swap(osgView->z);
        osgView->remove_points();

        osgView->pViewer->getEventHandlers().clear();
    }

}

void gui::on_rplane_clicked() {

    if (ui.btnRPlane->isCheckable()) {
    }
    else {
        osgView->pViewer->getEventHandlers().clear();

        ui.btnRPlane->setEnabled(false);

        osgView->pViewer->addEventHandler(new planeExtraction(osgView));

        // only one sub btn per time   
        ui.btnRPSave->setCheckable(false);
        ui.btnRPSave->setEnabled(false);
        ui.btnRTrim->setCheckable(false);
        ui.btnRTrim->setEnabled(false);
    }

}

void gui::on_rtrim_clicked() {

    if (ui.btnRTrim->isCheckable()) {

        ui.btnRTrim->setCheckable(false);
        ui.btnRPSave->setEnabled(true);

        bool check_angle = false;

        PointCloud<PointXYZRGB>::Ptr duobianxing(new PointCloud<PointXYZRGB>());

        for (int i = 0; i < osgView->x.size(); i++)
        {
            if (osgView->x[i] == -999 && osgView->y[i] == -999 && osgView->z[i] == -999) {
                check_angle = true;
                // empty green balls from cut
                osgView->x.clear();
                vector<float>().swap(osgView->x);
                osgView->y.clear();
                vector<float>().swap(osgView->y);
                osgView->z.clear();
                vector<float>().swap(osgView->z);
                osgView->remove_points();
                break;
            }
            else {
                duobianxing->push_back(pcl::PointXYZRGB(osgView->x[i], osgView->y[i], osgView->z[i], 0, 0, 0));
            }
        }

        if (check_angle) {

            QMessageBox::warning(NULL, "Warning", "Please choose the 6 fixed angle only.");
            check_angle = false;
        }
        else {


            // set correct path ====================================
            QTreeWidgetItem* p = osgView->cloudItem->parent();
            int cur_index = p->indexOfChild(osgView->cloudItem);
            int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
            //======================================================

            // pcl clouds vector =========================== TO BE DONE (only parent index now) ==========
            auto clouds = osgView->all_clouds.begin();
            std::advance(clouds, parent_index);
            //============================================================================================


            osgView->temp_cloud = osgView->osgAlgo->manualpolygontool(osgView->temp_cloud, duobianxing, 0); 

            cout << "finished cutting \n";

            check_plane_extraction = true;
        }

        osgView->trim_count = 0;
    }
    else {
        osgView->pViewer->getEventHandlers().clear(); // remove plane extraction
        osgView->x.clear();
        vector<float>().swap(osgView->x);
        osgView->y.clear();
        vector<float>().swap(osgView->y);
        osgView->z.clear();
        vector<float>().swap(osgView->z);
        cout << "stop extraction, start cutting \n";

        osgView->pViewer->addEventHandler(new osgTrim(osgView));

        ui.btnRPlane->setCheckable(false);
        ui.btnRPlane->setEnabled(false);
        ui.btnRPSave->setCheckable(false);
        ui.btnRPSave->setEnabled(false);
        ui.btnRTrim->setEnabled(false);
    }
}

void gui::on_rplanesave_clicked() {

    if (check_plane_extraction) {
        cout << "saved \n";

        // loading notification ============================ 
        startLoading();
        //==================================================

        // set correct path ====================================
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //======================================================

        // pcl clouds vector =========================== TO BE DONE (only parent index now) ==========
        auto clouds = osgView->all_clouds.begin();
        std::advance(clouds, parent_index);
        //============================================================================================

  
        // ================================================== extracted planes ==================================================
        vector<int> temp = osgView->all_plane; // selected planes
        std::vector<int>::iterator iter;
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr tempcloud(new pcl::PointCloud<pcl::PointXYZRGB>); // to be merged
        for (iter = temp.begin(); iter != temp.end(); iter++)
        {
            int cur = *iter;

            // RGB==================================================================
            uint32_t rgb_val_1;
            memcpy(&rgb_val_1, &(clouds->get()->points[cur].rgb), sizeof(uint32_t));
            uint32_t red, green, blue;
            blue = rgb_val_1 & 0x000000ff;
            rgb_val_1 = rgb_val_1 >> 8;
            green = rgb_val_1 & 0x000000ff;
            rgb_val_1 = rgb_val_1 >> 8;
            red = rgb_val_1 & 0x000000ff;

            tempcloud->push_back(pcl::PointXYZRGB(clouds->get()->points[cur].x, clouds->get()->points[cur].y, clouds->get()->points[cur].z, clouds->get()->points[cur].r, clouds->get()->points[cur].g, clouds->get()->points[cur].b));
        }
        //==========================================================================================================================

        // merge extracted planes with the trimmed spaces ====================
        tempcloud = osgView->osgAlgo->merge(tempcloud, osgView->temp_cloud);
        tempcloud = osgView->osgAlgo->voxel_downsampling(tempcloud, 0.01f);
        // ===================================================================

        
        osg::ref_ptr<osg::Switch> sub_gp = osgView->gp->getChild(parent_index)->asSwitch();
        sub_gp->addChild(osgView->createGeode(tempcloud,"extracted"));

        add_widgettree(p, QString("noise-reduced point cloud ") + QString::number(osgView->extract_plane_count)); // add a new geode into the widget tree ========== 

        stopLoading();


        check_plane_extraction = false;
        osgView->extract_plane_count++;

        // empty all stored planes
        osgView->all_plane.clear();
        vector<int>().swap(osgView->all_plane);


        // empty green balls from cut
        osgView->x.clear();
        vector<float>().swap(osgView->x);
        osgView->y.clear();
        vector<float>().swap(osgView->y);
        osgView->z.clear();
        vector<float>().swap(osgView->z);
        osgView->remove_points();

        ui.btnRPlane->setEnabled(true);
        osgView->pViewer->getEventHandlers().clear();

        cout << "Final points: " + QString::number(tempcloud->points.size()).toStdString() + " \n ";

    }
}

void gui::on_trim_clicked()
{

    //if (ui.btnTrim->isCheckable()) {

    //    osgView->pViewer->getEventHandlers().clear();

    //    // only one eventhandler per function
    //    ui.btnTrim->setChecked(false);
    //    ui.btnTrim->setCheckable(false);

    //    PointCloud<PointXYZRGB>::Ptr duobianxing(new PointCloud<PointXYZRGB>());


    //    for (int i = 0; i < osgView->x.size(); i++)
    //    {

    //        duobianxing->push_back(pcl::PointXYZRGB(osgView->x[i], osgView->y[i], osgView->z[i], 0, 0, 0));
    //    }

    //    PointCloud<PointXYZRGB>::Ptr cloud3(new PointCloud<PointXYZRGB>());


    //    // set correct path ====================================
    //    QTreeWidgetItem* p = osgView->cloudItem->parent();
    //    int cur_index = p->indexOfChild(osgView->cloudItem);
    //    int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
    //    //======================================================

    //    // pcl clouds vector =========================== TO BE DONE (only parent index now) ==========
    //    auto clouds = osgView->all_clouds.begin();
    //    std::advance(clouds, parent_index);
    //    //============================================================================================

    //    cloud3 = osgView->osgAlgo->manualpolygontool(*clouds, duobianxing, 0); 


    //    osg::ref_ptr<osg::Geode> geode1 = osg::ref_ptr<osg::Geode>(new osg::Geode());

    //    osg::ref_ptr<osg::Geometry> geometry1(new osg::Geometry());
    //    osg::ref_ptr<osg::Vec3Array> vertices1(new osg::Vec3Array());
    //    osg::ref_ptr<osg::Vec4Array> colors1(new osg::Vec4Array()); // rgb color



    //    for (int i = 0; i < cloud3->points.size(); i++) {

    //        vertices1->push_back(osg::Vec3(cloud3->points[i].x, cloud3->points[i].y, cloud3->points[i].z));

    //        // RGB============================================================================================
    //        uint32_t rgb_val_1;
    //        memcpy(&rgb_val_1, &(cloud3->points[i].rgb), sizeof(uint32_t));

    //        uint32_t red, green, blue;
    //        blue = rgb_val_1 & 0x000000ff;
    //        rgb_val_1 = rgb_val_1 >> 8;
    //        green = rgb_val_1 & 0x000000ff;
    //        rgb_val_1 = rgb_val_1 >> 8;
    //        red = rgb_val_1 & 0x000000ff;

    //        colors1->push_back(osg::Vec4f((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f));
    //        // ===============================================================================================
    //    }

    //    geometry1->setVertexArray(vertices1.get());
    //    geometry1->setColorArray(colors1.get());
    //    geometry1->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    //    geometry1->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    //    geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices1->size()));
    //    osgUtil::SmoothingVisitor smv;
    //    smv.smooth(*geometry1);

    //    osg::StateSet* ss = geometry1->getOrCreateStateSet();
    //    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    //    ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));

    //    geode1->addDrawable(geometry1.get());
    //    geode1->setName("extracted");

    //    osg::ref_ptr<osg::Switch> sub_gp = osgView->gp->getChild(parent_index)->asSwitch();
    //    sub_gp->addChild(geode1);

    //    // add a new geode into the widget tree ========== 
    //    QTreeWidgetItem* pCurrentItem = ui.treeWidget1->currentItem();
    //    QTreeWidgetItem* cloud = new QTreeWidgetItem(p, QStringList(QString("extracted point cloud ") + QString::number(osgView->extract_plane_count))); // get cloud name (path) (first child)
    //    cloud->setFlags(cloud->flags() | Qt::ItemIsUserCheckable);
    //    cloud->setCheckState(0, Qt::Checked);
    //    p->insertChild(p->childCount(), cloud);
    //    p->setExpanded(true);
    //    //=============================================


    //    osgView->x.clear();
    //    vector<float>().swap(osgView->x);
    //    osgView->y.clear();
    //    vector<float>().swap(osgView->y);
    //    osgView->z.clear();
    //    vector<float>().swap(osgView->z);

    //    osgView->remove_points();

    //}
    //else {

    //    

    //    // only one eventhandler per function

    //    ui.btnTrim->setCheckable(true);
    //    ui.btnTrim->setChecked(true);


    //    ////=================================== Get Cloud Position
    //    //QTreeWidgetItem* p = osgView->cloudItem->parent();
    //    //int cur_index = p->indexOfChild(osgView->cloudItem);
    //    //int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
    //    ////===================================


    //    //osg::ref_ptr<osg::Switch> gp = osgView->gp;
    //    //osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(parent_index)->asSwitch();

    //    //osg::Geode* geode1 = (osg::Geode*)sub_gp->getChild(cur_index); // get geode only

    //    //osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    //    //camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    //camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    //    //const osg::BoundingSphere bs = geode1->getBound();
    //    //double viewDist = 2.0 * bs.radius();
    //    //double zNear = viewDist - bs.radius();
    //    //double zFar = viewDist + bs.radius();
    //    //float top = bs.radius();
    //    //float right = bs.radius();
    //    //camera->setProjectionMatrixAsOrtho(-right, right, -top, top, zNear, zFar);
    //    ////camera->setProjectionMatrixAsOrtho2D(-right, right, -top, top);

    //    //osg::Vec3d up(0.0,1.0,0.0);
    //    //osg::Vec3d view(0.0, 0.0, 1.0);
    //    //osg::Vec3d center = bs.center();

    //    //osg::Vec3d eyePoint = center + view * viewDist;


    //    //osg::Vec3d eye, center1, up1;
    //    //osgView->pViewer->getCamera()->getViewMatrixAsLookAt(eye, center1, up1);
    //    //osg::Vec3d eyeForward = center1 - eye;


    //    //camera->setViewMatrixAsLookAt(eyeForward, center, eyeForward);
    //     
    //    //camera->addChild(geode1);

    //    //sub_gp->replaceChild(geode1, camera);

    //    osgView->pViewer->addEventHandler(new osgTrim(osgView));
    //}

}

void gui::on_move_clicked()
{
    if (ui.btnMove->isChecked()) {

        if ((ui.treeWidget1->topLevelItemCount() == 0 || osgView->cloudItem == NULL || prev_cloud == NULL)) {

            QMessageBox::information(NULL, "Error", "No point cloud is selected!");
        }
        else {

            move_cloud(false);
            ui.btnMClose->setVisible(true);
            ui.btnMClose->raise();
            ui.btnMove->setCheckable(true);
            ui.btnMove->setChecked(true);

            //=================================== Get Cloud Position
            QTreeWidgetItem* p = osgView->cloudItem->parent();
            int cur_index = p->indexOfChild(osgView->cloudItem);
            int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
            //===================================


            osg::ref_ptr<osg::Switch> gp = osgView->gp;
            osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(parent_index)->asSwitch();
            int current_pos_in_sub_grp = cur_index;


            osg::Geode* geode1 = (osg::Geode*)sub_gp->getChild(cur_index); // get geode only
            
            // apply selection + dagger
            osg::ref_ptr<osg::Group> temp_gp = new osg::Group;
            osg::ref_ptr<osgManipulator::Selection>  selection = new osgManipulator::Selection();//section MatrixTransform
            selection->addChild(geode1);
            temp_gp->addChild(selection);

            if (sub_gp->getChild(cur_index)->getName() == "unmoved") {
                geode1->setName("movedgeode");
            }else if (sub_gp->getChild(cur_index)->getName() == "mesh") {
                geode1->setName("movedgeodemesh");
                //cout << "meshed";
            }
            else if (sub_gp->getChild(cur_index)->getName() == "extracted") {
                geode1->setName("movedgeodeextracted");

            }

            osg::ref_ptr<osgManipulator::Dragger> dragger = new osgManipulator::TrackballDragger();
            float scale = geode1->getBound().radius();
            dragger->setupDefaultGeometry();
            dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) *
                osg::Matrix::translate(geode1->getBound().center()));
            dragger->addTransformUpdating(selection);
            dragger->setHandleEvents(true);
            temp_gp->addChild(dragger);


            osg::ref_ptr<osg::Group> temp_gp1 = new osg::Group;
            osg::ref_ptr<osgManipulator::Selection>  selection1 = new osgManipulator::Selection();//section MatrixTransform
            selection1->addChild(temp_gp);
            temp_gp1->addChild(selection1);

            osg::ref_ptr<osgManipulator::Dragger> dragger1 = new osgManipulator::TranslateAxisDragger();
            float scale1 = temp_gp->getBound().radius()*0.9;
            dragger1->setupDefaultGeometry();
            dragger1->setMatrix(osg::Matrix::scale(scale1, scale1, scale1) *
                osg::Matrix::translate(temp_gp->getBound().center()));
            dragger1->addTransformUpdating(selection1);
            dragger1->setHandleEvents(true);
            temp_gp1->addChild(dragger1);
            // ==============================================================


            sub_gp->removeChild(geode1); // remove original geode                    
            sub_gp->insertChild(cur_index, temp_gp1); // add a group to sub_gp


            // =========================== distance increase a bit further =================================
            osg::Vec3d viewDirection;
            osg::Vec3d upDirection;
            viewDirection.set(0.0, -1.0, 0.0);
            upDirection.set(0.0, 1.0, 0.0);
            get_pov(viewDirection, upDirection, geode1, 5.5);
            // ====================================================================================================

        }
    }
    else {
    }
}

void gui::on_mclose_clicked() {
    //=================================== Get Cloud Position
    QTreeWidgetItem* p = osgView->cloudItem->parent();
    int cur_index = p->indexOfChild(osgView->cloudItem);
    int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
    //===================================
   
    osg::ref_ptr<osg::Switch> gp = osgView->gp;
    osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(parent_index)->asSwitch(); // sub_grp
    osg::ref_ptr<osg::Group> temp_gp = (osg::Group*)sub_gp->getChild(cur_index); // selection(group of selection and dragger) and dragger inside



    temp_gp->removeChild(1); // xyz draggger

    osg::ref_ptr<osgManipulator::Selection>  first_child_from_temp_gp = (osgManipulator::Selection*)temp_gp->getChild(0);
    //osg::ref_ptr<osg::Geode> child_from_first_child_from_temp_gp = (osg::Geode*)((osgManipulator::Selection*)((osg::Group*)first_child_from_temp_gp->getChild(0))->getChild(0))->getChild(0);
    
    ((osg::Group*)first_child_from_temp_gp->getChild(0))->removeChild(1); // rotation dragger
    
    first_child_from_temp_gp->setName("moved");
    //cout << first_child_from_temp_gp->getNumChildren();
    sub_gp->replaceChild(temp_gp, first_child_from_temp_gp);


    ui.btnMClose->hide();
    ui.treeWidget1->clearSelection();
    move_cloud(true);
    de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 

}

void gui::on_target_clicked() { // add to the end of gp_all

    if (ui.btnTarget->isChecked()) {
        osgView->gp_all->insertChild(osgView->gp_all->getNumChildren(), osgView->makeCenterX());
    }
    else {
        osgView->gp_all->removeChild(osgView->gp_all->getNumChildren()-1, 1);
    }

}


void gui::on_btnView1Cloud_clicked()
{
    different_view(1);
}
void gui::on_btnView2Cloud_clicked()
{
    different_view(2);
}
void gui::on_btnView3Cloud_clicked()
{
    different_view(3);
}
void gui::on_btnView4Cloud_clicked()
{
    different_view(4);
}
void gui::on_btnView5Cloud_clicked()
{
    different_view(5);
}
void gui::on_btnView6Cloud_clicked()
{
    different_view(6);
}

void gui::on_point_clicked()
{
    if (ui.btnPoint->isCheckable()) {
        ui.btnPoint->setChecked(false);
        ui.btnPoint->setCheckable(false);
        osgView->pViewer->getEventHandlers().clear();

        // ========================================== TO BE DONE: currently, only index 0 (parent)  of cloudsdata is stored in the list =================
        auto cloudPos = osgView->cloudsData.begin();
        std::advance(cloudPos, osgView->current_clouddata_pos);
        (*cloudPos)->distance_res = 0.0;
        (*cloudPos)->first_point = false;
        // ==============================================================================================================================================

        osgView->remove_points(); 
        ui.cloudCoor->clear();
        ui.cloudDist->clear();

        // only one eventhandler per function
        ui.btnPhoto->setEnabled(true);

        ui.treeWidget1->clearSelection();
        move_cloud(true);
        de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 

    }
    else {
        osgView->pViewer->getEventHandlers().clear();

        move_cloud(false);
        ui.btnPoint->setEnabled(true);
        ui.btnPoint->setCheckable(true);
        ui.btnPoint->setChecked(true);

        osgView->pViewer->addEventHandler(new pickHandler(osgView));

        // only one eventhandler per function
        ui.btnPhoto->setCheckable(false);
        ui.btnPhoto->setEnabled(false);
    }
}


void gui::on_photo_clicked() {

    if (ui.btnPhoto->isCheckable()) {
        ui.btnPhoto->setChecked(false);
        ui.btnPhoto->setCheckable(false);
        osgView->pViewer->getEventHandlers().clear();

        // only one eventhandler per function
        ui.btnPoint->setEnabled(true);

        pic.close();

        ui.treeWidget1->clearSelection();
        move_cloud(true);
        de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 

    }
    else {
        osgView->pViewer->getEventHandlers().clear();

        move_cloud(false);
        ui.btnPhoto->setEnabled(true);
        ui.btnPhoto->setCheckable(true);
        ui.btnPhoto->setChecked(true);
        osgView->pViewer->addEventHandler(new pickPhoto(osgView));

        // only one eventhandler per function
        ui.btnPoint->setCheckable(false);
        ui.btnPoint->setEnabled(false);

        // ========================================== TO BE DONE: currently, only index 0 (parent)  of cloudsdata is stored in the list =================
        auto cloudPos = osgView->cloudsData.begin();
        std::advance(cloudPos, osgView->current_clouddata_pos);
        (*cloudPos)->distance_res = 0.0;
        (*cloudPos)->first_point = false;
        // ==============================================================================================================================================
    }
}

void gui::de_select_cloud() {
    ui.btnDel->setCheckable(false);
    ui.btnDel->setEnabled(false);
    ui.btnPoint->setCheckable(false);
    ui.btnPoint->setEnabled(false);
    osgView->pViewer->getEventHandlers().clear();
    ui.cloudPoints->clear();
    ui.cloudName->clear();
    ui.cloudCoor->clear();
    ui.cloudDist->clear();
    prev_cloud = NULL;
    osgView->remove_points();

    ui.btnPhoto->setCheckable(false);
    ui.btnPhoto->setEnabled(false);
    pic.close();

    ui.btnMesh->setCheckable(false);
    ui.btnMesh->setEnabled(false);
    ui.btnRNoise->setCheckable(false);
    ui.btnRNoise->setEnabled(false);
    ui.btnTrim->setCheckable(false);
    ui.btnTrim->setEnabled(false);
    ui.btnMove->setCheckable(false);
    ui.btnMove->setEnabled(false);
    ui.btnTreat->setCheckable(false);
    ui.btnTreat->setEnabled(false);
    treat.close();
    ui.btnMerge->setCheckable(false);
    ui.btnMerge->setEnabled(false);
    ui.btnSS->setCheckable(false);
    ui.btnSS->setEnabled(false);
    ui.btnOutput->setCheckable(false);
    ui.btnOutput->setEnabled(false);
    ui.btnOpen->setCheckable(false);
    ui.btnOpen->setEnabled(false);
    ui.btnSave->setCheckable(false);
    ui.btnSave->setEnabled(false);
    ui.btnShader->setCheckable(false);
    ui.btnShader->setEnabled(false);
    ui.btnHole->setCheckable(false);
    ui.btnHole->setEnabled(false);
}

void gui::show_select_cloud() {
    ui.btnDel->setCheckable(true);
    ui.btnDel->setEnabled(true);
    ui.btnPoint->setEnabled(true);
    ui.btnPhoto->setEnabled(true);
    pic.close();
    ui.btnMesh->setCheckable(true);
    ui.btnMesh->setEnabled(true);
    ui.btnRNoise->setCheckable(true);
    ui.btnRNoise->setEnabled(true);
    ui.btnTrim->setEnabled(true);
    ui.btnMove->setCheckable(true);
    ui.btnMove->setEnabled(true);
    ui.btnTreat->setEnabled(true);
    ui.btnMerge->setEnabled(true);
    ui.btnSS->setEnabled(true);
    ui.btnOutput->setEnabled(true);

    ui.btnOpen->setCheckable(true);
    ui.btnOpen->setEnabled(true);
    ui.btnSave->setCheckable(true);
    ui.btnSave->setEnabled(true);
    ui.btnShader->setCheckable(true);
    ui.btnShader->setEnabled(true);
    ui.btnHole->setCheckable(true);
    ui.btnHole->setEnabled(true);

}

void gui::move_cloud(bool x) {

    ui.btnDel->setCheckable(x);
    ui.btnDel->setEnabled(x);
    ui.btnPoint->setCheckable(x);
    ui.btnPoint->setEnabled(x);
    ui.btnPhoto->setCheckable(x);
    ui.btnPhoto->setEnabled(x);
    ui.btnMesh->setCheckable(x);
    ui.btnMesh->setEnabled(x);
    ui.btnRNoise->setCheckable(x);
    ui.btnRNoise->setEnabled(x);
    ui.btnTrim->setCheckable(x);
    ui.btnTrim->setEnabled(x);
    ui.btnMove->setCheckable(x);
    ui.btnMove->setEnabled(x);
    ui.btnTreat->setCheckable(x);
    ui.btnTreat->setEnabled(x);
    ui.btnMerge->setCheckable(x);
    ui.btnMerge->setEnabled(x);
    ui.btnSS->setCheckable(x);
    ui.btnSS->setEnabled(x);
    ui.btnOutput->setCheckable(x);
    ui.btnOutput->setEnabled(x);
    ui.cbColor->setEnabled(x);
    ui.comboPointsize->setEnabled(x);
    ui.comboIntensity->setEnabled(x);
    ui.treeWidget1->setEnabled(x);
    ui.btnOpen->setCheckable(x);
    ui.btnOpen->setEnabled(x);
    ui.btnSave->setCheckable(x);
    ui.btnSave->setEnabled(x);
    ui.btnShader->setCheckable(x);
    ui.btnShader->setEnabled(x);
    ui.btnHole->setCheckable(x);
    ui.btnHole->setEnabled(x);

}

//=====================================================================================================================================================================
void gui::paintEvent(QPaintEvent*)
{
    setGeo(); // widgets' positioning
    
    // tab bar's background
    QPainter painter(this);
    QPixmap pix;
    pix.load("../resources/images/background1.png");
    painter.drawPixmap(0, 29, this->width(), 45, pix);
    // Process page's background
    pix.load("../resources/images/background3.png");
    ui.bkgd1->setPixmap(pix);
    ui.bkgd1->setGeometry(this->width() * 0.01 + 10, this->height() * 0.45 + 32, this->width() - 40, this->height() * 0.385 - 6);
    pix.load("../resources/images/background4.png");
    ui.bkgd2->setPixmap(pix);
    ui.bkgd2->setGeometry(this->width() * 0.01 + 24, this->height() * 0.45 + 45, this->width() - 64, this->height() * 0.385 - 28);

    // logo ================================================================
    pix.load("../resources/images/new_title2.png"); // top left
    //p = pix.scaled(QSize(120, 20));
    ui.logo->setPixmap(pix);
    ui.logo->raise();
    ui.logo->setGeometry(0 , 0, 180,28);

    pix.load("../resources/images/new_logo1.png"); // bottom right
    ui.btmImg->setPixmap(pix);
    ui.btmImg->raise();
    ui.btmImg->setGeometry((ui.bkgd2->x()+ ui.bkgd2->width())*0.8 , ui.bkgd2->y() *1.0007, ui.bkgd2->width()*0.8, ui.bkgd2->height() * 0.995);
    //======================================================================


    if (ui.stackedWidget->currentIndex() == 1) {
        pix.load("../resources/images/background5.png");
        ui.bkgd3->setPixmap(pix);
        ui.bkgd3->setGeometry(0, 0, this->width() , 55);
        pix.load("../resources/images/background2.png");
        ui.bkgd4->setPixmap(pix);
        ui.bkgd4->setGeometry(this->width() - 55, 55, 55, this->height());

        QPalette palette;
        QColor color(ui.sliderR->value(), ui.sliderG->value(), ui.sliderB->value());
        palette.setColor(QPalette::Window,color);
        ui.labelBg->setPalette(palette);
        ui.labelBg->setAutoFillBackground(true);
    }

    if (osgView->point_check == true) {
        showCoor();
        showDist();
        osgView->point_check = false;
    }

    if (osgView->photo_check == true) {
        QString a = "../resources/images/icon2.png";
        QString b = "../resources/images/titlelogo.png";
        QString c = "../resources/images/icon3.png";
        pic.setFilepath(a, b, c);

        pic.show();
        osgView->photo_check = false;
    }

    if (osgView->plane_check) {

        osgView->plane_check = false;
        check_plane_extraction = true;
        ui.btnRTrim->setEnabled(true);
        planeextraction();
    }

    if (osgView->trim_count == 3) { // trim must more than 2 pts
        ui.btnRTrim->setEnabled(true);
        ui.btnRTrim->setCheckable(true);
        ui.btnRTrim->setChecked(true);
        osgView->trim_count = 999;
    }

    if (osgView->ss_taken) {
        osgView->ss_taken = false;
        QMessageBox::information(NULL, "Screen Shot", "Taken successfully");
    }
}

// ========================================== TO BE DONE: currently, only index 0 (parent)  of cloudsdata is stored in the list ================
void gui::showCoor()
{
    auto cloudPos = osgView->cloudsData.begin();
    std::advance(cloudPos, osgView->current_clouddata_pos);
    ui.cloudCoor->setText((*cloudPos)->getPropertyQString("coordinate"));
    ui.cloudCoor->selectAll();
    ui.cloudCoor->setAlignment(Qt::AlignLeft);
    QTextCursor cursor = ui.cloudCoor->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui.cloudCoor->setTextCursor(cursor);

}

void gui::showDist()
{
    auto cloudPos = osgView->cloudsData.begin();
    std::advance(cloudPos, osgView->current_clouddata_pos);
    
    ui.cloudDist->setText(QString::number((*cloudPos)->distance_res, 'f', 2)+" m");
    ui.cloudDist->setAlignment(Qt::AlignCenter);
}

void gui::planeextraction()
{
    // loading notification ============================ 
    startLoading();
    //==================================================
    
    // set correct path ====================================
    QTreeWidgetItem* p = osgView->cloudItem->parent();
    int cur_index = p->indexOfChild(osgView->cloudItem);
    int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
    //======================================================

    // pcl clouds vector =========================== TO BE DONE (only parent index now) ==========
    auto clouds = osgView->all_clouds.begin();
    std::advance(clouds, parent_index);
    //============================================================================================


    vector<int> temp; // store selected planes temporarily
    temp = osgView->osgAlgo->regiongrowing(*clouds, osgView->plane_x, osgView->plane_y, osgView->plane_z);
    std::vector<int>::iterator iter;

    
    osg::ref_ptr<osg::Geode> geode1 = new osg::Geode(); // temporary display of 'orange' planes
    osg::ref_ptr<osg::Geometry> geometry1(new osg::Geometry());
    osg::ref_ptr<osg::Vec3Array> vertices1(new osg::Vec3Array());
    osg::ref_ptr<osg::Vec4Array> colors1(new osg::Vec4Array()); // fake color
    

    for (iter = temp.begin(); iter != temp.end(); iter++)
    {
        osgView->all_plane.push_back(*iter);
        int cur = *iter;
        vertices1->push_back(osg::Vec3(clouds->get()->points[cur].x, clouds->get()->points[cur].y, clouds->get()->points[cur].z));
        colors1->push_back(osg::Vec4f(ui.planeR->value() / 255.0f, ui.planeG->value() /255.0f, ui.planeB->value()/255.0f, 0.1f));
        
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

    osgView->gp_temp->addChild(geode1); // to show plane extracted


    // store all planes ===========================================================================
    std::vector<int>::size_type old_Size = temp.size();//save the old size of first vector 
    osgView->all_plane.resize(old_Size + osgView->all_plane.size());//resize the first vector 
    std::copy_n(temp.begin(), temp.size(), osgView->all_plane.begin() + old_Size);
    //=============================================================================================

    stopLoading();
}


// ==============================================================================================================================================
void gui::different_view(int x) {

    osg::Vec3d viewDirection;
    osg::Vec3d upDirection;

    if (x == 1) {
        viewDirection.set(0.0, -1.0, 0.0);
        upDirection.set(0.0, 1.0, 0.0);        
        //cout << QString::number(osgView->gp_all->getNumChildren()).toStdString();
    }
    else if (x == 2) {
        viewDirection.set(0.0, 1.0, 0.0);
        upDirection.set(0.0, 0.0, 1.0);
        //osgView->gp_all->removeChild(osgView->gp_all->getNumChildren()-1);
    }
    else if (x == 3) {
        viewDirection.set(0.0, 0.0, 1.0);
        upDirection.set(0.0, 1.0, 0.0);
    }
    else if (x == 4) {
        viewDirection.set(0.0, 0.0, -1.0);
        upDirection.set(0.0, 1.0, 0.0);
    }
    else if (x == 5) {
        viewDirection.set(-1.0, 0.0, 0.0);
        upDirection.set(0.0, 0.0, 1.0);
    }
    else if (x == 6) {
        viewDirection.set(1.0, 0.0, 0.0);
        upDirection.set(0.0, 0.0, 1.0);
    }

    int cnt = ui.treeWidget1->topLevelItemCount();
    if (cnt == 0 || osgView->cloudItem == NULL || prev_cloud == NULL) {
        QMessageBox::information(NULL, "Error", "No point cloud is selected!");
    }
    else {
        osg::ref_ptr<osg::Switch> gp = osgView->gp;
        osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

        //=================================== Get Cloud Position
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //===================================

        get_pov(viewDirection, upDirection, (osg::Geode*)sub_gp->getChild(cur_index), 4.0);
    }
    
}

void gui::on_treat_clicked() { 

    if (ui.btnTreat->isCheckable()) {

        ui.treeWidget1->clearSelection();
        move_cloud(true);
        de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 
        treat.close();

    }
    else {
        
        move_cloud(false);
        ui.btnTClose->setVisible(true);
        ui.btnTClose->raise();
        ui.btnTreat->setCheckable(true);
        ui.btnTreat->setChecked(true);

        // set correct path ====================================
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //======================================================

        // pcl clouds vector =========================== TO BE DONE (only parent index now) ==========
        auto clouds = osgView->all_clouds.begin();
        std::advance(clouds, parent_index);
        //============================================================================================

        treat.setCloud(*clouds);

        treat.show();
    }
}

void gui::start_treat_cloud(QString a) {
    if (a == QString("Start")) {
        startLoading();
    }
}

void gui::treat_cloud(QString a) {
    if (a == QString("Done")) {
 
        // set correct path ====================================
        QTreeWidgetItem* p = osgView->cloudItem->parent();
        int cur_index = p->indexOfChild(osgView->cloudItem);
        int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
        //======================================================
        osg::ref_ptr<osg::Switch> sub_gp = osgView->gp->getChild(parent_index)->asSwitch();
        sub_gp->addChild(osgView->createGeode(treat.getCloud(), "treated"));
        add_widgettree(p, QString("treated point cloud ") + QString::number(osgView->treat_plane_count)); // add a new geode into the widget tree ========== 

        osgView->treat_plane_count++;

        //osgView->all_clouds.push_back(treat.getCloud()); // add to the vector to store cloud ptr
        stopLoading();
        
    }
}

void gui::on_tclose_clicked() {

    ui.btnTClose->hide();
    ui.treeWidget1->clearSelection();
    move_cloud(true);
    de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 

}

void gui::on_merge_clicked() {
    
    QList<QTreeWidgetItem*> selectedItemList = ui.treeWidget1->selectedItems();

    if (selectedItemList.size() < 2) {
        QMessageBox::information(NULL, "Error", "Not enough point clouds are selected!");
    }
    else {
        cout << " merge \n"; 
        // loading notification ============================
        startLoading();
        //==================================================
        
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr temp_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
        int temp_total_points = 0;

        int parent_index = ui.treeWidget1->indexOfTopLevelItem(selectedItemList.at(0)->parent());
        temp_cloud = osgView->all_clouds.at(parent_index);
        for (int i = 1; i < selectedItemList.size(); i++) {

            int index = ui.treeWidget1->indexOfTopLevelItem(selectedItemList.at(i)->parent());
            temp_cloud = osgView->osgAlgo->merge(temp_cloud, osgView->all_clouds.at( index));
            temp_total_points += temp_cloud->size();
        }

        
        osgView->all_clouds.push_back(temp_cloud); // add to the vector

        osg::ref_ptr<osg::Switch> sub_gp = new osg::Switch; // store clouds as sub-group inside gp
        sub_gp->addChild(osgView->createGeode(temp_cloud, "merged"));
        osgView->gp->addChild(sub_gp);

        osgUtil::Optimizer optimizer;
        optimizer.optimize(osgView->gp);


        // set cloud's property ================================================                   
        set_cloud_prop(QString("Merged Cloud"), temp_total_points);    
        //======================================================================

        // treewidget===========================================================
        QString temp = QString::number(osgView->count);
        QTreeWidgetItem* cloudTitle = new QTreeWidgetItem(ui.treeWidget1, QStringList("Cloud " + temp)); // add name (parent)
        cloudTitle->setFlags(cloudTitle->flags() & ~Qt::ItemIsSelectable);
        add_widgettree(cloudTitle, "Merged " + QString(QString::number(osgView->merged_cloud))); // add name (child)
        osgView->count++;
        osgView->merged_cloud++;
        //======================================================================
         
        stopLoading();
    }
}

void gui::add_widgettree(QTreeWidgetItem* parent, QString name) {
    QTreeWidgetItem* cloud = new QTreeWidgetItem(parent, QStringList(name)); // get cloud name (path) (first child)
    cloud->setFlags(cloud->flags() | Qt::ItemIsUserCheckable);
    cloud->setCheckState(0, Qt::Checked);
    parent->addChild(cloud);
    parent->setExpanded(true);

}

void gui::get_pov(osg::Vec3d viewDirection, osg::Vec3d upDirection, osg::ref_ptr<osg::Geode> geode1, float dist) {
    osg::Vec3d center = geode1->getBound().center();
    double viewDistance = dist * geode1->getBound().radius();
    osg::Vec3d eyePoint = center + viewDirection * viewDistance;
    osgView->pViewer->getCameraManipulator()->setHomePosition(eyePoint, center, upDirection);
    osgView->pViewer->home();
}

void gui::set_cloud_prop(QString name, int points)
{
    osgCloudData* cloudProperty = new osgCloudData; // store properties of each cloud
    cloudProperty->setProperty(name);
    cloudProperty->setProperty("points", points);
    osgView->cloudsData.push_back(cloudProperty);
}

void gui::on_help_clicked()
{
    QMessageBox::about(NULL, "Help", "Click this <a href='https://www.company.com/'>link</a> for more information.");
}

void gui::on_screenshot_clicked()
{
    if (ui.btnSS->isCheckable()) {

    }
    else {

        QMessageBox::information(NULL, "Instruction", " Double click (left mouse button) to take a screen shot at anywhere. \n Saved in the 'temp' folder as ScreenShot_x.jpg.");
        move_cloud(false);
        ui.btnSSClose->setVisible(true);
        ui.btnSSClose->raise();
        osgView->pViewer->getEventHandlers().clear();

        ui.btnSS->setCheckable(true);
        ui.btnSS->setChecked(true);

        string temppath = getTempFolder();
        cout << "SS started \n";

        screenshotHandlerObj = new screenshotHandler(osgWidget->height(), osgWidget->width(), osgView,temppath);
        osgView->pViewer->getCamera()->setPostDrawCallback(screenshotHandlerObj->getCaptureCallback());
        osgView->pViewer->addEventHandler(screenshotHandlerObj);
    }
}

void gui::on_screenshotclose_clicked() {

    ui.btnSSClose->hide();

    osgView->pViewer->getEventHandlers().clear();

    osgView->pViewer->getCamera()->removePostDrawCallback(screenshotHandlerObj->getCaptureCallback());

    ui.treeWidget1->clearSelection();
    move_cloud(true);
    de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 
    cout << "SS done \n";
}



void gui::on_output_clicked() {

    if (ui.btnOutput->isCheckable()) {
    }
    else {

        move_cloud(false);
        ui.btnOClose->setVisible(true);
        ui.btnOClose->raise();
        ui.btnOutput->setCheckable(true);
        ui.btnOutput->setChecked(true);

        reportPDF->setName(ui.cloudName->toPlainText());
        reportPDF->setPoints(ui.cloudPoints->toPlainText());
        reportPDF->setLocation(QDir::currentPath());
       
        reportPDF->refresh();
        reportPDF->show();
    }

}
void gui::on_oclose_clicked() {
    ui.btnOClose->hide();
    ui.treeWidget1->clearSelection();
    move_cloud(true);
    de_select_cloud(); // this is needed because every btn needs to be clicked twice if only move_cloud() 
    reportPDF->close();
}


static const char* blockyVertSource = {
    "// blocky.vert - an GLSL vertex shader with animation\n"
    "// the App updates uniforms \"slowly\" (eg once per frame) for animation.\n"
    "uniform float Sine;\n"
    "const vec3 LightPosition = vec3(0.0, 0.0, 4.0);\n"
    "const float BlockScale = 0.30;\n"
    "// varyings are written by vert shader, interpolated, and read by frag shader.\n"
    "varying float LightIntensity;\n"
    "varying vec2  BlockPosition;\n"
    "void main(void)\n"
    "{\n"
    "    // per-vertex diffuse lighting\n"
    "    vec4 ecPosition    = gl_ModelViewMatrix * gl_Vertex;\n"
    "    vec3 tnorm         = normalize(gl_NormalMatrix * gl_Normal);\n"
    "    vec3 lightVec      = normalize(LightPosition - vec3 (ecPosition));\n"
    "    LightIntensity     = max(dot(lightVec, tnorm), 0.0);\n"
    "    // blocks will be determined by fragment's position on the XZ plane.\n"
    "    BlockPosition  = gl_Vertex.xz / BlockScale;\n"
    "    // scale the geometry based on an animation variable.\n"
    "    vec4 vertex    = gl_Vertex;\n"
    "    vertex.w       = 1.0 + 0.4 * (Sine + 1.0);\n"
    "    gl_Position    = gl_ModelViewProjectionMatrix * vertex;\n"
    "}\n"
};

static const char* blockyFragSource = {
    "// blocky.frag - an GLSL fragment shader with animation\n"
    "// the App updates uniforms \"slowly\" (eg once per frame) for animation.\n"
    "uniform float Sine;\n"
    "const vec3 Color1 = vec3(1.0, 1.0, 1.0);\n"
    "const vec3 Color2 = vec3(0.0, 0.0, 0.0);\n"
    "// varyings are written by vert shader, interpolated, and read by frag shader.\n"
    "varying vec2  BlockPosition;\n"
    "varying float LightIntensity;\n"
    "void main(void)\n"
    "{\n"
    "    vec3 color;\n"
    "    float ss, tt, w, h;\n"
    "    ss = BlockPosition.x;\n"
    "    tt = BlockPosition.y;\n"
    "    if (fract(tt * 0.5) > 0.5)\n"
    "        ss += 0.5;\n"
    "    ss = fract(ss);\n"
    "    tt = fract(tt);\n"
    "    // animate the proportion of block to mortar\n"
    "    float blockFract = (Sine + 1.1) * 0.4;\n"
    "    w = step(ss, blockFract);\n"
    "    h = step(tt, blockFract);\n"
    "    color = mix(Color2, Color1, w * h) * LightIntensity;\n"
    "    gl_FragColor = vec4 (color, 1.0);\n"
    "}\n"
};

static const char* vertSource = {

    "varying vec4 color;\n"
    "void main(void)\n"
    "{\n"
        "color = gl_Vertex;\n"
        "gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;\n"
    "}\n"
};

static const char* fragSource = {
    "varying vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = clamp(color,0.1,0.8);\n"
    "}\n"
};


void gui::on_shader_clicked() {
    //=================================== Get Cloud Position
    QTreeWidgetItem* p = osgView->cloudItem->parent();
    int cur_index = p->indexOfChild(osgView->cloudItem);
    int parent_index = ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent());
    //===================================
    osg::ref_ptr<osg::Switch> gp = osgView->gp;
    osg::ref_ptr<osg::Switch> sub_gp = gp->getChild(ui.treeWidget1->indexOfTopLevelItem(osgView->cloudItem->parent()))->asSwitch();

    osg::Geode* geode1 = (osg::Geode*)sub_gp->getChild(cur_index); // get geode only
    osg::StateSet* ss = geode1->getDrawable(0)->getStateSet();


    if (ui.btnShader->isChecked()) {

        ui.btnShader->setChecked(true);

        osg::Program* program = new osg::Program;
        program->setName("blocky");
        /*program->addShader(new osg::Shader(osg::Shader::VERTEX, blockyVertSource));
        program->addShader(new osg::Shader(osg::Shader::FRAGMENT, blockyFragSource));*/
        //program->addShader(new osg::Shader(osg::Shader::VERTEX, vertSource));
        //program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragSource));
        
        //program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, "E:\\companyv1\\companyCode\\mainApp\\mainApp\\edl_bilateral_filter.vert"));
        //program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, "E:\\companyv1\\companyCode\\mainApp\\mainApp\\edl_bilateral_filter.frag"));
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        ss->setAttributeAndModes(program, osg::StateAttribute::ON);



    }
    else {
        ui.btnShader->setChecked(false);

        geode1->getDrawable(0)->setStateSet(new osg::StateSet);
        osg::StateSet* ss = geode1->getDrawable(0)->getStateSet();
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));
    }
   
}


// ============================================================================ 3D View Ends ======================================================================================================

void gui::dragEnterEvent(QDragEnterEvent* e)
{
 
    if (ui.stackedWidget->currentIndex() == 0) {
        if ((e->pos().y()) > 110.0 && (e->pos().y()) < 300.0)
        {
            e->acceptProposedAction();	
        }
    }
    else if (ui.stackedWidget->currentIndex() == 1) {
        
        if (e->pos().y() >= ui.widget->y() && e->pos().y() <= (ui.widget->y() + ui.widget->height() * 0.98) && e->pos().x() >= ui.widget->x() && e->pos().x() <= (ui.widget->x() + ui.widget->width() * 0.84))
        {
            if(e->pos().y() >= ui.widget->y())
            e->acceptProposedAction();	
        }
    }
}

void gui::dropEvent(QDropEvent* e)
{
    if (ui.stackedWidget->currentIndex() == 0) {
        if (running == true) { // if data is running

            globalzzenter->abortFile();
        }
        else { // first time or finished
            filename = e->mimeData()->urls().first().toString();
            if (filename.isEmpty()) { ui.dataInput1->setText("Empty File! Please Retry."); }
            else {
                if (filename.toStdString().substr(filename.toStdString().find_last_of(".") + 1) == "bag") {
                    std::string draggedfilename = filename.toStdString().substr(filename.toStdString().find_last_of("/") + 1);
                    QString qdraggedfilename = QString::fromStdString(draggedfilename);
                    ui.dataInput1->setText(QString("\"%1\" received. Please press 'Run' button to continue!").arg(qdraggedfilename));
                    ui.btnProgress->setEnabled(true);
                    ui.stage->setText("0/3"); // the label for running the .bag file input
                    ui.progressBar->setValue(0);
                }
                else {
                    ui.dataInput1->setText("Denied! Please drag .bag file only.");
                }
                ui.dataInput1->setCursorPosition(0);
            }
        }
    }
    else if (ui.stackedWidget->currentIndex() == 1) { // 3d view page

        filename = e->mimeData()->urls().first().toString();
        
        // loading notification ============================
        startLoading();
        //==================================================

        if (osgView->receivePointCloud(filename) > 0) {
            initOSG();
        }
        else {
            stopLoading();
        } 
    }
}

// ============================================================================ PROCESS PAGE ======================================================================================================
void gui::on_btnRun_clicked() {

    running = true;
    
    ui.stage->setText("1/3"); // the label for running the .bag file input
    ui.progressBar->setValue(0);
    ui.dataInput1->setText("preparing...(if the progressive bar doesn't move for 2min, please exit and retry.)");
    ui.dataInput1->setCursorPosition(0);

    ui.btnProgress->setEnabled(false);
    ui.btnProgress->hide();
    ui.btnStopProgress->raise();
    ui.btnStopProgress->setVisible(true);
    ui.btnStopProgress->setEnabled(true);

    QString lidarBag = filename;
    int lidarbagIndex = 8; // remove file:/.......

    lidarBag = lidarBag.mid(lidarbagIndex).simplified();
    lidarbagIndex = lidarBag.lastIndexOf("/");

    if (lidarbagIndex > 0)
    {

        ui.progressBar->setValue(20);

        // start ros========================================== 
        processCore = new QProcess;
        processCore->start("cmd.exe");
        processCore->write(QString(aa + "\r\n").toUtf8());
        processCore->write(QString("roscore \r\n").toUtf8());

        connect(processCore, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_roscore()));


    }
    else if (lidarbagIndex < 0)
    {
        ui.dataInput1->setText("Denied! Invalid .bag file.");
        return;
    }
   
}

void gui::slot_roscore() {
    QString str = QString(processCore->readAllStandardOutput()); //roscore need run 2 times to get output
    int index = str.lastIndexOf("NODES");

    if (index > -1) {
        slot_roscore_output();
        return;
        //mergeDeal();
    }
    else {
        QEventLoop loop1;
        QTimer::singleShot(1000, &loop1, SLOT(quit()));
        loop1.exec();
    }
}

void gui::slot_roscore_output() {

    if (running) {
        ui.progressBar->setValue(40);
        ui.dataInput1->setText("Configuration..(if the progressive bar doesn't move for 1min, please exit and retry.)");
        ui.dataInput1->setCursorPosition(0);

        cmd_yaml = new QProcess;
        cmd_yaml->start("cmd.exe");

        QString lD3_startAlgorithm_str1 = "rosparam load params.yaml && echo CFYcfy";
        cmd_yaml->write(QString(aa + "\r\n").toUtf8());
        cmd_yaml->write(lD3_startAlgorithm_str1.toLocal8Bit() + '\r' + '\n');
        connect(cmd_yaml, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_checkYaml()));

    }

}

void gui::slot_checkYaml() {

    QString str = QString(cmd_yaml->readAllStandardOutput());
    int index = str.lastIndexOf("cfy"); // will echo twice

    if (index > 1) {
        paramIndex++;
        if (paramIndex > 1) {

            checkbagC();
            paramIndex = 0;
        }
    }
}

void gui::checkbagC() {

    if (running) {

        QString lidarBag = filename;

        QString path1 = "";
        QString path2 = "";
        QString  D3_checkbagStr = "";
        processCheckC = new QProcess; // preparation
        processCheckC->start("cmd.exe");

        int lidarbagIndex = 8; // remove file:/.......

        lidarBag = lidarBag.mid(lidarbagIndex).simplified();
        lidarbagIndex = lidarBag.lastIndexOf("/");

        if (lidarbagIndex > 0)
        {
            path1 = lidarBag.replace('/', '\\');
            D3_checkbagStr = QString("rosbag check %1").arg(path1);
            suanfaBOOL = true; 

            // write to terminal
            processCheckC->write(QString(aa + "\r\n").toUtf8());
            processCheckC->write(QString(D3_checkbagStr + "\r\n").toUtf8());
            ui.dataInput1->setText("Checking bag...(if the progress bar doesn't move for 5mins, please check your bag or ask supplier)");
            ui.dataInput1->setCursorPosition(0);
            ui.progressBar->setValue(60);

            connect(processCheckC, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_bagcheck_output()));

        }
        else if (lidarbagIndex < 0)
        {
            ui.dataInput1->setText("Denied! Invalid .bag file.");
            return;
        }
    }
}

void gui::slot_bagcheck_output()
{
    if (running) {
        QString str = QString(processCheckC->readAllStandardOutput());
        int check = str.indexOf("Bag file does not need any migrations");

        if (check < 0)
        {
        }
        else // check the 3 topics and included in the bag
        {
            //====================================================================================
            QEventLoop loop1;
            QTimer::singleShot(500, &loop1, SLOT(quit()));
            loop1.exec();
            //====================================================================================

            checkbag();
            return;
        }
    }
}

void gui::checkbag() {

    if (running) {

        QString lidarBag = filename;

        QString path1 = "";
        QString path2 = "";
        QString  D3_checkbagStr = "";
        processCheck = new QProcess; // preparation
        processCheck->start("cmd.exe");

        int lidarbagIndex = 8; // remove file:/.......

        lidarBag = lidarBag.mid(lidarbagIndex).simplified();
        lidarbagIndex = lidarBag.lastIndexOf("/");

        if (lidarbagIndex > 0)
        {
            path1 = lidarBag.replace('/', '\\');
            D3_checkbagStr = QString("rosbag info %1").arg(path1);
            //algoBOOL = true; 

            // write to terminal
            processCheck->write(QString(aa + "\r\n").toUtf8());
            processCheck->write(QString(D3_checkbagStr + "\r\n").toUtf8());
            ui.dataInput1->setText("Checking topics...(if the progress bar doesn't move for 3mins, please check your bag or ask supplier)");
            ui.dataInput1->setCursorPosition(0);
            ui.progressBar->setValue(90);


            connect(processCheck, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_baginfo_output()));

        }
        else if (lidarbagIndex < 0)
        {
            ui.dataInput1->setText("Denied! Invalid .bag file.");
            return;
        }
    }
}

void gui::slot_baginfo_output()
{
    if (running) {
        QString str = QString(processCheck->readAllStandardOutput());
        int imuIndex = str.indexOf("/imu/data");
        int encIndex = str.indexOf("/ENCread");
        int hesaiIndex = str.indexOf("/hesai/pandar");
        int end = str.indexOf("msgs    : sensor_msgs/PointCloud2");

        if (imuIndex < 0 && encIndex < 0 && hesaiIndex < 0)
        {
        }
        else // check the 3 topics and included in the bag
        {
            //====================================================================================

            QString get_total_frame = "";
            for (int i = hesaiIndex; i < end; i++) {
                get_total_frame += str.at(i);
            }
            QStringList l = get_total_frame.split(" ");
            frame_size = l.at(l.size() - 2).toInt(); // get frame number
            QEventLoop loop1;
            QTimer::singleShot(1000, &loop1, SLOT(quit()));
            loop1.exec();
            //====================================================================================

            ui.btnStopProgress->setEnabled(false);
            slot_startBag();
        }
    }
}
void gui::slot_startBag() // play and record simultaneously
{  
    if (running) {

        processStart = new QProcess;
        processStart->start("cmd.exe");
        QString path1 = "";
        QString path2 = "";
        QString  D3_startBagStr = "";
        QString lidarBag = filename;
        int lidarbagIndex = 8;
        lidarBag = lidarBag.mid(lidarbagIndex).simplified();
        lidarbagIndex = lidarBag.lastIndexOf("/");


        if (lidarbagIndex > 0)
        {
            path1 = lidarBag.replace('/', '\\');
            path2 = lidarBag.left(lidarbagIndex + 1);

        }

        //(record)===============================================================================
        DatatimeDir = (QDateTime::currentDateTime()).toString("yyyy.MM.dd_hh:mm:ss");
        DatatimeDir = DatatimeDir.replace('.', '_');
        DatatimeDir = DatatimeDir.replace(':', '_');
        DatatimeabsoDir = path2 + DatatimeDir;

        //============================================================================
        processMkdir = new QProcess;
        processMkdir->start("cmd.exe");
        QString D3_mkdirBagFile = QString("md %1").arg(DatatimeabsoDir);
        processMkdir->write(D3_mkdirBagFile.toLocal8Bit() + '\n');
        QEventLoop loop1;
        QTimer::singleShot(1000, &loop1, SLOT(quit()));
        loop1.exec();
        // ================================================================================================================


        processRecord = new QProcess;
        processRecord->start("cmd.exe");
        QString D3_RecordBagStr = QString("rosbag record -O %1\\filename.bag -b 4096 /algorithm1/mapping/cloud_registered_raw __name:=record_data").arg(DatatimeabsoDir);  //   /hilbert_h/mapping/cloud_registered_raw  /velodyne_points
        processRecord->write(QString(aa + "\r\n").toUtf8());
        processRecord->write(D3_RecordBagStr.toLocal8Bit() + '\n');
        QEventLoop loop11;
        QTimer::singleShot(1000, &loop11, SLOT(quit()));
        loop11.exec();

        //========================================================================================================================================

        // read the bag
        D3_startBagStr = QString("algorithm1_nrt.exe %1 __name:=bagplay && echo CFYplaybagendcfy").arg(path1);

        processStart->write(QString(aa + "\r\n").toUtf8());
        processStart->write(D3_startBagStr.toLocal8Bit() + '\n');

        connect(processStart, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_startBag_output()));


        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();              
        QString str1 = process.readAllStandardOutput();

        if (!str1.contains("record.exe") && ui.progressBar->value() == 0)
        {
        }

        ui.progressBar->setValue(0);
        ui.stage->setText("2/3");
        ui.dataInput1->setText("Recording..(starts soon)");
    }
}

void gui::slot_startBag_output()
{
    if (running) {

        QString str = QString(processStart->readAllStandardOutput());
        int index = str.lastIndexOf("playbagendcfy"); // will echo twice
        //qDebug() << str;
        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();                 
        QString str1 = process.readAllStandardOutput();

        if (!str1.contains("algorithm1_nrt.exe") && ui.progressBar->value() == 0)
        {
        }
        if (index > 1)
        {
            playbagIndex = playbagIndex + 1;

            ui.btnStopProgress->setEnabled(true);

            if (playbagIndex > 1)
            {

                closeAll(); // close play, record

                QEventLoop loop;
                QTimer::singleShot(2000, &loop, SLOT(quit()));
                loop.exec();
                ui.progressBar->setValue(100);

                mergeDeal();
                playbagIndex = 0;
            }
        }

        int check = str.lastIndexOf("failed rate");
        if (check > 0) {
            int indexDuration = str.indexOf(" | ") + 2;
            QString progress = str.mid(indexDuration);
            progress = progress.simplified().section(' ', 0, 0).simplified();
            int current_prog = (progress.toInt() * 100 / frame_size);
            ui.progressBar->setValue(current_prog);
            ui.dataInput1->setText("Recording..(this will take a bit longer)");
        }

    }
}

void gui::mergeDeal()
{
    if (running) {

        //DatatimeabsoDir = "C:\\Users\\company\\Desktop\\2022_09_01_10_38_58";
        ui.dataInput1->setText("Merging..(starts soon)");
        processM = new QProcess;
        processM->start("cmd.exe");

        QString D3_merge_str = QString("merge.exe %1\\filename.bag /algorithm1/mapping/cloud_registered_raw").arg(DatatimeabsoDir);
        processM->write(QString(aa + "\r\n").toUtf8());
        processM->write(D3_merge_str.toLocal8Bit() + '\r' + '\n');

        connect(processM, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_D3_merge_output()));

        ui.stage->setText("3/3");
        ui.progressBar->setValue(0);
    }
}


void gui::slot_D3_merge_output()
{
    if (running) {
        QString str = QString(processM->readAllStandardOutput());
        int index = str.lastIndexOf("success");

        if (count > 1) { // set progress bar's value
            if (ui.progressBar->value() > 40) {
                ui.dataInput1->setText("Please wait a bit longer..");
            }
            else {
                ui.dataInput1->setText("Merging..");
            }
        }

        if (index < 0) {

            if (str.lastIndexOf("\r\n") > 0) {
                if (count * 7 < 100) {
                    ui.progressBar->setValue(count * 7);
                    count++;
                }
            }
        }
        else {
            count = 1;
            delBagFile();
            return;
        }
    }
}

void gui::delBagFile()
{
    processD = new QProcess;
    processD->start("cmd.exe");


    QString D3_delBagFile_str = QString("del/f/s/q %1\\filename.bag").arg(DatatimeabsoDir);     //.arg(DatatimeabsoDir);  //&& cd ~/%1 && rm -rf pcd0/ && rm -f filename.bag
    processD->write(D3_delBagFile_str.toLocal8Bit() + '\n');  //************NEED UNCOMMENTED

    QEventLoop loop;
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();

    ui.progressBar->setValue(100);
    ui.dataInput1->setText("done, thanks for waiting.");
    ui.btnProgress->setVisible(true);
    ui.btnStopProgress->hide();
    ui.btnStopProgress->setEnabled(false);



    HistoricalDir3 = HistoricalDir2;
    HistoricalDir2 = HistoricalDir1;
    HistoricalDir1 = DatatimeabsoDir;

    ui.dataline1->setText(HistoricalDir1);
    ui.dataline2->setText(HistoricalDir2);
    ui.dataline3->setText(HistoricalDir3);

    QFile file("./historical_data.txt");  //new file
    bool fileok = file.open(QIODevice::WriteOnly);
    if (fileok == true)
    {
        QString str = QString("%1\r\n%2\r\n%3").arg(HistoricalDir1).arg(HistoricalDir2).arg(HistoricalDir3);
        file.write(str.toStdString().data());
    }
    file.close();

    algoBOOL = false;


    if ((globalzzenter->outtime == true) && (suanfaBOOL == false)) // if license expired, wait for the conversion of the .bag file to be completed first.
    {
        //QMessageBox::information(NULL, "a", "gg5");
        time(&now);
        globalzzenter->timeStr = QString::fromStdString(std::to_string(now));
        globalzzenter->timePos = now;
        QString x("PocoDynamic.dll");
        globalzzenter->saveDataAsStream(x);
        this->hide();
        dlg.show();
        dlg.datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        dlg.datasetlabel->setText("License expired. Please contact the supplier.");
    }

    filename = "";
    running = false;

}

void gui::closeAll()
{
    processCl = new QProcess;
    processCl->start("cmd.exe");
    processCl->write(QString(aa + "\r\n").toUtf8());
    QString D3_closeAll_str3 = " rosnode kill record_data";
    processCl->write(D3_closeAll_str3.toLocal8Bit() + '\n');
    QString D3_closeAll_str1 = "rosnode kill /ekf_gps";
    processCl->write(D3_closeAll_str1.toLocal8Bit() + '\n');
    QString D3_closeAll_str2 = " rosnode kill bagplay";
    processCl->write(D3_closeAll_str2.toLocal8Bit() + '\n');
    
}

void gui::on_btnRun_Close_clicked() { // Stop Button 

    QMessageBox::StandardButton check;
    check = QMessageBox::information(NULL, "The process is running", "Are you sure to stop the process?", QMessageBox::Yes, QMessageBox::No);
    if (check == QMessageBox::Yes) {

        running = false;
        algo = false;
        ui.dataInput1->setText("Waiting..");

        if (cmd_yaml != NULL) {

            disconnect(cmd_yaml, 0, 0, 0);
            cmd_yaml->terminate();
            cmd_yaml = NULL;

        }
        if (processCore != NULL) {

            disconnect(processCore, 0, 0, 0);
            processCore->terminate();
            processCore = NULL;

        }
        if (processCheck != NULL) {

            disconnect(processCheck, 0, 0, 0);
            processCheck->terminate();
            processCheck = NULL;

        }
        if (processCheckC != NULL) {

            disconnect(processCheckC, 0, 0, 0);
            processCheckC->terminate();
            processCheckC = NULL;

        }
        if (processStart != NULL) {

            disconnect(processStart, 0, 0, 0);
            processStart->terminate();
            processStart = NULL;

        }
        if (processM != NULL) {

            disconnect(processM, 0, 0, 0);
            processM->terminate();
            processM = NULL;

        }
        if (processRecord != NULL) {

            disconnect(processRecord, 0, 0, 0);
            processRecord->terminate();
            processRecord = NULL;

            closeAll(); // waiting for shutting down rosbag record

        }
        if (processMkdir != NULL) {

            disconnect(processMkdir, 0, 0, 0);
            processMkdir->terminate();
            processMkdir = NULL;

        }

        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished(-1);                   
        QString str1 = process.readAllStandardOutput();

        if (str1.contains("algorithm1_nrt.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt.exe"));
        }
        if (str1.contains("merge.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
        }


        QEventLoop loop;
        QTimer::singleShot(3000, &loop, SLOT(quit()));
        loop.exec();


        QFileInfo file1;
        file1 = QString("%1").arg(DatatimeabsoDir);
        if (file1.exists() == true) {

            QDir dir(DatatimeabsoDir);
        }


        playbagIndex = 0; // for 2/3
        count = 1; // for 3/3

        process.close();
        ui.dataInput1->setText("Cancelled, drag a .bag file to begin.");
        ui.btnProgress->setVisible(true);
        ui.btnStopProgress->hide();
        ui.btnStopProgress->setEnabled(false);
        //ui.stage->setText("1/3"); // the label for running the .bag file input
        //ui.progressBar->setValue(0);
    }
}
//========================================================================== PROCESS ENDS =========================================================================================================



void gui::setUpProUi() {
    // three buttons on the title bar =========================================================
    btnClose = new QPushButton("");
    btnMax = new QPushButton("");
    btnMin = new QPushButton("");
    btnMin->setStyleSheet("QPushButton{border-image:url(../resources/images/minimizenew.png);} QPushButton::hover{ border-image:url(../resources/images/minimize.png);}");
    btnMax->setStyleSheet("QPushButton{border-image:url(../resources/images/maxnew.png); }QPushButton::hover{ border-image:url(../resources/images/max.png);} ");
    btnClose->setStyleSheet("QPushButton{border-image:url(../resources/images/closenew.png);}QPushButton::hover{ border-image:url(../resources/images/close.png);}");
    btnClose->setParent(this);
    btnMin->setParent(this);
    btnMax->setParent(this);
    connect(btnClose, SIGNAL(clicked()), this, SLOT(quit_mainwindow()));
    connect(btnMin, SIGNAL(clicked()), this, SLOT(minimize_mainwindow()));
    connect(btnMax, SIGNAL(clicked()), this, SLOT(max_mainwindow()));
    //=============================================================================================

    // the two pages (Process & 3D View)
    connect(ui.btnProcess, SIGNAL(clicked()), this, SLOT(on_process_clicked()));
    connect(ui.btn3dview, SIGNAL(clicked()), this, SLOT(on_3dview_clicked()));
    connect(ui.btn3dview, SIGNAL(clicked()), this, SLOT(btn3dview_clicked()));
    connect(ui.btnProcess, SIGNAL(clicked()), this, SLOT(btnprocess_clicked()));

    // About page
    //connect(ui.btnAbout, SIGNAL(clicked()), this, SLOT(on_btnAbout_clicked()));

    // RUN button
    connect(ui.btnProgress, SIGNAL(clicked()), this, SLOT(on_btnRun_clicked()));
    // Stop button
    connect(ui.btnStopProgress, SIGNAL(clicked()), this, SLOT(on_btnRun_Close_clicked()));

    // OPEN btn in the Process page=============================================
    connect(ui.btnopen1, SIGNAL(clicked()), this, SLOT(on_btnOpen_clicked1())); 
    connect(ui.btnopen2, SIGNAL(clicked()), this, SLOT(on_btnOpen_clicked2())); 
    connect(ui.btnopen3, SIGNAL(clicked()), this, SLOT(on_btnOpen_clicked3())); 
    //==========================================================================

    // DISPLAY btn in the Process page=============================================
    connect(ui.btndisplay1, SIGNAL(clicked()), this, SLOT(on_btnDisp_clicked1()));
    connect(ui.btndisplay2, SIGNAL(clicked()), this, SLOT(on_btnDisp_clicked2()));
    connect(ui.btndisplay3, SIGNAL(clicked()), this, SLOT(on_btnDisp_clicked3()));
    //==========================================================================

    // for 3d view =============================================================================================
    osgWidget = new osgQOpenGLWidget(ui.widget);          //  Appoint osg Window display position 
    osgView = new osgview;
    reportPDF = new report;
    //QWidget* title = ui.dock3->titleBarWidget();// remove right dockwidget title bar
    //ui.dock3->setTitleBarWidget(new QWidget);
    //delete title;
    connect(osgWidget, SIGNAL(initialized()), this, SLOT(initOSG())); // for background color
    connect(ui.treeWidget1, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(on_selectCloud_clicked(QTreeWidgetItem*, int))); // select cloud 
    connect(ui.btnDel, SIGNAL(clicked()), this, SLOT(on_btndelCloud_clicked()));
    connect(ui.btnView1, SIGNAL(clicked()), this, SLOT(on_btnView1Cloud_clicked()));
    connect(ui.btnView2, SIGNAL(clicked()), this, SLOT(on_btnView2Cloud_clicked()));
    connect(ui.btnView3, SIGNAL(clicked()), this, SLOT(on_btnView3Cloud_clicked()));
    connect(ui.btnView4, SIGNAL(clicked()), this, SLOT(on_btnView4Cloud_clicked()));
    connect(ui.btnView5, SIGNAL(clicked()), this, SLOT(on_btnView5Cloud_clicked()));
    connect(ui.btnView6, SIGNAL(clicked()), this, SLOT(on_btnView6Cloud_clicked()));
    connect(ui.btnSetBg, SIGNAL(clicked()), this, SLOT(on_setBg_changed()));
    connect(ui.btnPoint, SIGNAL(clicked()), this, SLOT(on_point_clicked()));
    connect(ui.btnPhoto, SIGNAL(clicked()), this, SLOT(on_photo_clicked()));
    connect(ui.comboPointsize, SIGNAL(activated(QString)), this, SLOT(slot_pointsize(QString)));
    connect(ui.comboIntensity, SIGNAL(activated(int)), this, SLOT(slot_colormap(int)));
    connect(ui.cbColor, SIGNAL(activated(int)), this, SLOT(slot_color(int)));
    connect(ui.btnMesh, SIGNAL(clicked()), this, SLOT(on_mesh_clicked()));
    connect(ui.btnRNoise, SIGNAL(clicked()), this, SLOT(on_rnoise_clicked()));
    connect(ui.btnTrim, SIGNAL(clicked()), this, SLOT(on_trim_clicked()));
    connect(ui.btnMove, SIGNAL(clicked()), this, SLOT(on_move_clicked()));
    connect(ui.btnMClose, SIGNAL(clicked()), this, SLOT(on_mclose_clicked()));
    connect(ui.btnTClose, SIGNAL(clicked()), this, SLOT(on_tclose_clicked()));
    connect(ui.btnTarget, SIGNAL(clicked()), this, SLOT(on_target_clicked()));
    connect(ui.btnRClose, SIGNAL(clicked()), this, SLOT(on_rclose_clicked()));
    connect(ui.btnRPlane, SIGNAL(clicked()), this, SLOT(on_rplane_clicked()));
    connect(ui.btnRPSave, SIGNAL(clicked()), this, SLOT(on_rplanesave_clicked()));
    connect(ui.btnRTrim, SIGNAL(clicked()), this, SLOT(on_rtrim_clicked()));
    connect(ui.btnTreat, SIGNAL(clicked()), this, SLOT(on_treat_clicked()));
    connect(&treat, SIGNAL(treat_the_cloud(QString)), this, SLOT(treat_cloud(QString)));
    connect(&treat, SIGNAL(start_treat_the_cloud(QString)), this, SLOT(start_treat_cloud(QString)));
    connect(ui.btnMeClose, SIGNAL(clicked()), this, SLOT(on_meclose_clicked()));
    connect(ui.btnMePoss, SIGNAL(clicked()), this, SLOT(on_meposs_clicked()));
    connect(ui.btnMeAf, SIGNAL(clicked()), this, SLOT(on_meaf_clicked()));
    connect(ui.btnMeSs, SIGNAL(clicked()), this, SLOT(on_mess_clicked()));
    connect(ui.btnMerge, SIGNAL(clicked()), this, SLOT(on_merge_clicked()));
    connect(ui.btnHelp, SIGNAL(clicked()), this, SLOT(on_help_clicked()));
    connect(ui.btnSS, SIGNAL(clicked()), this, SLOT(on_screenshot_clicked()));
    connect(ui.btnSSClose, SIGNAL(clicked()), this, SLOT(on_screenshotclose_clicked()));
    connect(ui.btnOutput, SIGNAL(clicked()), this, SLOT(on_output_clicked()));
    connect(ui.btnOClose, SIGNAL(clicked()), this, SLOT(on_oclose_clicked()));
    connect(ui.btnShader, SIGNAL(clicked()), this, SLOT(on_shader_clicked()));
    ui.comboIntensity->setItemData(2, "Red->Yellow->White", Qt::ToolTipRole);
    ui.comboIntensity->setItemData(4, "Blue->Green->Yellow->Red", Qt::ToolTipRole);

    //==========================================================================================================
}

void gui::on_process_clicked() {

    ui.btn3dview->setChecked(false);
    ui.btn3dview->setCheckable(false);
    ui.btn3dview->setCheckable(true);
    //ui.btnAbout->setChecked(false);
    //ui.btnAbout->setCheckable(false);
    //ui.btnAbout->setCheckable(true);

    ui.btnProcess->setCheckable(true);
    ui.btnProcess->setChecked(true);
}

void gui::on_3dview_clicked() {

    ui.btnProcess->setChecked(false);
    ui.btnProcess->setCheckable(false);
    ui.btnProcess->setCheckable(true);
    //ui.btnAbout->setChecked(false);
    //ui.btnAbout->setCheckable(false);
    //ui.btnAbout->setCheckable(true);

    ui.btn3dview->setCheckable(true);
    ui.btn3dview->setChecked(true);
}

void gui::btn3dview_clicked() {
    
    ui.stackedWidget->setCurrentIndex(1);

}

void gui::btnprocess_clicked() {

    ui.stackedWidget->setCurrentIndex(0);
}

//void gui::on_btnAbout_clicked() {
//    ui.btnProcess->setChecked(false);
//    ui.btnProcess->setCheckable(false);
//    ui.btn3dview->setChecked(false);
//    ui.btn3dview->setCheckable(false);
//    
//
//    ui.btnAbout->setCheckable(true);
//    ui.btnAbout->setChecked(true);
//
//    QMessageBox::about(this, "company", "All right reserves.");
//
//}

void gui::quit_mainwindow()
{

    QProcess process;
    QString cmdd = "tasklist";
    process.start(cmdd);
    process.waitForFinished();          
    QString str1 = process.readAllStandardOutput();

    if (str1.contains("roscore.exe") )
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("roscore.exe"));
    }
    if (str1.contains("rosmaster.exe") )
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("rosmaster.exe"));
    }
    if (str1.contains("rosout.exe") )
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("rosout.exe"));
    }
    if (str1.contains("python.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("python.exe"));
    }
    if (str1.contains("mainApp.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("mainApp.exe"));
    }
    if (str1.contains("algorithm1_nrt.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt.exe"));
    }
    if (str1.contains("merge.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
    }
    
   
    cmd_yaml->close();
    processCore->close();
    processCore1->close();
    processCheck->close();
    processCheckC->close();
    processStart->close();
    processMkdir->close();
    processRecord->close();
    processM->close();
    processD->close();
    processCl->close();
    process.close();


    exit(1);
}

void gui::minimize_mainwindow() {
    this->setWindowState(Qt::WindowMinimized);
}

void gui::max_mainwindow() {

     if (!max_pressed ){
        prev_width = geometry().width();
        prev_height = geometry().height();

        QDesktopWidget* desktop = QApplication::desktop();
        QRect desktopGeometry = desktop->availableGeometry();
        int desktopHeight = desktopGeometry.height();
        int desktopWidth = desktopGeometry.width();
        resize(desktopWidth, desktopHeight);

        int padx = (frameGeometry().width() - geometry().width()) / 2;
        move(-padx, 0);

        max_pressed = true;

    }
    else {    
         QDesktopWidget* desktop = QApplication::desktop();
         QRect desktopGeometry = desktop->availableGeometry();

         int desktopHeight1 = prev_height;
         int desktopWidth1 = prev_width;
         resize(desktopWidth1, desktopHeight1);

         int padx = (desktopGeometry.width() - prev_width) / 2;
         int pady = (desktopGeometry.height() - prev_height) / 2;
         move(+padx, +pady);
         
         max_pressed = false;

    }
}

void gui::mousePressEvent(QMouseEvent* event)
{
    
    /*float pox = ((QMouseEvent*)event)->pos().x();
    float poy = ((QMouseEvent*)event)->pos().y();
    QMessageBox::information(NULL, "a", QString::number(pox) + "___" + QString::number(poy));*/

    if (event->button() == Qt::LeftButton && ((QMouseEvent*)event)->pos().y()<=30.0){ // only can drag when mouse at top bar
        move1 = true;
        lastPos = event->globalPos() - pos();
    }
    return QFrame::mousePressEvent(event);

}

void gui::mouseMoveEvent(QMouseEvent* event)
{

    if (move1 && (event->buttons() && Qt::LeftButton) && (event->globalPos() - lastPos).manhattanLength() > QApplication::startDragDistance()) {

        move(event->globalPos() - lastPos);
        lastPos = event->globalPos() - pos();
 
    }

    return QFrame::mouseMoveEvent(event);
}

void gui::mouseReleaseEvent(QMouseEvent* event)
{
    move1 = false;
   
}

void gui::bringFront() {
    ui.widget->raise();
    ui.label->raise();
    ui.label_2->raise();
    ui.label_3->raise();
    ui.dataline1->raise();
    ui.dataline2->raise();
    ui.dataline3->raise();
    ui.btnopen1->raise();
    ui.btnopen2->raise();
    ui.btnopen3->raise();
    ui.btndisplay1->raise();
    ui.btndisplay2->raise();
    ui.btndisplay3->raise();
    ui.dataInput1->raise();
    ui.bkgd3->raise();
    ui.dock1->raise();
    ui.dock2->raise();
    ui.bkgd4->raise();
    ui.btnStopProgress->hide();
    ui.btnDel->raise();
    ui.btnPoint->raise();
    ui.btnView1->raise();
    ui.btnView2->raise();
    ui.btnView3->raise();
    ui.btnView4->raise();
    ui.btnView5->raise();
    ui.btnView6->raise();
    ui.sliderR->raise();
    ui.sliderG->raise();
    ui.sliderB->raise();
    ui.btnSetBg->raise();
    ui.btnPhoto->raise();
    ui.btnMesh->raise();
    ui.btnRNoise->raise();
    ui.btnTrim->raise();
    ui.btnMove->raise();
    ui.btnTarget->raise();
    ui.btnTreat->raise();
    ui.btnMerge->raise();
    ui.btnHelp->raise();
    ui.btnSS->raise();
    ui.btnOutput->raise();
    ui.btnOpen->raise();
    ui.btnSave->raise();
    ui.btnShader->raise();
    ui.btnHole->raise();

    ui.btnRClose->hide();
    ui.btnRPSave->hide();
    ui.btnRPlane->hide();
    ui.btnMClose->hide();
    ui.btnTClose->hide();
    ui.btnRTrim->hide();
    ui.planeR->hide();
    ui.planeG->hide();
    ui.planeB->hide();
    ui.planeColour->hide();
    ui.btnMeClose->hide();
    ui.btnMeSs->hide();
    ui.btnMeAf->hide();
    ui.btnMePoss->hide();
    ui.btnSSClose->hide();
    ui.btnOClose->hide();

    ui.lastItemClicked->raise();


}

void gui::setGeo() {

    // title bar 3 buttons
    btnClose->setGeometry(this->width() - 37, -3.5, 32, 32);
    btnMax->setGeometry(this->width() - 77, -3.5, 32, 32);
    btnMin->setGeometry(this->width() - 117, -3, 32, 32);
    // to set individuals' position (varies with the size of the UI) ========================================================================
    // tab bar titles
    ui.btnProcess->setGeometry(20, 28, 150, 50);
    ui.btn3dview->setGeometry(170, 28, 150, 50);
    //ui.btnAbout->setGeometry(this->width()-130, 40, 150, 50);
    //to set the individual UIs
    ui.stackedWidget->setGeometry(0, 73, this->width(), this->height());
    // for data sets: to display/open
    ui.label->setGeometry(60, this->height() * 0.578 + 5, 240, 30);
    ui.label_2->setGeometry(60, this->height() * 0.578 + 65, 240, 30);
    ui.label_3->setGeometry(60, this->height() * 0.578 + 125, 240, 30);
    ui.dataline1->setGeometry(150, this->height() * 0.578 + 5, this->width() * 0.31, 30);
    ui.dataline2->setGeometry(150, this->height() * 0.578 + 65, this->width() * 0.31, 30);
    ui.dataline3->setGeometry(150, this->height() * 0.578 + 125, this->width() * 0.31, 30);
    ui.btnopen1->setGeometry(this->width() * 0.325 + 150, this->height() * 0.578 + 5, 80, 30);
    ui.btnopen2->setGeometry(this->width() * 0.325 + 150, this->height() * 0.578 + 65, 80, 30);
    ui.btnopen3->setGeometry(this->width() * 0.325 + 150, this->height() * 0.578 + 125, 80, 30);
    ui.btndisplay1->setGeometry(this->width() * 0.325 + 250, this->height() * 0.578 + 5, 80, 30);
    ui.btndisplay2->setGeometry(this->width() * 0.325 + 250, this->height() * 0.578 + 65, 80, 30);
    ui.btndisplay3->setGeometry(this->width() * 0.325 + 250, this->height() * 0.578 + 125, 80, 30);
    // data/file to be dragged into
    ui.dataInput->setGeometry(20, 30, this->width() - 40, this->height() * 0.197);
    ui.dataInput1->setGeometry(50, this->height() * 0.063, this->width() - 100, this->height() * 0.134);
    // progress
    ui.progressBar->setGeometry(20, 52 + this->height() * 0.197, (this->width() - 40) - this->height() * 0.197 - 10, this->height() * 0.197);
    ui.stage->setGeometry((this->width() - 40) - this->height() * 0.25 - 5, 52 + this->height() * 0.197, 50, this->height() * 0.197);
    // the 'run' button
    ui.btnProgress->setGeometry((this->width() - 40) - this->height() * 0.197 + 20, 52 + this->height() * 0.197, this->height() * 0.197, this->height() * 0.197);
    // the 'stop' button
    ui.btnStopProgress->setGeometry((this->width() - 40) - this->height() * 0.197 + 20, 52 + this->height() * 0.197, this->height() * 0.197, this->height() * 0.197);
    // ======================================================================================================================================
    ui.labelTime->setGeometry(this->width()-350, this->height()*0.97, 400, 19);
    ui.labelTime->setText(globalzzenter->maintitleQstr);
    // 3d view =====================================================================
    //============================= top bar ======================
    osgWidget->resize(this->width()*0.88, this->height()*0.88);
    ui.widget->setGeometry(ui.dock1->x()+ ui.dock1->width() +5 , 30, this->width(), this->height());
    ui.dock1->setGeometry(10, 60, this->width() / 8 , (this->height()/2)-130);
    ui.dock2->setGeometry(10, this->height() / 2 - 60, this->width() / 8, (this->height() / 2) - 50);
    ui.btnDel->setGeometry(ui.dock1->x() + ui.dock1->width() + 5, 5, 55, 50);
    ui.btnOpen->setGeometry(ui.btnDel->x() + 65 , 5, 55, 50);
    ui.btnSave->setGeometry(ui.btnDel->x() + 65 * 2, 5, 55, 50);
    ui.btnPoint->setGeometry(ui.btnDel->x()+65 * 3, 5, 55, 50);
    ui.btnPhoto->setGeometry(ui.btnDel->x() + 65 * 4, 5, 55, 50);
    ui.btnTrim->setGeometry(ui.btnDel->x() + 65 * 5, 5, 55, 50);
    ui.btnMove->setGeometry(ui.btnDel->x() + 65 * 6, 5, 55, 50);
    ui.btnMesh->setGeometry(ui.btnDel->x() + 65 * 7, 5, 55, 50);
    ui.btnRNoise->setGeometry(ui.btnDel->x() + 65 * 8, 5, 55, 50);
    ui.btnTreat->setGeometry(ui.btnDel->x() + 65 * 9, 5, 55, 50);
    ui.btnMerge->setGeometry(ui.btnDel->x() + 65 * 10, 5, 55, 50);
    ui.btnHole->setGeometry(ui.btnDel->x() + 65 * 11, 5, 55, 50);
    ui.btnSS->setGeometry(ui.btnDel->x() + 65 * 12, 5, 55, 50);
    ui.btnOutput->setGeometry(ui.btnDel->x() + 65 * 13, 5, 55, 50);
    ui.btnHelp->setGeometry(this->width() - 55, 5, 55, 50);
    //============================ right bar ========================
    ui.btnShader->setGeometry(this->width() - 55, 60, 55, 50);
    ui.btnView1->setGeometry(this->width() - 55, 115, 55, 50);
    ui.btnView2->setGeometry(this->width() - 55, 170, 55, 50);
    ui.btnView3->setGeometry(this->width() - 55, 225, 55, 50);
    ui.btnView4->setGeometry(this->width() - 55, 280, 55, 50);
    ui.btnView5->setGeometry(this->width() - 55, 335, 55, 50);
    ui.btnView6->setGeometry(this->width() - 55, 390, 55, 50);
    ui.btnTarget->setGeometry(this->width() - 55, 445, 55, 50);
    // ======================== sub bar ===========================
    ui.btnMClose->setGeometry(this->width() - 60 - 60, 65, 60, 22);
    ui.btnTClose->setGeometry(this->width() - 60 - 60, 65, 60, 22);
    ui.btnRClose->setGeometry(this->width() -60 - 60, 65, 60, 22);
    ui.btnRPSave->setGeometry(this->width() - 120 - 65, 65, 60, 22);
    ui.btnRTrim->setGeometry(this->width() - 180 - 70, 65, 60, 22);
    ui.btnRPlane->setGeometry(this->width() - 240 - 75, 65, 60, 22);
    ui.planeR->setGeometry(ui.btnRPlane->x() +30, ui.btnRPlane->y() + 35, ui.btnRClose->x() - ui.btnRPlane->x(), 50);
    ui.planeG->setGeometry(ui.btnRPlane->x() +30, ui.btnRPlane->y() + 65, ui.btnRClose->x() - ui.btnRPlane->x(), 50);
    ui.planeB->setGeometry(ui.btnRPlane->x() +30, ui.btnRPlane->y() + 95, ui.btnRClose->x() - ui.btnRPlane->x(), 50);
    ui.planeColour->setGeometry(ui.btnRPlane->x() , ui.planeR->y()-4, ui.btnRClose->x()+60 - ui.btnRPlane->x(), ui.planeB->y() - ui.planeR->y()+20+10);
    ui.btnMeClose->setGeometry(this->width() - 60 - 60, 65, 60, 22);
    ui.btnMeSs->setGeometry(this->width() - 120 - 65, 65, 60, 22);
    ui.btnMeAf->setGeometry(this->width() - 180 - 70, 65, 60, 22);
    ui.btnMePoss->setGeometry(this->width() - 240 - 75, 65, 60, 22);
    ui.btnSSClose->setGeometry(this->width() - 60 - 60, 65, 60, 22);
    ui.btnOClose->setGeometry(this->width() - 60 - 60, 65, 60, 22);

    QString plane_color = QString("QLineEdit{ border: 3px rgb(%1,%2,%3); border-style:solid ;} QLineEdit{ background-color:transparent; }").arg(ui.planeR->value()).arg(ui.planeG->value()).arg(ui.planeB->value());
    ui.planeColour->setStyleSheet(plane_color);

    QString x = QString("QPushButton#btnSetBg{color:#ffffff; background:transparent;  border-radius:7px; border:1px solid #ff9912;} QPushButton#btnSetBg::hover{background-color:rgb(%1,%2,%3)}").arg(ui.sliderR->value()).arg( ui.sliderG->value()).arg( ui.sliderB->value());
    ui.btnSetBg->setStyleSheet(x);

    ui.lastItemClicked->setGeometry(ui.dock1->x() + ui.dock1->width() + 5, 60, this->width()/2, 60);
    //================================================================================

}

void gui::displayDialog() {
    dlg.show();
}

void gui::displayMainwindow()
{
    this->show();
    ui.dataInput1->setText("Please Drag .bag File Here");
    ui.progressBar->setValue(0);
    globalzzenter->outtime = false;
    dlg.hide();
}

void gui::slot_timeError(QString errstr)   
{

    if (errstr == QString("S6"))
    {
        ui.labelTime->setText(globalzzenter->maintitleQstr);
    }

    else if (errstr == QString("S3"))
    {
        ui.dataInput1->setText("Please Drag .bag File Here");
        ui.progressBar->setValue(0);
        globalzzenter->outtime = false;
        this->hide();
        dlg.show();
        dlg.datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        dlg.datasetlabel->setText("System time error. Please update the system time. ");
    }
    else
    {
        if (suanfaBOOL == false) {
            ui.dataInput1->setText("Please Drag .bag File Here");
            ui.progressBar->setValue(0);
            globalzzenter->outtime = false;
            this->hide();
            dlg.show();
            dlg.datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
            dlg.datasetlabel->setText("License expired. Please contact the supplier.");
            
        }
        
    }
}

void gui::on_btnOpen_clicked(QString a,int x) {

    if (a != "")
    {

        QString dlgTitle = "Choose File"; 
        QString filter = "All Files(*.*)";
        QString aFileName = QFileDialog::getOpenFileName(this, dlgTitle, a, filter);
        if (!aFileName.isEmpty()) {
            if (x == 1) {
                ui.dataline1->setText(aFileName);
            }
            else if (x == 2) {
                ui.dataline2->setText(aFileName);
            }
            else {
                ui.dataline3->setText(aFileName);
            }
            
        };
    }
    else {
        QMessageBox::information(NULL, "Error", "Invalid file path.");
    }
}

void gui::on_btnDisplay_clicked(QString a) {

    if (a != "")
    {
        ui.btn3dview->click(); // set up TrackballManipulator first
        filename = a.replace('/', '\\');
        if (osgView->receivePointCloud(filename) > 0) {
            initOSG();
        }
    }
    else {
        QMessageBox::information(NULL, "Error", "Invalid file path.");
    }
}

void gui::on_btnOpen_clicked1()
{
    on_btnOpen_clicked(ui.dataline1->text(),1);
}
void gui::on_btnOpen_clicked2()
{
    on_btnOpen_clicked(ui.dataline2->text(),2);
}
void gui::on_btnOpen_clicked3()
{
    on_btnOpen_clicked(ui.dataline3->text(),3);
}

void gui::on_btnDisp_clicked1()
{
    ui.btn3dview->click();
    //on_btnDisplay_clicked(ui.dataline1->text());
}
void gui::on_btnDisp_clicked2()
{
    on_btnDisplay_clicked(ui.dataline2->text());
}
void gui::on_btnDisp_clicked3()
{
    on_btnDisplay_clicked(ui.dataline3->text());
}


void gui::readHistoricalData()
{
    currentpath = dir.currentPath();
    aa = "..\\setup.bat";
    //aa = currentpath+"/setup.bat";
    QString fileName = currentpath+"/historical_data.txt";

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream txtInput(&f);
    QString lineStr;
    int Noi = -1;
    while (!txtInput.atEnd())
    {
        if (Noi == -1)
        {
            Noi = 0;
        }
        lineStr = txtInput.readLine();
        if (Noi == 0)
        {
            HistoricalDir1 = lineStr;
            ui.dataline1->setText(lineStr);
        }
        if (Noi == 1)
        {
            HistoricalDir2 = lineStr;
            ui.dataline2->setText(lineStr);
        }
        if (Noi == 2)
        {
            HistoricalDir3 = lineStr;
            ui.dataline3->setText(lineStr);
        }

        Noi++;
    }
    f.close();

    // start ros========================================== 

    //processCore1 = new QProcess;
    //processCore1->start("cmd.exe");
    //processCore1->write(QString(aa + "\r\n").toUtf8());
    //processCore1->write(QString("roscore \r\n").toUtf8());
    //===================================================
}
gui::~gui()
{
    qDebug() << "~gui()";
}

// loading notification =====================================
void gui::initLoading() {
    QPalette label_pe;
    QFont ft;
    ft.setPointSize(22);
    ft.setBold(true);
    QColor color;
    color.setRgb(255, 152, 0);
    label_pe.setColor(QPalette::WindowText, color);
    loadinglabel = new QLabel(this);
    loadinglabel->setText("Loading..");
    loadinglabel->setPalette(label_pe);
    loadinglabel->setFont(ft);
    loadinglabel->hide();

}

void gui::startLoading()
{
    loadinglabel->setGeometry(this->width() / 2 - 80, 0, 200, 30);
    loadinglabel->raise();
    loadinglabel->setVisible(true);

    QEventLoop loop;
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();
}


void gui::stopLoading()
{
    loadinglabel->hide();
}
//=========================================================

string gui::getTempFolder() {
    int a;
    string temppath;
    string filePath = QDir::currentPath().toStdString();

    a = filePath.rfind("/");

    filePath = filePath.substr(0, a);


    std::cout << filePath + "\n";
    temppath = filePath + "/temp";
    osgView->osgAlgo->wenjian(temppath);

    return temppath;
}
