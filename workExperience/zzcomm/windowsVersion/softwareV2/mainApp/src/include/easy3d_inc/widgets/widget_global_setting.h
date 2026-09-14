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


#ifndef WIDGET_GLOBAL_SETTING_H
#define WIDGET_GLOBAL_SETTING_H

#include <QWidget>

#include "ui_widget_global_setting.h"
#include "../../resources/language/linguist.h"
namespace Ui {
    class WidgetGlobalSetting;
}

namespace easy3d {
    class ClippingPlane;
}

class PaintCanvas;
class WidgetTrianglesDrawable;
class WidgetCheckerSphere;

class WidgetGlobalSetting : public QWidget {
    Q_OBJECT

public:
    explicit WidgetGlobalSetting(QWidget* parent);
    ~WidgetGlobalSetting() override;

    Ui::WidgetGlobalSetting* getUi() { linguist* lang = linguist::getLinguistInstance(); lang->changeLanguage(Language::zh_); ui_->retranslateUi(this); return ui_; }

    void setEyeDomeLighting(bool);

    void setSphereColor(int);           // 0 = classic , 1 = dark

    void changeLanguage(Language);

private slots:
    void setEnableClippingPlane(bool);
    void setClippingPlaneVisible(bool);
    void recenterClippingPlane();
    void setClippingPlaneColor();
    void setSSAOAlgorithm(int);
    void setSSAORadius(int);
    void setSSAOBias(int);
    void setShadow(bool);
    void setLightDistance(int);
    void setShadowSmoothPattern(int);
    void setShadowSoftness(int);
    void setShadowDarkness(int);
    void setImposterShadows(bool);

signals:
    // tell mainwindow ui 
    void shadow_toggled(bool); 
    void ssao_on(bool);
    void clipping_plane_on(bool);
    // ==================

private:
    void disableUnavailableOptions();

    QColor prev_bc;

private:
    Ui::WidgetGlobalSetting* ui_;
    PaintCanvas* viewer_;

    // for communication between widgets (i.e., keep ui updated)
    friend class mainwindow;
    WidgetTrianglesDrawable* widgetTrianglesDrawable_;
};


#endif // WIDGET_GLOBAL_SETTING_H
