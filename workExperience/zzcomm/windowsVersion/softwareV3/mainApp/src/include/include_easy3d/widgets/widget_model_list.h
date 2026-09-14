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


#ifndef WIDGET_MODEL_LIST_H
#define WIDGET_MODEL_LIST_H

#include <QTreeWidget>

namespace easy3d {
    class Model;
    class Drawable;
}

class mainApp;
class PaintCanvas;

// tree dock in the mainwindow ui
class WidgetModelList : public QTreeWidget {
    Q_OBJECT

public:

    explicit WidgetModelList(QWidget* parent = nullptr);
    ~WidgetModelList() override = default;

    void init(mainApp* w); // set headers 

    // single model
    void addModel(easy3d::Model* obj, bool make_current); // when a new model is dragged in

    void mergeModels(const std::vector<easy3d::Model*>& objs); // select n>1 models -> merge together

    void decomposeModel(easy3d::Model* obj); // decompose mesh model

    void updateModelList(); // refresh tree dock 

    void prepareContextMenu(QMenu* menu); // right click menu

    void updateDrawableVisibility(easy3d::Drawable* d); // the given drawable
    void updateDrawableVisibilities();  // all drawables
    void updateVisibilities();  // all models and drawables

    void deleteFromMainwindow(); // mainwindow ui 'delete' toolbutton

private Q_SLOTS:

    void duplicateCurrent(); // right click -> copy
    void mergeSelected(); // select n>1 models -> merge together
    void deleteSelected(); // delete item from tree dock
    void decomposeSelected(); // decompose mesh model

    void showSelected(); // right click -> show
    void hideSelected(); // right click -> hide

    void modelItemPressed(QTreeWidgetItem* current, int column);
    void modelItemSelectionChanged();

    void showContextMenu(const QPoint& p); // right click on the item

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    PaintCanvas* viewer();

private:
    mainApp* mainWindow_;
    QMenu* popupMenu_;
};

#endif