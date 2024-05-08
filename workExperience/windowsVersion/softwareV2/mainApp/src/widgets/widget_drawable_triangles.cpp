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



#include "../include/easy3d_inc/widgets/widget_drawable_triangles.h"
#include "../include/mainwindow.h"
#include "../include/easy3d_inc/viewer.h"
#include "../include/easy3d_inc/widgets/widget_model_list.h"

#include <QColorDialog>
#include <QFileDialog>

#include <easy3d/core/model.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/drawable_triangles.h>
#include <easy3d/renderer/texture_manager.h>
#include <easy3d/renderer/buffer.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/poly_mesh.h>
#include <easy3d/util/file_system.h>
#include <easy3d/util/logging.h>
#include <easy3d/util/resource.h>
#include <easy3d/util/stop_watch.h>


using namespace easy3d;

easy3d::StopWatch w2;
WidgetTrianglesDrawable::WidgetTrianglesDrawable(QWidget* parent)
    : WidgetDrawable(parent), ui_(new Ui::WidgetTrianglesDrawable), left_restrictValue(0), right_restrictValue(0)
{
    ui_->setupUi(this);

    if (colormaps_.empty())
        ui_->comboBoxScalarFieldStyle->addItem("not available");
    else {
        for (const auto& colormap : colormaps_)
            ui_->comboBoxScalarFieldStyle->addItem(QIcon(QString::fromStdString(colormap.file)),
                QString::fromStdString("  " + colormap.name));
    }
}


void WidgetTrianglesDrawable::connectAll() {
    // which drawable
    connect(ui_->comboBoxDrawables, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setActiveDrawable(const QString&)));

    // visible
    connect(ui_->checkBoxVisible, SIGNAL(toggled(bool)), this, SLOT(setDrawableVisible(bool)));

    // phong shading
    connect(ui_->checkBoxPhongShading, SIGNAL(toggled(bool)), this, SLOT(setPhongShading(bool)));

    // lighting
    connect(ui_->comboBoxLightingOptions, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setLighting(const QString&)));

    // color scheme
    connect(ui_->comboBoxColorScheme, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setColorScheme(const QString&)));

    // default color
    connect(ui_->toolButtonDefaultColor, SIGNAL(clicked()), this, SLOT(setDefaultColor()));

    // back color
    connect(ui_->checkBoxBackColor, SIGNAL(toggled(bool)), this, SLOT(setDistinctBackColor(bool)));
    connect(ui_->toolButtonBackColor, SIGNAL(clicked()), this, SLOT(setBackColor()));

    // scalar field
    connect(ui_->comboBoxScalarFieldStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(setScalarFieldStyle(int)));
    connect(ui_->checkBoxScalarFieldClamp, SIGNAL(toggled(bool)), this, SLOT(setScalarFieldClamp(bool)));
    connect(ui_->slider_leftclamp, SIGNAL(valueChanged(int)), this, SLOT(setScalarFieldClampLower(int)));
    connect(ui_->slider_rightclamp, SIGNAL(valueChanged(int)), this, SLOT(setScalarFieldClampUpper(int)));

    // vector field
    connect(ui_->comboBoxVectorField, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setVectorField(const QString&)));
    connect(ui_->doubleSpinBoxVectorFieldScale, SIGNAL(valueChanged(double)), this, SLOT(setVectorFieldScale(double)));
}


void WidgetTrianglesDrawable::disconnectAll() {
    // which drawable
    disconnect(ui_->comboBoxDrawables, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setActiveDrawable(const QString&)));

    // visible
    disconnect(ui_->checkBoxVisible, SIGNAL(toggled(bool)), this, SLOT(setDrawableVisible(bool)));

    // phong shading
    disconnect(ui_->checkBoxPhongShading, SIGNAL(toggled(bool)), this, SLOT(setPhongShading(bool)));

    // lighting
    disconnect(ui_->comboBoxLightingOptions, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setLighting(const QString&)));

    // color scheme
    disconnect(ui_->comboBoxColorScheme, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setColorScheme(const QString&)));

    // default color
    disconnect(ui_->toolButtonDefaultColor, SIGNAL(clicked()), this, SLOT(setDefaultColor()));

    // back color
    disconnect(ui_->checkBoxBackColor, SIGNAL(toggled(bool)), this, SLOT(setDistinctBackColor(bool)));
    disconnect(ui_->toolButtonBackColor, SIGNAL(clicked()), this, SLOT(setBackColor()));

    // scalar field
    disconnect(ui_->comboBoxScalarFieldStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(setScalarFieldStyle(int)));
    disconnect(ui_->checkBoxScalarFieldClamp, SIGNAL(toggled(bool)), this, SLOT(setScalarFieldClamp(bool)));
    disconnect(ui_->slider_leftclamp, SIGNAL(valueChanged(int)), this, SLOT(setScalarFieldClampLower(int)));
    disconnect(ui_->slider_rightclamp, SIGNAL(valueChanged(int)), this, SLOT(setScalarFieldClampUpper(int)));

    // vector field
    disconnect(ui_->comboBoxVectorField, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(setVectorField(const QString&)));
    disconnect(ui_->doubleSpinBoxVectorFieldScale, SIGNAL(valueChanged(double)), this, SLOT(setVectorFieldScale(double)));
}


WidgetTrianglesDrawable::~WidgetTrianglesDrawable() {
    delete ui_;
}

// update the panel to be consistent with the drawable's rendering parameters
void WidgetTrianglesDrawable::updatePanel() {
    auto model = viewer_->currentModel();
    if (!model) {
        active_drawable_.clear();
        ui_->lineEditFaces->clear();
    }

    auto d = dynamic_cast<TrianglesDrawable*>(drawable());
    if (!model || !model->renderer()->is_visible() || !d) {
        setEnabled(false);
        ui_->lineEditFaces->clear(); // if point cloud is clicked
        return;
    }

    w2.restart();
    QApplication::processEvents(); // show loading dialog

    setEnabled(true);

    disconnectAll();

    auto& state = states_[d];
    auto& scheme = d->state();

    ui_->comboBoxDrawables->clear();
    const auto& drawables = model->renderer()->triangles_drawables();
    for (auto drawable : drawables)
        ui_->comboBoxDrawables->addItem(QString::fromStdString(drawable->name()));
    ui_->comboBoxDrawables->setCurrentText(QString::fromStdString(d->name()));

    // faces
    if (dynamic_cast<SurfaceMesh*>(model)) {
        auto mesh = dynamic_cast<SurfaceMesh*>(model);
        ui_->lineEditFaces->setText(QString::number(mesh->n_faces()));
        ui_->lineEditFaces->setCursorPosition(0);
    }

    // visible
    ui_->checkBoxVisible->setChecked(d->is_visible());
    window_->widgetModelList()->updateDrawableVisibilities();

    // phong shading
    ui_->checkBoxPhongShading->setChecked(d->smooth_shading());

    {   // lighting
        if (d->lighting()) {
            if (d->lighting_two_sides())
                ui_->comboBoxLightingOptions->setCurrentText("front and back");
            else
                ui_->comboBoxLightingOptions->setCurrentText("front only");
        }
        else
            ui_->comboBoxLightingOptions->setCurrentText("disabled");
    }

    {   // color scheme
        ui_->comboBoxColorScheme->clear();
        const std::vector<QString>& schemes = colorSchemes(viewer_->currentModel());
        for (const auto& name : schemes)
            ui_->comboBoxColorScheme->addItem(name);

        for (const auto& name : schemes) {

            if (name.contains(QString::fromStdString(scheme.property_name()))) {
                ui_->comboBoxColorScheme->setCurrentText(name);
                break;
            }
        }
        // lasso -> return back to normal color
        if (ui_->comboBoxColorScheme->currentText() != "scalar - f:select" ) {
            color_scheme_name = ui_->comboBoxColorScheme->currentText();
        }


        // default color
        auto c = d->color();
        QPixmap pixmap(ui_->toolButtonDefaultColor->size());
        pixmap.fill(
            QColor(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255)));
        ui_->toolButtonDefaultColor->setIcon(QIcon(pixmap));

        // back side color
        ui_->checkBoxBackColor->setChecked(d->distinct_back_color());
        c = d->back_color();
        pixmap.fill(
            QColor(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255)));
        ui_->toolButtonBackColor->setIcon(QIcon(pixmap));

    }

    {   // scalar field
        ui_->comboBoxScalarFieldStyle->setCurrentIndex(state.scalar_style);
        const auto& coloring = ui_->comboBoxColorScheme->currentText().toStdString();
        if (coloring.find("scalar - ") != std::string::npos && coloring.find(scheme.property_name()) != std::string::npos) {
            auto tex = colormapTexture(states_[d].scalar_style, states_[d].discrete_color, states_[d].num_stripes);
            if (tex)
                d->set_texture(tex);
        }
        ui_->checkBoxScalarFieldClamp->setChecked(scheme.clamp_range());
        ui_->slider_leftclamp->setValue(scheme.clamp_lower() * 100);
        ui_->slider_rightclamp->setValue(scheme.clamp_upper() * 100);
        ui_->label_leftclamp->setText(QString::number(ui_->slider_leftclamp->value()) + "%");
        ui_->label_rightclamp->setText(QString::number(ui_->slider_rightclamp->value()) + "%");
    }

    {   // vector field
        ui_->comboBoxVectorField->clear();
        const std::vector<QString>& fields = vectorFields(viewer_->currentModel());
        for (const auto& name : fields)
            ui_->comboBoxVectorField->addItem(name);

        ui_->comboBoxVectorField->setCurrentText(state.vector_field);
        ui_->doubleSpinBoxVectorFieldScale->setValue(state.vector_field_scale);
    }

    disableUnavailableOptions();

    connectAll();

    if (w2.elapsed_seconds() > 0.5) {

        QString temp = QString(tr("updating rendering buffers for drawable '%1', done in %2")).arg(QString::fromStdString(drawable()->name())).arg(QString::fromStdString(w2.time_string()));
        window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
    }
}




namespace internal {

    template <typename MODEL>
    void collect_color_schemes(MODEL* model, const QString& scalar_prefix, std::vector<QString>& schemes) {
        // color schemes from color properties and texture
        for (const auto& name : model->face_properties()) {
            if (name.find("f:color") != std::string::npos)
                schemes.push_back(QString::fromStdString(name));
        }
        for (const auto& name : model->vertex_properties()) {
            if (name.find("v:color") != std::string::npos || name.find("v:texcoord") != std::string::npos)
                schemes.push_back(QString::fromStdString(name));
        }

        // scalar fields defined on faces
        for (const auto& name : model->face_properties()) {
            if (model->template get_face_property<float>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_face_property<double>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_face_property<unsigned int>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_face_property<int>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_face_property<char>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_face_property<unsigned char>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_face_property<bool>(name) && name == "f:select")
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
        }

        // scalar fields defined on vertices
        for (const auto& name : model->vertex_properties()) {
            if (model->template get_vertex_property<float>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_vertex_property<double>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_vertex_property<unsigned int>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_vertex_property<int>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_vertex_property<char>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
            else if (model->template get_vertex_property<unsigned char>(name))
                schemes.push_back(scalar_prefix + QString::fromStdString(name));
        }
    }

    // vector fields defined on faces
    template <typename MODEL>
    void vector_fields_on_faces(MODEL* model, std::vector<QString>& fields) {
        fields.emplace_back("f:normal");
        for (const auto& name : model->face_properties()) {
            if (model->template get_face_property<vec3>(name)) {
                if (name != "f:normal" && name != "f:color")
                    fields.push_back(QString::fromStdString(name));
            }
        }
    }

}



std::vector<QString> WidgetTrianglesDrawable::colorSchemes(const easy3d::Model* model) {
    std::vector<QString> schemes;
    schemes.emplace_back("uniform color");

    auto mesh = dynamic_cast<SurfaceMesh*>(viewer_->currentModel());
    if (mesh) {
        for (const auto& name : mesh->halfedge_properties()) {
            if (name.find("h:texcoord") != std::string::npos)
                schemes.push_back(QString::fromStdString(name));
        }

        internal::collect_color_schemes(mesh, scalar_prefix_, schemes);
    }

    auto poly = dynamic_cast<PolyMesh*>(viewer_->currentModel());
    if (poly)
        internal::collect_color_schemes(poly, scalar_prefix_, schemes);

    return schemes;
}


std::vector<QString> WidgetTrianglesDrawable::vectorFields(const easy3d::Model* model) {
    std::vector<QString> fields;

    auto mesh = dynamic_cast<SurfaceMesh*>(viewer_->currentModel());
    if (mesh)
        internal::vector_fields_on_faces(mesh, fields);
    else {
        auto poly = dynamic_cast<PolyMesh*>(viewer_->currentModel());
        if (poly)
            internal::vector_fields_on_faces(poly, fields);
    }

    // if no vector fields found, add a "not available" item
    if (fields.empty())
        fields.emplace_back("not available");
    else   // add one allowing to disable vector fields
        fields.insert(fields.begin(), "disabled");

    return fields;
}


Drawable* WidgetTrianglesDrawable::drawable() {
    auto model = viewer_->currentModel();
    if (!model) {
        active_drawable_.clear();
        return nullptr;
    }

    auto pos = active_drawable_.find(model);
    if (pos != active_drawable_.end())
        return model->renderer()->get_triangles_drawable(pos->second);
    else {
        for (auto d : model->renderer()->triangles_drawables()) {
            active_drawable_[model] = d->name();
            return d;
        }
        return nullptr;
    }
}


void WidgetTrianglesDrawable::setActiveDrawable(const QString& text) {
    auto model = viewer_->currentModel();
    const std::string& name = text.toStdString();

    if (active_drawable_.find(model) != active_drawable_.end()) {
        if (active_drawable_[model] == name)
            return; // already active
    }

    if (model->renderer()->get_triangles_drawable(name)) {
        active_drawable_[model] = name;
    }
    else {
        LOG(ERROR) << "drawable '" << name << "' not defined on model: " << model->name();
        const auto& drawables = model->renderer()->triangles_drawables();
        if (drawables.empty())
            LOG(ERROR) << "no triangles drawable defined on model: " << model->name();
        else
            active_drawable_[model] = drawables[0]->name();
    }

    updatePanel();
}


void WidgetTrianglesDrawable::setPhongShading(bool b) {
    auto d = dynamic_cast<TrianglesDrawable*>(drawable());
    if (d->smooth_shading() != b) {
        d->set_smooth_shading(b);
        viewer_->update();
    }
}


void WidgetTrianglesDrawable::setColorScheme(const QString& text) {
    auto d = drawable();


    auto& scheme = d->state();
    scheme.set_clamp_range(ui_->checkBoxScalarFieldClamp->isChecked());
    scheme.set_clamp_lower(ui_->slider_leftclamp->value() / 100.0f);
    scheme.set_clamp_upper(ui_->slider_rightclamp->value() / 100.0f);
    states_[d].scalar_style = ui_->comboBoxScalarFieldStyle->currentIndex();

    WidgetDrawable::setColorScheme(text);
}





void WidgetTrianglesDrawable::setOpacity(int a) {
    auto d = dynamic_cast<TrianglesDrawable*>(drawable());
    d->set_opacity(a / 100.0f);
    viewer_->update();
}


void WidgetTrianglesDrawable::setDefaultColor() {
    auto d = drawable();
    const auto& c = d->color();
    QColor orig(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255));
    const QColor& color = QColorDialog::getColor(orig, this);
    if (color.isValid()) {
        const vec4 new_color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
        d->set_uniform_coloring(new_color);
        viewer_->update();

        QPixmap pixmap(ui_->toolButtonDefaultColor->size());
        pixmap.fill(color);
        ui_->toolButtonDefaultColor->setIcon(QIcon(pixmap));
    }
}


void WidgetTrianglesDrawable::setBackColor() {
    auto d = drawable();
    const auto& c = d->back_color();
    QColor orig(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255));
    const QColor& color = QColorDialog::getColor(orig, this);
    if (color.isValid()) {
        const vec4 new_color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
        d->set_back_color(new_color);
        viewer_->update();

        QPixmap pixmap(ui_->toolButtonBackColor->size());
        pixmap.fill(color);
        ui_->toolButtonBackColor->setIcon(QIcon(pixmap));
    }
}


void WidgetTrianglesDrawable::setVectorField(const QString& text) {
    auto mesh = viewer_->currentModel();

    if (text == "disabled") {
        const auto& drawables = mesh->renderer()->lines_drawables();
        for (auto d : drawables) {
            if (d->name().find("vector - f") != std::string::npos)
                d->set_visible(false);
        }
        states_[drawable()].vector_field = "disabled";
    }
    else {
        const std::string& name = text.toStdString();
        updateVectorFieldBuffer(mesh, name);

        auto d = mesh->renderer()->get_lines_drawable("vector - " + name);
        if (d) { // just in case the vector field has been removed
            d->set_visible(true);
            states_[drawable()].vector_field = text;
        }
    }

    viewer_->update();
    window_->updateRenderingPanel();
}


void WidgetTrianglesDrawable::setScalarFieldStyle(int idx) {
    WidgetDrawable::setScalarFieldStyle(idx);
    
}


namespace internal {

    template<typename MODEL>
    // spinbox must appear as a parameter because its value maybe changed and its is used later in a lambda function
    void updateVectorFieldBuffer(MODEL* model, const std::string& name, QDoubleSpinBox* spinBox) {
        if (!model)
            return;

        if (name == "f:normal") {
            auto normals = model->template get_face_property<vec3>(name);
            if (!normals)
                model->update_face_normals();
        }
        else {
            auto prop = model->template get_face_property<vec3>(name);
            if (!prop && name != "disabled") {
                LOG(ERROR) << "vector field '" << name << "' doesn't exist";
                return;
            }
        }

        // a vector field is visualized as a LinesDrawable whose name is the same as the vector field
        auto drawable = model->renderer()->get_lines_drawable("vector - " + name);
        if (!drawable) {
            drawable = model->renderer()->add_lines_drawable("vector - " + name);
            drawable->set_update_func(
                [model, drawable, name, spinBox](Model* m, Drawable* d) -> void {
                    buffer::update(model, dynamic_cast<LinesDrawable*>(drawable), name, State::FACE, spinBox->value());
                }
            );
        }
    }


    void set_highlight_range(SurfaceMesh* m, Drawable* d, const std::pair<int, int>& range) {
        if (range.second < 0 || range.second < range.first) {
            d->set_highlight_range(std::make_pair(-1, -1));
            LOG(INFO) << "no face can be highlighted";
            return;
        }

        // convert to triangle range
        int f_min = (range.first < 0 ? 0 : range.first);
        int f_max = (range.second >= static_cast<int>(m->n_faces()) ? static_cast<int>(m->n_faces()) - 1 : range.second);
        int t_min = std::numeric_limits<int>::max();
        int t_max = -std::numeric_limits<int>::max();
        auto triangle_range = m->get_face_property<std::pair<int, int> >("f:triangle_range");
        if (triangle_range) {
            for (int fid = f_min; fid <= f_max; ++fid) {
                auto f = SurfaceMesh::Face(fid);
                const auto& ran = triangle_range[f];
                t_min = std::min(t_min, ran.first);
                t_max = std::max(t_max, ran.second);
            }

            d->set_highlight_range(std::make_pair(t_min, t_max));
            if (f_min != t_min || f_max != t_max)
                LOG(INFO) << "highlighting faces [" << f_min << ", " << f_max << "] (= triangles ["
                << t_min << ", " << t_max << "])";
            else
                LOG(INFO) << "highlighting faces [" << f_min << ", " << f_max << "]";
        }
        else {
            LOG_N_TIMES(3, ERROR) << "face property \'f:triangle_range\' not defined. " << COUNTER;
        }
    }

}


void WidgetTrianglesDrawable::updateVectorFieldBuffer(Model* model, const std::string& name) {
    if (dynamic_cast<SurfaceMesh*>(model)) {
        auto mesh = dynamic_cast<SurfaceMesh*>(model);
        internal::updateVectorFieldBuffer(mesh, name, ui_->doubleSpinBoxVectorFieldScale);
    }
    else if (dynamic_cast<PolyMesh*>(model)) {
        auto mesh = dynamic_cast<PolyMesh*>(model);
        internal::updateVectorFieldBuffer(mesh, name, ui_->doubleSpinBoxVectorFieldScale);
    }
}



void WidgetTrianglesDrawable::disableUnavailableOptions() {
    auto d = drawable();

    bool visible = ui_->checkBoxVisible->isChecked();
    ui_->labelPhongShading->setEnabled(visible);
    ui_->checkBoxPhongShading->setEnabled(visible);
    ui_->labelLighting->setEnabled(visible);
    ui_->comboBoxLightingOptions->setEnabled(visible);
    ui_->labelColorScheme->setEnabled(visible);
    ui_->comboBoxColorScheme->setEnabled(visible);

    bool can_modify_default_color = visible && (ui_->comboBoxColorScheme->currentText() == "uniform color");
    ui_->labelDefaultColor->setEnabled(can_modify_default_color);
    ui_->toolButtonDefaultColor->setEnabled(can_modify_default_color);

    const auto& lighting_option = ui_->comboBoxLightingOptions->currentText();
    bool can_modify_back_color = visible && (lighting_option != "disabled");
    ui_->labelBackColor->setEnabled(can_modify_back_color);
    ui_->checkBoxBackColor->setEnabled(can_modify_back_color);
    ui_->toolButtonBackColor->setEnabled(can_modify_back_color && d->distinct_back_color());

    // scalar field
    bool can_show_scalar = visible && ui_->comboBoxColorScheme->currentText().contains(scalar_prefix_);
    ui_->labelScalarFieldStyle->setEnabled(can_show_scalar);
    ui_->comboBoxScalarFieldStyle->setEnabled(can_show_scalar);

    ui_->labelScalarFieldClamp->setEnabled(can_show_scalar);
    ui_->checkBoxScalarFieldClamp->setEnabled(can_show_scalar);
    bool can_edit_clamp = can_show_scalar && d->clamp_range();
    ui_->slider_leftclamp->setEnabled(can_edit_clamp && ui_->checkBoxScalarFieldClamp->isChecked());
    ui_->slider_rightclamp->setEnabled(can_edit_clamp && ui_->checkBoxScalarFieldClamp->isChecked());
    //ui_->doubleSpinBoxScalarFieldClampLower->setEnabled(can_edit_clamp && ui_->checkBoxScalarFieldClamp->isChecked());
    //ui_->doubleSpinBoxScalarFieldClampUpper->setEnabled(can_edit_clamp && ui_->checkBoxScalarFieldClamp->isChecked());

    // vector field
    bool can_show_vector = visible && ui_->comboBoxVectorField->currentText() != "not available";
    ui_->labelVectorField->setEnabled(can_show_vector);
    ui_->comboBoxVectorField->setEnabled(can_show_vector);
    bool can_modify_vector_style = can_show_vector && ui_->comboBoxVectorField->currentText() != "disabled";
    ui_->labelVectorFieldScale->setEnabled(can_modify_vector_style);
    ui_->doubleSpinBoxVectorFieldScale->setEnabled(can_modify_vector_style);

    update();
    qApp->processEvents();
}


void WidgetTrianglesDrawable::setScalarFieldClampLower(int v) {
    w2.restart();
    if (v + ui_->slider_rightclamp->value() < 100) {

        ui_->label_leftclamp->setText(QString::number(v) + "%");
        auto d = drawable();
        if (d->clamp_upper() * 100 + v < 100) {
            d->set_clamp_lower(v / 100.0f);
            d->update();
            viewer_->update();
        }
        left_restrictValue = v;
    }
    else {
        ui_->slider_leftclamp->setSliderPosition(left_restrictValue);
    }

    QApplication::processEvents(); // show loading dialog

    if (w2.elapsed_seconds() > 0.5) {

        QString temp = QString(tr("updating rendering buffers for drawable '%1', done in %2")).arg(QString::fromStdString(drawable()->name())).arg(QString::fromStdString(w2.time_string()));
        window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
    }

}


void WidgetTrianglesDrawable::setScalarFieldClampUpper(int v) {
    w2.restart();
    if (v + ui_->slider_leftclamp->value() < 100) {

        ui_->label_rightclamp->setText(QString::number(v) + "%");
        auto d = drawable();
        if (d->clamp_lower() * 100 + v < 100) {
            d->set_clamp_upper(v / 100.0f);
            d->update();
            viewer_->update();
        }
        right_restrictValue = v;
    }
    else {
        ui_->slider_rightclamp->setSliderPosition(right_restrictValue);
    }
    
    QApplication::processEvents(); // show loading dialog

    if (w2.elapsed_seconds() > 0.5) {

        QString temp = QString(tr("updating rendering buffers for drawable '%1', done in %2")).arg(QString::fromStdString(drawable()->name())).arg(QString::fromStdString(w2.time_string()));
        window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());
    }
}

void WidgetTrianglesDrawable::changeLanguage(Language a)
{
    linguist* lang = linguist::getLinguistInstance();
    if (a == Language::en_)
    {
        lang->changeLanguage(Language::en_);
    }
    else {
        lang->changeLanguage(Language::zh_);
    }
    ui_->retranslateUi(this);

}