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

#include "../../include/include_easy3d/widgets/widget_model_list.h"
#include "../../include/include_easy3d/widgets/widget_drawable.h"
#include "../../include/include_easy3d/widgets/widget_global_setting.h"
#include "../../include/include_gui/mainApp.h"
#include "../../include/include_easy3d/viewer.h"

 //#include "widgets/widget_drawable_triangles.h"


#include <QColorDialog>

#include <easy3d/core/model.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/renderer/soft_shadow.h>
#include <easy3d/renderer/ambient_occlusion.h>
#include <easy3d/renderer/clipping_plane.h>
#include <easy3d/renderer/camera.h>
#include <easy3d/renderer/manipulator.h>
#include <easy3d/renderer/manipulated_frame.h>
#include <easy3d/util/setting.h>

#include <ui_widget_global_setting.h>


using namespace easy3d;

WidgetGlobalSetting::WidgetGlobalSetting(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::WidgetGlobalSetting)
    , widgetTrianglesDrawable_(nullptr)
    //, prev_bc(parent->palette().color(parent->backgroundRole()))
{
    ui_->setupUi(this);
    viewer_ = dynamic_cast<mainApp*>(parent)->viewer();

    //// SSAO
    //ui_->comboBoxSSAOAlgorithm->addItem("None");
    //ui_->comboBoxSSAOAlgorithm->addItem("Classic");
    //ui_->comboBoxSSAOAlgorithm->setCurrentIndex(0);
    //ui_->horizontalSliderSSAORadius->setValue(setting::effect_ssao_radius * 100);
    //ui_->horizontalSliderSSAOBias->setValue(setting::effect_ssao_bias * 100);

    //// shadow
    //ui_->checkBoxShadow->setChecked(setting::effect_shadow_enabled);
    //ui_->comboBoxShadowSmoothPattern->addItem("Poisson  25 x 25 ");
    //ui_->comboBoxShadowSmoothPattern->addItem("Poisson  32 x 64 ");
    //ui_->comboBoxShadowSmoothPattern->addItem("Poisson  64 x 128");
    //ui_->comboBoxShadowSmoothPattern->addItem("Poisson 100 x 100");
    //ui_->comboBoxShadowSmoothPattern->addItem("Regular  49 x 225");
    //ui_->comboBoxShadowSmoothPattern->setCurrentIndex(2);
    //connect(ui_->comboBoxShadowSmoothPattern, SIGNAL(currentIndexChanged(int)), this, SLOT(setShadowSmoothPattern(int)));

    //ui_->horizontalSliderShadowLightDistance->setValue(setting::effect_shadow_light_distance);
    //ui_->horizontalSliderShadowSoftness->setValue(setting::effect_shadow_softness * 100);
    //ui_->horizontalSliderShadowDarkness->setValue(setting::effect_shadow_darkness * 100);

    //----------------------------------------------

    QPalette::ColorRole role = parent->backgroundRole();
    QColor bc = parent->palette().color(role);
    prev_bc = bc;
    //ui_->checkerSphere->setBackgroundColor(bc);
    //ui_->checkerSphere->setBackgroundColor(QColor(1, 1, 0));
    connect(ui_->checkerSphere, SIGNAL(lightPositionChanged()), viewer_, SLOT(update()));

    connect(ui_->checkBoxClippingPlaneEnable, SIGNAL(toggled(bool)), this, SLOT(setEnableClippingPlane(bool)));
    connect(ui_->checkBoxClippingPlaneVisible, SIGNAL(toggled(bool)), this, SLOT(setClippingPlaneVisible(bool)));
    connect(ui_->toolButtonRecenterClippingPlane, SIGNAL(clicked()), this, SLOT(recenterClippingPlane()));
    connect(ui_->toolButtonClippingPlaneColor, SIGNAL(clicked()), this, SLOT(setClippingPlaneColor()));


    // visible
    ui_->checkBoxClippingPlaneVisible->setChecked(true);
    // default color
    const auto& c = ClippingPlane::instance()->color();
    QPixmap pixmap(ui_->toolButtonClippingPlaneColor->size());
    pixmap.fill(
        QColor(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255)));
    ui_->toolButtonClippingPlaneColor->setIcon(QIcon(pixmap));

    //connect(ui_->comboBoxSSAOAlgorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(setSSAOAlgorithm(int)));
    //connect(ui_->horizontalSliderSSAORadius, SIGNAL(valueChanged(int)), this, SLOT(setSSAORadius(int)));
    //connect(ui_->horizontalSliderSSAOBias, SIGNAL(valueChanged(int)), this, SLOT(setSSAOBias(int)));

    //connect(ui_->checkBoxShadow, SIGNAL(toggled(bool)), this, SLOT(setShadow(bool)));

    //connect(ui_->horizontalSliderShadowLightDistance, SIGNAL(valueChanged(int)), this, SLOT(setLightDistance(int)));
    //connect(ui_->horizontalSliderShadowSoftness, SIGNAL(valueChanged(int)), this, SLOT(setShadowSoftness(int)));
    //connect(ui_->horizontalSliderShadowDarkness, SIGNAL(valueChanged(int)), this, SLOT(setShadowDarkness(int)));




    //ui_->line_5->hide();
    //ui_->line_4->hide();
    //ui_->labelSSAO->hide();
    //ui_->comboBoxSSAOAlgorithm->hide();
    //ui_->labelSSAORadius->hide();
    //ui_->horizontalSliderSSAORadius->hide();
    //ui_->labelSSAOBias->hide();
    //ui_->horizontalSliderSSAOBias->hide();
    //ui_->line_3->hide();
    //ui_->labelShadow->hide();
    //ui_->checkBoxShadow->hide();
    //ui_->labelShadowSmoothPattern->hide();
    //ui_->comboBoxShadowSmoothPattern->hide();
    //ui_->labelShadowLightDistance->hide();
    //ui_->horizontalSliderShadowLightDistance->hide();
    //ui_->labelShadowSoftness->hide();
    //ui_->horizontalSliderShadowSoftness->hide();
    //ui_->labelShadowDarkness->hide();
    //ui_->horizontalSliderShadowDarkness->hide();
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);
    //this->setMaximumHeight(200);

}


WidgetGlobalSetting::~WidgetGlobalSetting()
{
    delete ui_;
}


void WidgetGlobalSetting::setEnableClippingPlane(bool b) {
    ClippingPlane::instance()->set_enabled(b);
    // connect the manipulator's signal to the viewer's update function to automatically update rendering.
    ClippingPlane::instance()->manipulator()->frame()->modified.connect(viewer_,
        static_cast<void (PaintCanvas::*)(void)>(&PaintCanvas::update));
    if (b) {
        static bool init_size = false;
        if (!init_size) {
            recenterClippingPlane();
            init_size = true;
        }
    }
    viewer_->update();
    disableUnavailableOptions();

    emit(clipping_plane_on(b));

}


void WidgetGlobalSetting::setClippingPlaneVisible(bool b) {
    ClippingPlane::instance()->set_visible(b);
    viewer_->update();
    disableUnavailableOptions();
}


void WidgetGlobalSetting::recenterClippingPlane() {
    Box3 box;
    for (auto m : viewer_->models()) {
        if (m->renderer()->is_visible())
            box.grow(m->bounding_box(true));
    }

    if (box.is_valid()) {
        ClippingPlane::instance()->fit_scene(box.center(), box.radius());
        viewer_->camera()->setSceneBoundingBox(box.min_point(), box.max_point());
        viewer_->update();
    }
}


void WidgetGlobalSetting::setClippingPlaneColor() {
    const auto& c = ClippingPlane::instance()->color();
    QColor orig(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255));
    const QColor& color = QColorDialog::getColor(orig, this);
    if (color.isValid()) {
        const vec4 new_color(color.redF(), color.greenF(), color.blueF(), c.a);
        ClippingPlane::instance()->set_color(new_color);
        viewer_->update();

        QPixmap pixmap(ui_->toolButtonClippingPlaneColor->size());
        pixmap.fill(color);
        ui_->toolButtonClippingPlaneColor->setIcon(QIcon(pixmap));
    }
}


//void WidgetGlobalSetting::setSSAOAlgorithm(int algo) {
//    viewer_->enableSsao(algo != 0);
//
//    if (algo != 0 && ui_->checkBoxShadow->isChecked())
//        ui_->checkBoxShadow->setChecked(false);
//
//    viewer_->update();
//    disableUnavailableOptions();
//
//
//    if (algo != 0)
//        emit(ssao_on(true));
//    else
//        emit(ssao_on(false));
//}
//
//
//void WidgetGlobalSetting::setSSAORadius(int v) {
//    if (viewer_->ssao()) {
//        viewer_->ssao()->set_radius(v / 100.0f);
//        viewer_->update();
//    }
//}
//
//
//void WidgetGlobalSetting::setSSAOBias(int v) {
//    if (viewer_->ssao()) {
//        viewer_->ssao()->set_bias(v / 100.0f);
//        viewer_->update();
//    }
//}


void WidgetGlobalSetting::setEyeDomeLighting(bool b) {
    //if (b && ui_->checkBoxShadow->isChecked())
    //    ui_->checkBoxShadow->setChecked(false);  // shadow and EDL cannot co-exist

    viewer_->enableEyeDomeLighting(b);
    viewer_->update();
}


//void WidgetGlobalSetting::setShadow(bool b) {
//
//    emit(shadow_toggled(b));
//
//    if (b && ui_->comboBoxSSAOAlgorithm->currentIndex() != 0)
//        ui_->comboBoxSSAOAlgorithm->setCurrentIndex(0);
//
//    viewer_->enableShadow(b);
//    viewer_->update();
//
//    disableUnavailableOptions();
//}
//
//
//void WidgetGlobalSetting::setLightDistance(int d) {
//    viewer_->shadow()->set_light_distance(d);
//    viewer_->update();
//}
//
//
//void WidgetGlobalSetting::setShadowSmoothPattern(int v) {
//    auto shadow = dynamic_cast<SoftShadow*>(viewer_->shadow());
//    if (shadow) {
//        shadow->set_sample_pattern(SoftShadow::SamplePattern(v));
//        viewer_->update();
//    }
//}
//
//
//void WidgetGlobalSetting::setShadowSoftness(int v) {
//    auto shadow = dynamic_cast<SoftShadow*>(viewer_->shadow());
//    if (shadow) {
//        shadow->set_softness(static_cast<float>(v) / 100.0f);
//        viewer_->update();
//    }
//}
//
//
//void WidgetGlobalSetting::setShadowDarkness(int v) {
//    viewer_->shadow()->set_darkness(static_cast<float>(v) / 100.0f);
//    viewer_->update();
//}


//void WidgetGlobalSetting::setImposterShadows(bool) {
//
//}


void WidgetGlobalSetting::disableUnavailableOptions() {

    bool visible;

    // clipping plane
    visible = ui_->checkBoxClippingPlaneEnable->isChecked();
    ui_->toolButtonRecenterClippingPlane->setEnabled(visible);
    ui_->labelClippingPlaneVisible->setEnabled(visible);
    ui_->checkBoxClippingPlaneVisible->setEnabled(visible);
    bool can_change_clipping_plane_color = visible && (ui_->checkBoxClippingPlaneVisible->isChecked());
    ui_->toolButtonClippingPlaneColor->setEnabled(can_change_clipping_plane_color);


    //// SSAO
    //visible = (ui_->comboBoxSSAOAlgorithm->currentIndex() != 0);
    //ui_->labelSSAORadius->setEnabled(visible);
    //ui_->horizontalSliderSSAORadius->setEnabled(visible);
    //ui_->labelSSAOBias->setEnabled(visible);
    //ui_->horizontalSliderSSAOBias->setEnabled(visible);

    //// shadow
    //visible = ui_->checkBoxShadow->isChecked();
    //ui_->labelShadowSmoothPattern->setEnabled(visible);
    //ui_->comboBoxShadowSmoothPattern->setEnabled(visible);
    //ui_->labelShadowLightDistance->setEnabled(visible);
    //ui_->horizontalSliderShadowLightDistance->setEnabled(visible);
    //ui_->labelShadowSoftness->setEnabled(visible);
    //ui_->horizontalSliderShadowSoftness->setEnabled(visible);
    //ui_->labelShadowDarkness->setEnabled(visible);
    //ui_->horizontalSliderShadowDarkness->setEnabled(visible);

    update();
    qApp->processEvents();
}

void WidgetGlobalSetting::setSphereColor(int a) {

    ui_->checkerSphere->setBackgroundColor(prev_bc);
    if (a == 1) {
        ui_->toolButtonRecenterClippingPlane->setIcon(QIcon(":resources/icons/recenter1.png"));

        //ui_->checkerSphere->setBackgroundColor(QColor(0.0, 0.0, 1.0, 1.0));
        //std::cout << ui_->checkerSphere->backgroundColor().redF() << " " << ui_->checkerSphere->backgroundColor().greenF() << " " << ui_->checkerSphere->backgroundColor().blueF() << std::endl;
        //std::cout << "dark \n";
        //update();
    }
    else {
        ui_->toolButtonRecenterClippingPlane->setIcon(QIcon(":resources/icons/recenter.png"));


        //std::cout << prev_bc.redF() << " "<< prev_bc.greenF() << " "<<prev_bc.blueF() << std::endl;
        //std::cout << "classic \n";
    }
}

//void WidgetGlobalSetting::changeLanguage(Language a)
//{
//    linguist* lang = linguist::getLinguistInstance();
//    if (a == Language::en_)
//    {
//        lang->changeLanguage(Language::en_);
//    }
//    else {
//        lang->changeLanguage(Language::zh_);
//    }
//    ui_->retranslateUi(this);
//}
