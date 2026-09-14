/********************************************************************
 * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
 *      for processing and rendering 3D data.
 *      Journal of Open Source Software, 6(64), 3255, 2021.
 * ------------------------------------------------------------------
 *
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************/


#include "../include/easy3d_inc/dialog/dialog_point_cloud_simplification.h"

#include <easy3d/kdtree/kdtree_search_eth.h>
#include <easy3d/algo/point_cloud_simplification.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/util/logging.h>
#include <easy3d/util/stop_watch.h>
#include <QButtonGroup>
#include <QIntValidator>

#include "../include/easy3d_inc/viewer.h"
#include "../include/mainwindow.h"

using namespace easy3d;


DialogPointCloudSimplification::DialogPointCloudSimplification(mainwindow* window)
    : Dialog(window), kdtree_(nullptr) {
    setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // default value
    lineEditDistanceThreshold->setText("0.01");
    lineEditAverageSpacing->setText("unknown");

    connect(buttonComputeAvgSpacing, SIGNAL(clicked()), this, SLOT(computeAvgSpacing()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(queryButton, SIGNAL(clicked()), this, SLOT(query()));
}


DialogPointCloudSimplification::~DialogPointCloudSimplification() {
    delete kdtree_;
}


void DialogPointCloudSimplification::closeEvent(QCloseEvent* e) {
    points_to_remove_.clear();
    window_->statusBar()->clearMessage();
    window_->log->set_log(log_type::Info, QString(tr("Down sampling done.")).toStdString(), window_->getLogWidget());
    QDialog::closeEvent(e);
}


void DialogPointCloudSimplification::showEvent(QShowEvent* e) {
    auto cloud = dynamic_cast<PointCloud*>(viewer_->currentModel());

    if (cloud) {

        window_->statusBar()->showMessage(QString::fromStdString(cloud->name()));

        lineEditDistanceThreshold->setDisabled(false);
        checkBoxUniform->setDisabled(false);
        lineEditAverageSpacing->setDisabled(false);
        buttonComputeAvgSpacing->setDisabled(false);

        window_->log->set_log(log_type::Info, QString(tr("'Query' before 'Apply' at all time.")).toStdString(), window_->getLogWidget());
    }


    QDialog::showEvent(e);
}

void DialogPointCloudSimplification::constructKdTree() {
    auto cloud = dynamic_cast<PointCloud*>(viewer_->currentModel());
    if (cloud) {
        delete kdtree_;
        kdtree_ = new KdTreeSearch_ETH(cloud);
    }
}


void DialogPointCloudSimplification::computeAvgSpacing() {
    auto cloud = dynamic_cast<PointCloud*>(viewer_->currentModel());
    if (cloud) {
        constructKdTree();

        if (kdtree_) {
            float as = PointCloudSimplification::average_space(cloud, kdtree_, 6);
            lineEditAverageSpacing->setText(QString("%1").arg(as));
            show();
        }
    }
}


void DialogPointCloudSimplification::query() {
    auto cloud = dynamic_cast<PointCloud*>(viewer_->currentModel());
    if (cloud) {

        float threshold = lineEditDistanceThreshold->text().toFloat();
        if (checkBoxUniform->isChecked()) {
            if (!kdtree_)
                constructKdTree();
            points_to_remove_ = PointCloudSimplification::uniform_simplification(cloud, threshold, kdtree_);
        }
        else
            points_to_remove_ = PointCloudSimplification::grid_simplification(cloud, threshold);
        
       
        QString temp = tr("Queried. ") + QString::number(cloud->n_vertices() - points_to_remove_.size()) + tr(" points will remain.");
        window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
    }
}


void DialogPointCloudSimplification::apply() {
    auto cloud = dynamic_cast<PointCloud*>(viewer_->currentModel());
    if (cloud) {
        if (points_to_remove_.empty()) {
            window_->log->set_log(log_type::Warning, QString(tr("No points have been marked. Please query for points at first.")).toStdString(), window_->getLogWidget());
            return;
        }

        auto old_num = cloud->n_vertices();
        for (auto v : points_to_remove_)
            cloud->delete_vertex(v);

        cloud->collect_garbage();

        if (!points_to_remove_.empty()) {
            points_to_remove_.clear();
            auto new_num = cloud->n_vertices();

            QString temp = QString::number(old_num - new_num) + tr(" points removed. ")+ QString::number(new_num) + tr(" points remain.");
            window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());

            cloud->renderer()->update();
            window_->updateUi();
            viewer_->update();
        }

        delete kdtree_;
        kdtree_ = nullptr;
    }
}
