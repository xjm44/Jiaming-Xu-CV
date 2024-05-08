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

#include "../include/easy3d_inc/widgets/widget_model_list.h"
#include "../include/mainwindow.h"
#include "../include/easy3d_inc/viewer.h"
//#include "walk_through.h"

#include <easy3d/core/graph.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/point_cloud.h>
#include <easy3d/core/poly_mesh.h>
#include <easy3d/core/surface_mesh_builder.h>
#include <easy3d/algo/surface_mesh_components.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/renderer/manipulator.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/drawable_triangles.h>
#include <easy3d/util/file_system.h>

#include <QMenu>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QMouseEvent>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <easy3d/util/progress.h>


using namespace easy3d;

// parent item
class ModelItem : public QTreeWidgetItem {
public:
    ModelItem(QTreeWidget* parent, Model* m) : QTreeWidgetItem(parent), model_(m) {
        for (int i = 0; i < parent->columnCount(); ++i) {
            QTreeWidgetItem::setTextAlignment(i, Qt::AlignLeft);
        }
    }

    ~ModelItem() override = default;

    Model* model() { return model_; }

    void setIcon(int column) {
        if (dynamic_cast<SurfaceMesh*>(model())) {
            static QIcon iconMesh(QString::fromUtf8(":/resources/icons/mesh.png"));
            QTreeWidgetItem::setIcon(column, iconMesh);
        }
        else if (dynamic_cast<PointCloud*>(model())) {
            static QIcon iconPointCloud(QString::fromUtf8(":/resources/icons/point_cloud.png"));
            QTreeWidgetItem::setIcon(column, iconPointCloud);
        }
        else if (dynamic_cast<Graph*>(model())) {
            static QIcon iconGraph(QString::fromUtf8(":/resources/icons/graph.png"));
            QTreeWidgetItem::setIcon(column, iconGraph);
        }
        else if (dynamic_cast<PolyMesh*>(model())) {
            static QIcon iconTetrahedra(QString::fromUtf8(":/resources/icons/tetrahedra.png"));
            QTreeWidgetItem::setIcon(column, iconTetrahedra);
        }
    }


    void setVisible(int column, bool visible) {
        static QIcon iconShow(QString::fromUtf8(":/resources/icons/show.png"));
        static QIcon iconHide(QString::fromUtf8(":/resources/icons/hide.png"));
        if (visible)
            QTreeWidgetItem::setIcon(column, iconShow);
        else
            QTreeWidgetItem::setIcon(column, iconHide);

        int num = childCount();
        for (int i = 0; i < num; ++i)
            this->child(i)->setDisabled(!visible);
    }

    void highlight(bool b) {
        for (int i = 0; i < columnCount(); ++i)
            QTreeWidgetItem::setBackground(i, b ? QColor(250, 171, 67) : QBrush());
    }

private:
    Model* model_;
};

// child item
class DrawableItem : public QTreeWidgetItem {
public:
    DrawableItem(ModelItem* parent, Drawable* d) : QTreeWidgetItem(parent), drawable_(d) {
        for (int i = 0; i < parent->columnCount(); ++i)
            QTreeWidgetItem::setTextAlignment(i, Qt::AlignLeft);
    }

    ~DrawableItem() override = default;

    Drawable* drawable() { return drawable_; }

    void setVisible(int column, bool visible) {
        static QIcon iconShow(QString::fromUtf8(":/resources/icons/show.png"));
        static QIcon iconHide(QString::fromUtf8(":/resources/icons/hide.png"));
        if (visible)
            QTreeWidgetItem::setIcon(column, iconShow);
        else
            QTreeWidgetItem::setIcon(column, iconHide);
    }

private:
    Drawable* drawable_;
};


WidgetModelList::WidgetModelList(QWidget* parent)
    : QTreeWidget(parent), mainWindow_(nullptr)
    , popupMenu_(nullptr)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(modelItemSelectionChanged()));
    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(modelItemPressed(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

}


void WidgetModelList::init(mainwindow* w) {
    mainWindow_ = w;

    QStringList headerLabels;
    headerLabels << "Index" << "Type" << "Show" << "Name";

    setHeaderLabels(headerLabels);

    header()->setDefaultSectionSize(60);

    setIndentation(10);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
}


PaintCanvas* WidgetModelList::viewer() {
    return mainWindow_->viewer();
}


void WidgetModelList::updateDrawableVisibility(easy3d::Drawable* d) {
    int num = topLevelItemCount();
    for (int i = 0; i < num; ++i) {
        auto item = dynamic_cast<ModelItem*>(topLevelItem(i));
        if (item->model() == d->model()) {
            int num_children = item->childCount();
            for (int j = 0; j < num_children; ++j) {
                auto d_item = dynamic_cast<DrawableItem*>(item->child(j));
                if (d_item->drawable() == d)
                    d_item->setVisible(2, d->is_visible());
            }
        }
    }
}


void WidgetModelList::updateVisibilities() {
    int num = topLevelItemCount();
    for (int i = 0; i < num; ++i) {
        auto item = dynamic_cast<ModelItem*>(topLevelItem(i));
        bool visible = item->model()->renderer()->is_visible();
        item->setVisible(2, visible);
        item->highlight(item->model() == viewer()->currentModel());
        int num_children = item->childCount();
        for (int j = 0; j < num_children; ++j) {
            auto d_item = dynamic_cast<DrawableItem*>(item->child(j));
            d_item->setVisible(2, d_item->drawable()->is_visible());
        }
    }
}


void WidgetModelList::updateDrawableVisibilities() {
    int num = topLevelItemCount();
    for (int i = 0; i < num; ++i) {
        auto item = dynamic_cast<ModelItem*>(topLevelItem(i));
        int num_children = item->childCount();
        for (int j = 0; j < num_children; ++j) {
            auto d_item = dynamic_cast<DrawableItem*>(item->child(j));
            d_item->setVisible(2, d_item->drawable()->is_visible());
        }
    }
}


void WidgetModelList::prepareContextMenu(QMenu* menu) {
    menu->clear();  // I want to customize the menu list

    int num_selected = 0;
    int num_selected_meshes = 0;
    int num_unselected = 0;
    int num_invisible = 0;
    int num_visible_in_selected = 0;
    int num_invisible_in_selected = 0;

    int total = topLevelItemCount();
    for (int i = 0; i < total; ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        Model* model = dynamic_cast<ModelItem*>(item)->model();

        if (item->isSelected()) {
            ++num_selected;
            if (dynamic_cast<SurfaceMesh*>(model))
                ++num_selected_meshes;
        }
        else
            ++num_unselected;

        if (!model->renderer()->is_visible()) {
            ++num_invisible;
            if (item->isSelected())
                ++num_invisible_in_selected;
        }
        else {
            if (item->isSelected())
                ++num_visible_in_selected;
        }
    }


    // hide / show
    if (num_invisible_in_selected > 0) {
        QAction* actionShowSelected = menu->addAction("Show");
        connect(actionShowSelected, SIGNAL(triggered()), this, SLOT(showSelected()));
    }

    if (num_visible_in_selected > 0) {
        QAction* actionHideSelected = menu->addAction("Hide");
        connect(actionHideSelected, SIGNAL(triggered()), this, SLOT(hideSelected()));
    }
    menu->addSeparator();



    // manipulation
    if (num_selected == 1) {
        QAction* actionDuplicateCurrent = menu->addAction(
            QIcon(QString::fromUtf8(":/resources/icons/duplicate.png")), "Duplicate");
        connect(actionDuplicateCurrent, SIGNAL(triggered()), this, SLOT(duplicateCurrent()));
    }
    else if (num_selected > 1) {
        QAction* actionMergeSelected = menu->addAction(
            QIcon(QString::fromUtf8(":/resources/icons/merge.png")), "Merge");
        connect(actionMergeSelected, SIGNAL(triggered()), this, SLOT(mergeSelected()));
    }

    if (num_selected_meshes > 0) {
        QAction* actionDecomposeSelected = menu->addAction(
            QIcon(QString::fromUtf8(":/resources/icons/decompose.png")), "Decompose");
        connect(actionDecomposeSelected, SIGNAL(triggered()), this, SLOT(decomposeSelected()));
    }

    if (num_selected > 0) {
        menu->addSeparator();
        QAction* actionDeleteSelected = menu->addAction(
            QIcon(QString::fromUtf8(":/resources/icons/delete.png")), "Delete");
        connect(actionDeleteSelected, SIGNAL(triggered()), this, SLOT(deleteSelected()));
    }
}


void WidgetModelList::showContextMenu(const QPoint& pos) {
    if (!popupMenu_)
        popupMenu_ = new QMenu(this);

    prepareContextMenu(popupMenu_);

    QPoint p = mapToGlobal(pos);
    popupMenu_->popup(QPoint(p.x(), p.y() + header()->height()));
}


void WidgetModelList::updateModelList() {
    disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(modelItemSelectionChanged()));
    disconnect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this,
        SLOT(modelItemPressed(QTreeWidgetItem*, int)));
    disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    
    // clear everything and creat the list from scratch
    clear();


    const std::vector<Model*>& models = viewer()->models();
    for (unsigned int i = 0; i < models.size(); ++i) {
        Model* model = models[i];
        const std::string& name = file_system::base_name(model->name());
        auto item = dynamic_cast<ModelItem*>(this->topLevelItem(i));
        if (!item) {
            item = new ModelItem(this, model);
            addTopLevelItem(item);
        }

        item->setData(0, Qt::DisplayRole, i + 1);
        item->setIcon(1);
        item->setVisible(2, model->renderer()->is_visible());
        item->setData(3, Qt::DisplayRole, QString::fromStdString(name));
        item->highlight(model == viewer()->currentModel());

#if 1   // add the drawables as children
        for (auto d : model->renderer()->points_drawables()) {
            auto d_item = new DrawableItem(item, d);
            d_item->setVisible(2, d->is_visible());
            d_item->setData(3, Qt::DisplayRole, QString::fromStdString(d->name()));
            item->addChild(d_item);
        }

        for (auto d : model->renderer()->lines_drawables()) {
            auto d_item = new DrawableItem(item, d);
            d_item->setVisible(2, d->is_visible());
            d_item->setData(3, Qt::DisplayRole, QString::fromStdString(d->name()));
            item->addChild(d_item);
        }

        for (auto d : model->renderer()->triangles_drawables()) {
            auto d_item = new DrawableItem(item, d);
            d_item->setVisible(2, d->is_visible());
            d_item->setData(3, Qt::DisplayRole, QString::fromStdString(d->name()));
            item->addChild(d_item);
        }
#endif
    }

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(modelItemSelectionChanged()));
    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(modelItemPressed(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    

    mainWindow_->updateRenderingPanel();

}


void WidgetModelList::duplicateCurrent() {
    if (viewer()->currentModel()) {
        Model* model = viewer()->currentModel();

        Model* copy = nullptr;
        if (dynamic_cast<SurfaceMesh*>(model))
            copy = new SurfaceMesh(*dynamic_cast<SurfaceMesh*>(model));
        else if (dynamic_cast<PointCloud*>(model))
            copy = new PointCloud(*dynamic_cast<PointCloud*>(model));
        else if (dynamic_cast<Graph*>(model))
            copy = new Graph(*dynamic_cast<Graph*>(model));
        else if (dynamic_cast<PolyMesh*>(model))
            copy = new PolyMesh(*dynamic_cast<PolyMesh*>(model));

        if (copy) {
            const std::string& name = file_system::name_less_extension(model->name()) + "_copy";
            copy->set_name(name);
            viewer()->addModel(copy);
            addModel(copy, true);
        }
    }
}


void WidgetModelList::showSelected() {
    if (selectedItems().empty())
        return;

    const QList<QTreeWidgetItem*>& items = selectedItems();
    for (const auto& it : items) {
        auto item = dynamic_cast<ModelItem*>(it);
        Model* model = item->model();
        model->renderer()->set_visible(true);
    }

    updateModelList();
    viewer()->update();
}


void WidgetModelList::hideSelected() {
    if (selectedItems().empty())
        return;

    const QList<QTreeWidgetItem*>& items = selectedItems();
    for (const auto& it : items) {
        auto item = dynamic_cast<ModelItem*>(it);
        Model* model = item->model();
        model->renderer()->set_visible(false);
    }

    updateModelList();
    viewer()->update();
}


void WidgetModelList::decomposeSelected() {
    const QList<QTreeWidgetItem*>& items = selectedItems();
    for (const auto& it : items) {
        auto item = dynamic_cast<ModelItem*>(it);
        Model* model = item->model();
        decomposeModel(model);
    }
}


void WidgetModelList::mergeSelected() {
    std::vector<Model*> models;
    const QList<QTreeWidgetItem*>& items = selectedItems();
    for (const auto& it : items) {
        auto item = dynamic_cast<ModelItem*>(it);
        models.push_back(item->model());
    }

    mergeModels(models);
}


void WidgetModelList::deleteSelected() {
    int del = 0;

    Model* current_model = viewer()->currentModel();
    if (!current_model) {
        mainWindow_->log->set_log(log_type::Info, QString(tr("No model is selected.")).toStdString(), mainWindow_->getLogWidget());
    }
    else {
        if (selectedItems().size() > 1) {
            const QList<QTreeWidgetItem*>& items = selectedItems();

            for (const auto& it : items) {
                auto item = dynamic_cast<ModelItem*>(it);
                if (item) {
                    viewer()->deleteModel(item->model());
                    del++;
                }
            }

            if (del != 0) {

                updateModelList();
                viewer()->update();
                mainWindow_->updateRenderingPanel();

                mainWindow_->log->set_log(log_type::Info, QString(tr("%1 model(s) deleted.")).arg(QString::number(del)).toStdString(), mainWindow_->getLogWidget());
            }
            else {
                mainWindow_->log->set_log(log_type::Info, QString(tr("No model is selected or deleted.")).toStdString(), mainWindow_->getLogWidget());
            }
        }
        else {
            if (current_model) {
                viewer()->deleteModel(current_model);
                updateModelList();
                viewer()->update();
                mainWindow_->updateRenderingPanel();
            }
        }
    }
}

void WidgetModelList::deleteFromMainwindow() {
    deleteSelected();
}

void WidgetModelList::modelItemSelectionChanged() {
    int num = topLevelItemCount();
    for (int i = 0; i < num; ++i) {
        auto item = dynamic_cast<ModelItem*>(topLevelItem(i));

        //// don't allow changing selection for camera path creation
        //if (viewer()->walkThrough()->status() == WalkThrough::STOPPED && viewer()->isSelectModelEnabled())
        //    item->model()->renderer()->set_selected(item->isSelected());
        
        
    }
    viewer()->update();
    update();
    //mainWindow_->updateRenderingPanel();
}



void WidgetModelList::modelItemPressed(QTreeWidgetItem* current, int column) {
    Model* prev_active_model = viewer()->currentModel();

    Model* active_model = nullptr;
    if (dynamic_cast<ModelItem*>(current)) {
        auto current_item = dynamic_cast<ModelItem*>(current);
        active_model = current_item->model();
        viewer()->setCurrentModel(active_model);
        if (column == 2) {
        /*if (column == 2 && !selected_only_) {*/
            Model* model = current_item->model();
            bool visible = !model->renderer()->is_visible();
            current_item->setVisible(2, visible);
            model->renderer()->set_visible(visible);
            
        }

    }
    else if (dynamic_cast<DrawableItem*>(current)) {
        auto drawable_item = dynamic_cast<DrawableItem*>(current);
        active_model = drawable_item->drawable()->model();
        if (column == 2) {
            Drawable* d = drawable_item->drawable();
            bool visible = !d->is_visible();
            d->set_visible(visible);
            drawable_item->setVisible(2, visible);
        }

        mainWindow_->activeDrawableChanged(drawable_item->drawable());    
    }

    viewer()->setCurrentModel(active_model);

    updateVisibilities();

    viewer()->update();

    mainWindow_->updateRenderingPanel();

}


void WidgetModelList::mousePressEvent(QMouseEvent* e) {

    //if (!indexAt(e->pos()).isValid())
    //    selectionModel()->clearSelection();
    QTreeWidget::mousePressEvent(e);
}


void WidgetModelList::mouseReleaseEvent(QMouseEvent* e) {
    QTreeWidget::mouseReleaseEvent(e);
}



void WidgetModelList::addModel(Model* model, bool make_current) {
    if (!model)
        return;

    if (make_current) {
        //mainWindow_->enableCameraManipulation();
        viewer()->setCurrentModel(model);
    }

    updateModelList();
    viewer()->fitScreen();
    viewer()->update();
}


void WidgetModelList::mergeModels(const std::vector<Model*>& models) {
    std::vector<SurfaceMesh*> meshes;
    std::vector<PointCloud*> clouds;

    std::vector<Model*> selectedModels;
    for (auto m : models) {
        if (dynamic_cast<SurfaceMesh*>(m))
            meshes.push_back(dynamic_cast<SurfaceMesh*>(m));
        else if (dynamic_cast<PointCloud*>(m))
            clouds.push_back(dynamic_cast<PointCloud*>(m));
    }

    std::vector<Model*> to_delete;

    if (meshes.size() > 1) {
        SurfaceMesh* to = meshes[0];
        auto index = to->add_face_property<int>("f:index", 0);
        ProgressLogger logger(meshes.size() - 1, false, false);
        for (std::size_t i = 1; i < meshes.size(); ++i) {
            logger.notify(i);
            if (logger.is_canceled())
                break;

            unsigned int id_start = to->n_faces();
            SurfaceMesh* from = meshes[i];
            to->join(*from);

            unsigned int count = from->n_faces();
            for (unsigned int id = id_start; id < id_start + count; ++id)
                index[SurfaceMesh::Face(static_cast<int>(id))] = static_cast<int>(i);

            to_delete.push_back(from);
        }

        to->set_name("merged_mesh");
        to->renderer()->update();
        to->manipulator()->reset();
    }

    if (clouds.size() > 1) {
        PointCloud* to = clouds[0];
        auto index = to->add_vertex_property<int>("v:index", 0);
        if (index != Property<PointCloud>()) {
            
            ProgressLogger logger(clouds.size() - 1, false, false);
            for (int i = 1; i < clouds.size(); ++i) {
                logger.notify(i);
                if (logger.is_canceled())
                    break;

                unsigned int id_start = to->n_vertices();
                PointCloud* from = clouds[i];
                to->join(*from);

                unsigned int count = from->n_vertices();
                for (unsigned int id = id_start; id < id_start + count; ++id)
                    index[PointCloud::Vertex(static_cast<int>(id))] = static_cast<int>(i);

                to_delete.push_back(from);
            }

            to->set_name("merged_point_set");
            to->renderer()->update();
            
        }
        else {
            mainWindow_->log->set_log(log_type::Error, QString(tr("A property with name v:index already exists. Returning invalid property.")).toStdString(), mainWindow_->getLogWidget());
            return;
        }
    }

    for (auto model : to_delete)
        viewer()->deleteModel(model);

    // update display and ui
    if (meshes.size() > 1 || clouds.size() > 1) {
        updateModelList();
        mainWindow_->updateRenderingPanel();
        viewer()->update();
    }
}


void WidgetModelList::decomposeModel(Model* model) {
    auto mesh = dynamic_cast<SurfaceMesh*>(model);
    if (!mesh)
        return;

    const auto& components = SurfaceMeshComponent::extract(mesh);
    if (components.size() == 1) {
        LOG(WARNING) << "model has only one component";
        return;
    }

    const std::string base_name =
        file_system::parent_directory(mesh->name()) + "/" + file_system::base_name(mesh->name()) + "_part_";

    ProgressLogger progress(components.size(), false, false);
    for (unsigned int i = 0; i < components.size(); i++) {
        if (progress.is_canceled()) {
            LOG(WARNING) << "decomposing model cancelled";
            return;
        }

        SurfaceMesh* new_mesh = components[i].to_mesh();
        new_mesh->set_name(base_name + std::to_string(i + 1));
        viewer()->addModel(new_mesh);
        progress.next();
    }

    // delete the original model
    viewer()->deleteModel(mesh);
    updateModelList();
    mainWindow_->updateRenderingPanel();
    viewer()->update();

    LOG(INFO) << "model decomposed into " << components.size() << " parts";
}