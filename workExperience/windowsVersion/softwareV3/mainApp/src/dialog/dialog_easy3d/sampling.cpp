#include "../../include/include_easy3d/dialog/sampling.h"
#include "../../include/include_gui/mainApp.h"
#include "../../include/include_gui/widget_newProcess.h"
#include "../../include/include_gui/gcp.h"
#include "../../include/include_easy3d/viewer.h"
#include <easy3d/kdtree/kdtree_search_eth.h>
#include <easy3d/algo/point_cloud_simplification.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/util/logging.h>
#include <easy3d/util/stop_watch.h>
#include <easy3d/fileio/point_cloud_io.h>
#include <QButtonGroup>
#include <QIntValidator>

using namespace easy3d;

sampling::sampling(mainApp*parent)
	: QDialog(parent), kdtree_(nullptr)
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    /*this->setFixedHeight(250);*/
    this->setFixedWidth(400);
	setupUi(this);
	window_ = parent;
	init();
}

sampling::~sampling()
{
	delete kdtree_;
}
void sampling::init() {
    viewer_ = window_->viewer();
	// default value
	doubleSpinBoxDistThreshold->setValue(0.01);
	lineEditAvgSpacing->setText("unknown");
	checkBoxUniform->setChecked(1);

	connect(buttonComputeAvgSpacing, SIGNAL(clicked()), this, SLOT(computeAvgSpacing()), Qt::UniqueConnection);
	connect(btnApply, SIGNAL(clicked()), this, SLOT(apply()), Qt::UniqueConnection);
	connect(btnQuery, SIGNAL(clicked()), this, SLOT(query()), Qt::UniqueConnection);
    connect(btnSor, SIGNAL(clicked()), this, SLOT(sor()), Qt::UniqueConnection);

}
void sampling::closeEvent(QCloseEvent* event) {
	std::cout << "sampling close" << std::endl;
	points_to_remove_.clear();
	emit(_closed());
}

void sampling::showEvent(QShowEvent* e) {
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer_->currentModel());

    if (cloud) {

        //window_->statusBar()->showMessage(QString::fromStdString(cloud->name()));

        doubleSpinBoxDistThreshold->setDisabled(false);
        checkBoxUniform->setDisabled(false);
        lineEditAvgSpacing->setDisabled(false);
        buttonComputeAvgSpacing->setDisabled(false);

        //window_->log->set_log(log_type::Info, QString(tr("'Query' before 'Apply' at all time.")).toStdString(), window_->getLogWidget());
    }


    QDialog::showEvent(e);
}

void sampling::constructKdTree() {
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer_->currentModel());
    if (cloud) {
        delete kdtree_;
        kdtree_ = new KdTreeSearch_ETH(cloud);
    }
}


void sampling::computeAvgSpacing() {
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer_->currentModel());
    if (cloud) {
        constructKdTree();

        if (kdtree_) {
            float as = PointCloudSimplification::average_space(cloud, kdtree_, 6);
            lineEditAvgSpacing->setText(QString("%1").arg(as));
            show();
        }
    }
}

void sampling::query() {
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer_->currentModel());
    if (cloud) {

        float threshold = doubleSpinBoxDistThreshold->value();
        if (checkBoxUniform->isChecked()) {
            if (!kdtree_)
                constructKdTree();
            points_to_remove_ = PointCloudSimplification::uniform_simplification(cloud, threshold, kdtree_);
        }
        else
            points_to_remove_ = PointCloudSimplification::grid_simplification(cloud, threshold);


        //QString temp = tr("Queried. ") + QString::number(cloud->n_vertices() - points_to_remove_.size()) + tr(" points will remain.");
        //window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
    }
}

void sampling::apply() {
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer_->currentModel());
    if (cloud) {
        if (points_to_remove_.empty()) {
            //window_->log->set_log(log_type::Warning, QString(tr("No points have been marked. Please query for points at first.")).toStdString(), window_->getLogWidget());
            return;
        }

        auto old_num = cloud->n_vertices();
        for (auto v : points_to_remove_)
            cloud->delete_vertex(v);

        cloud->collect_garbage();

        if (!points_to_remove_.empty()) {
            points_to_remove_.clear();
            auto new_num = cloud->n_vertices();

            //QString temp = QString::number(old_num - new_num) + tr(" points removed. ") + QString::number(new_num) + tr(" points remain.");
            //window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());

            cloud->renderer()->update();
            window_->updateUi();
            viewer_->update();
        }

        delete kdtree_;
        kdtree_ = nullptr;
    }
}

void sampling::sor() {
    gcp* g = new gcp(new widget_newProcess(window_));
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer_->currentModel());
    easy3d::PointCloudIO::save(QString("..\\temp\\temp.ply").toStdString(), cloud);

    // use pcl to sor the saved .ply file
    //pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZRGB>);
    //pcl::io::loadPLYFile("..\\temp\\temp.ply", *cloud0);
    //std::cout << "test1" << std::endl;
    //pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud01(new pcl::PointCloud<pcl::PointXYZRGB>);
    //pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB> outrem;
    //outrem.setInputCloud(cloud0);
    //outrem.setMeanK(spinBox->value());
    //outrem.setStddevMulThresh(doubleSpinBox->value());
    //outrem.filter(*cloud01);


    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZI>);
    //pcl::io::loadPLYFile("E:\\vsTest\\ConsoleApplication1\\ConsoleApplication1\\mergedMLS.ply", *cloud0);
    pcl::io::loadPLYFile("..\\temp\\temp.ply", *cloud0);
    std::cout << "test1" << std::endl;
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud01(new pcl::PointCloud<pcl::PointXYZI>);
    cloud01 = g->sor(cloud0, spinBox->value(), doubleSpinBox->value());
    pcl::io::savePLYFileBinary("..\\temp\\temp1.ply", *cloud01);
    std::cout << "test2" << std::endl;

    // add to easy3d
    window_->open("..\\temp\\temp1.ply");



}