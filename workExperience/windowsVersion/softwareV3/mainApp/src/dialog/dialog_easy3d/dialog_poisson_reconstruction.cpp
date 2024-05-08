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

#include "../../include/include_easy3d/dialog/dialog_poisson_reconstruction.h"
#include "../../include/include_easy3d/viewer.h"
#include "../../include/include_gui/mainApp.h"

#include <easy3d/core/point_cloud.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/algo/point_cloud_poisson_reconstruction.h>
#include <easy3d/util/file_system.h>
#include <easy3d/util/logging.h>
#include <easy3d/util/stop_watch.h>

using namespace easy3d;

DialogPoissonReconstruction::DialogPoissonReconstruction(mainApp* window) :
    Dialog(window) {

    setupUi(this);
    //layout()->setSizeConstraint(QLayout::SetFixedSize);
    //setFixedWidth(800);
    // default value
    default_octree_depth_ = 8;
    default_samples_per_node_ = 1;
    default_trim_value_ = 6.0;
    default_area_ratio_ = 0.001;

    density_attr_name_ = "v:density";

    resetParameters();
    showHint(false);

    connect(pushButtonHelp, SIGNAL(toggled(bool)), this, SLOT(showHint(bool)));
    connect(pushButtonDefault, SIGNAL(clicked()), this, SLOT(resetParameters()));
    connect(pushButtonReconstruct, SIGNAL(clicked()), this, SLOT(reconstruct()));
    connect(pushButtonTrim, SIGNAL(clicked()), this, SLOT(trim()));

}

void DialogPoissonReconstruction::resetParameters() {
    spinBoxOctreeDepth->setValue(default_octree_depth_);
    spinBoxSamplesPerNode->setValue(default_samples_per_node_);

    doubleSpinBoxTrimValue->setValue(default_trim_value_);
}


void DialogPoissonReconstruction::showHint(bool b) {
    if (b)
        widgetHint->show();
    else
        widgetHint->hide();
}

easy3d::StopWatch w4;
void DialogPoissonReconstruction::reconstruct() {
    auto cloud = dynamic_cast<PointCloud*>(viewer_->currentModel());
    if (cloud) {
        int octree_depth = spinBoxOctreeDepth->value();
        double samples_per_node = spinBoxSamplesPerNode->value();

        //window_->log->set_log(log_type::Info, QString(tr("Reconstructing..")).toStdString(), window_->getLogWidget());
        QApplication::processEvents(); // show loading dialog
        w4.restart();

        PoissonReconstruction recon;
        recon.set_depth(octree_depth);
        recon.set_samples_per_node(samples_per_node);

        SurfaceMesh* mesh = recon.apply(cloud, density_attr_name_);
        if (mesh) {
            const std::string& name = file_system::name_less_extension(cloud->name()) + "_poisson_reconstruction.ply";
            mesh->set_name(name);
            viewer_->addModel(mesh);
            window_->updateUi();
            viewer_->update();

            QString temp = QString(tr("Time spent for reconstruction, %1.")).arg(QString::fromStdString(w4.time_string()));
            //window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
            window_->setStatusWTime(temp.toStdString());
        }
        else {
            //window_->log->set_log(log_type::Error, QString(tr("Normal information does not exist for Poisson surface reconstruction. Go to 'Point Cloud' -> 'Estimate Normal'.")).toStdString(), window_->getLogWidget());
            window_->setStatusWTime(QString(tr("Normal information does not exist for Poisson surface reconstruction.")).toStdString());
        }
    }
    else {
        //window_->log->set_log(log_type::Warning, QString(tr("Make sure point cloud is selected.")).toStdString(), window_->getLogWidget());
    }
}

void DialogPoissonReconstruction::trim() {
    auto mesh = dynamic_cast<SurfaceMesh*>(viewer_->currentModel());
    if (mesh) {
        double trim_value = doubleSpinBoxTrimValue->value();
        double area_ratio = default_area_ratio_;
        bool triangulate = false; // I can do it using my algorithm :-)

        auto density = mesh->vertex_property<float>(density_attr_name_);
        if (!density) {
            //window_->log->set_log(log_type::Warning, QString(tr("No property \'density\' for trimming.")).toStdString(), window_->getLogWidget());
            return;
        }

        float min_density = FLT_MAX;
        float max_density = -FLT_MAX;
        for (auto v : mesh->vertices()) {
            float value = density[v];
            min_density = std::min(min_density, value);
            max_density = std::max(max_density, value);
        }

        if (trim_value <= min_density || trim_value >= max_density) {

            QString temp = QString(tr("Trim value (%1) out of density range [%2, %3]").arg(trim_value).arg(min_density).arg(max_density));
            //window_->log->set_log(log_type::Warning, temp.toStdString(), window_->getLogWidget());
            window_->setStatusWTime(temp.toStdString());
            return;
        }

        //window_->log->set_log(log_type::Info, QString(tr("Extracting..")).toStdString(), window_->getLogWidget());
        QApplication::processEvents(); // show loading dialog
        w4.restart();

        SurfaceMesh* trimmed_mesh = PoissonReconstruction::trim(mesh, density_attr_name_, trim_value, area_ratio,
            triangulate);
        if (trimmed_mesh) {
            const std::string& name = file_system::name_less_extension(mesh->name()) + "_trimmed.ply";
            trimmed_mesh->set_name(name);
            viewer_->addModel(trimmed_mesh);
            window_->updateUi();
            viewer_->update();

            QString temp = QString(tr("Time spent for trimming, %1.")).arg(QString::fromStdString(w4.time_string()));
            //window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
            window_->setStatusWTime(temp.toStdString());
        }
    }
    else {
        //window_->log->set_log(log_type::Warning, QString(tr("Make sure mesh model is selected.")).toStdString(), window_->getLogWidget());
    }
}

void DialogPoissonReconstruction::closeEvent(QCloseEvent* event)
{
    std::cout << "mesh close" << std::endl;
    emit(_closed());
}

