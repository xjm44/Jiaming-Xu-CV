///********************************************************************
// * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
// * https://3d.bk.tudelft.nl/liangliang/
// *
// * This file is part of Easy3D. If it is useful in your research/work,
// * I would be grateful if you show your appreciation by citing it:
// * ------------------------------------------------------------------
// *      Liangliang Nan.
// *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
// *      for processing and rendering 3D data.
// *      Journal of Open Source Software, 6(64), 3255, 2021.
// * ------------------------------------------------------------------
// *
// * Easy3D is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License Version 3
// * as published by the Free Software Foundation.
// *
// * Easy3D is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see <http://www.gnu.org/licenses/>.
// ********************************************************************/

#include <QDir>
#include "../include/include_gui/mainApp.h"
#include "../include/include_easy3d/viewer.h"

#include "../include/include_easy3d/dialog/walk_through.h"

#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/point_cloud.h>
#include <easy3d/core/graph.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/drawable_triangles.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/renderer/shader_program.h>
#include <easy3d/renderer/shader_manager.h>
#include <easy3d/renderer/shape.h>
#include <easy3d/renderer/transform.h>
#include <easy3d/renderer/camera.h>
#include <easy3d/renderer/manipulated_camera_frame.h>
#include <easy3d/renderer/key_frame_interpolator.h>
#include <easy3d/renderer/ambient_occlusion.h>
#include <easy3d/renderer/soft_shadow.h>
#include <easy3d/renderer/dual_depth_peeling.h>
#include <easy3d/renderer/eye_dome_lighting.h>
#include <easy3d/renderer/read_pixel.h>
#include <easy3d/renderer/opengl_util.h>
#include <easy3d/renderer/opengl_error.h>
#include <easy3d/renderer/clipping_plane.h>
#include <easy3d/renderer/texture_manager.h>
#include <easy3d/renderer/text_renderer.h>
#include <easy3d/renderer/manipulator.h>
#include <easy3d/renderer/buffer.h>
#include <easy3d/util/resource.h>
#include <easy3d/gui/picker_point_cloud.h>
#include <easy3d/gui/picker_surface_mesh.h>
#include <easy3d/gui/picker_model.h>
#include <easy3d/gui/picker.h>
#include <easy3d/util/file_system.h>
#include <easy3d/util/logging.h>
#include <easy3d/util/string.h>
#include <easy3d/util/line_stream.h>
#include <easy3d/util/dialog.h>
#include <easy3d/util/setting.h>

#include <QKeyEvent>
#include <QPainter>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QApplication>
#include <QClipboard>
#include <QTime>
#include <QStatusBar>

using namespace easy3d;

static struct Init {
    Init() { setting::background_color = vec4(28 / 255, 28 / 255, 28 / 255, 1); }
} init;

PaintCanvas::PaintCanvas(mainApp* window)
    : QOpenGLWidget(window)
    , window_(window)
    , func_(nullptr)
    //, texter_(nullptr)
    //, show_easy3d_logo_(true)
    , show_frame_rate_(true)
    , dpi_scaling_(1.0)
    , samples_(0)
    , camera_(nullptr)
    , background_color_(setting::background_color)
    , pressed_button_(Qt::NoButton)
    //, modifiers_(Qt::NoModifier)
    //, mouse_current_pos_(0, 0)
    //, mouse_pressed_pos_(0, 0)
    , pressed_key_(-1)
    , show_manip_sphere_(false)
    , window_show_manip_sphere_(false)
    , drawable_axes_(nullptr)
    , drawable_manip_sphere_(nullptr)
    //, model_picker_(nullptr)
    //, allow_select_model_(false)
    //, surface_mesh_picker_(nullptr)
    //, show_primitive_id_under_mouse_(false)
    //, show_primitive_property_under_mouse_(false)
    //, picked_face_index_(-1)
    //, point_cloud_picker_(nullptr)
    //, picked_vertex_index_(-1)
    //, show_coordinates_under_mouse_(false)
    , model_idx_(-1)
    , ssao_(nullptr), edl_(nullptr), shadow_(nullptr)
    , no_of_points(0)
    , show_line(false)
    , zoomDist(0.0f)
    , showSize(false)
    , photo_texture_(nullptr)
    , showPhoto(false)
    , photoPath(QString())
    , cameraPoint(0)
    , startGcp(false)
{ 

    // like Qt::StrongFocus plus the widget accepts focus by using the mouse wheel.
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    camera_ = new Camera;
    camera_->setType(Camera::PERSPECTIVE);
    //camera_->setType(Camera::ORTHOGRAPHIC);
    camera_->setUpVector(vec3(0, 0, 1)); // Z pointing up
    camera_->setViewDirection(vec3(-1, 0, 0)); // X pointing out
    camera_->showEntireScene();

    easy3d::connect(&camera_->frame_modified, this, static_cast<void (PaintCanvas::*)(void)>(&PaintCanvas::update));

    walk_through_ = new WalkThrough(camera_);

}


PaintCanvas::~PaintCanvas() {
    // Make sure the context is current and then explicitly
    // destroy all underlying OpenGL resources.
    makeCurrent();

    cleanup();

    doneCurrent();
}

void PaintCanvas::cleanup() {
    for (auto m : models_) {
        delete m->renderer();
        delete m->manipulator();
        delete m;
    }

    delete camera_;
    delete walkThrough();
    delete drawable_axes_;
    delete drawable_manip_sphere_;
    delete shadow_;
    delete edl_;
    //delete model_picker_;
    //delete surface_mesh_picker_;
    //delete point_cloud_picker_;

    ShaderManager::terminate();
    TextureManager::terminate();
}

void PaintCanvas::setBackgroundColor(const vec4& c) {
    background_color_ = c;
    makeCurrent();
    func_->glClearColor(background_color_[0], background_color_[1], background_color_[2], background_color_[3]);
    doneCurrent();
}

void PaintCanvas::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);

    // The viewport is set up by QOpenGLWidget before drawing.
    // So I don't need to set up.
    // func_->glViewport(0, 0, static_cast<int>(w * highdpi_), static_cast<int>(h * highdpi_));

    camera()->setScreenWidthAndHeight(w, h);
}

void PaintCanvas::init() {
}

void PaintCanvas::initializeGL() {

    QOpenGLWidget::initializeGL();
    func_ = context()->functions();
    func_->initializeOpenGLFunctions();

    OpenglUtil::init();


    if (!func_->hasOpenGLFeature(QOpenGLFunctions::Multisample)) {
       // window_->log->set_log(log_type::Warning, QString(tr("Multisample not supported on this machine! This application may not run properly.")).toStdString(), window_->getLogWidget());
        return;
    }
    if (!func_->hasOpenGLFeature(QOpenGLFunctions::Framebuffers)) {
       // window_->log->set_log(log_type::Warning, QString(tr("Framebuffer Object is not supported on this machine! This application may not run properly.")).toStdString(), window_->getLogWidget());
        return;

    }

    func_->glEnable(GL_DEPTH_TEST);

    func_->glClearDepthf(1.0f);
    func_->glClearColor(background_color_[0], background_color_[1], background_color_[2], background_color_[3]);


    int major_requested = QSurfaceFormat::defaultFormat().majorVersion();
    int minor_requested = QSurfaceFormat::defaultFormat().minorVersion();

    //// a = output to the console log
    //std::string a = "OpenGL vendor: "; a += (const char*)func_->glGetString(GL_VENDOR);
    //window_->log->set_log(log_type::Info, a, window_->getLogWidget());
    //a = "OpenGL renderer: "; a += (const char*)func_->glGetString(GL_RENDERER);
    //window_->log->set_log(log_type::Info, a, window_->getLogWidget());
    //a = "OpenGL version requested: "; a += std::to_string(major_requested); a += "."; a += std::to_string(minor_requested);
    //window_->log->set_log(log_type::Info, a, window_->getLogWidget());
    //a = "OpenGL version received: "; a += (const char*)func_->glGetString(GL_VERSION);
    //window_->log->set_log(log_type::Info, a, window_->getLogWidget());
    //a = "GLSL version received: "; a += (const char*)func_->glGetString(GL_SHADING_LANGUAGE_VERSION);
    //window_->log->set_log(log_type::Info, a, window_->getLogWidget());

    int major = 0;
    func_->glGetIntegerv(GL_MAJOR_VERSION, &major);
    int minor = 0;
    func_->glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major * 10 + minor < 32) {
        //window_->log->set_log(log_type::Error, QString(tr("This application requires at least OpenGL 3.2")).toStdString(), window_->getLogWidget());
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    dpi_scaling_ = static_cast<float>(devicePixelRatioF());
#else
    dpi_scaling_ = static_cast<float>(devicePixelRatio());
#endif

    //a = "DPI scaling: "; a += std::to_string(dpi_scaling());
    //window_->log->set_log(log_type::Info, a, window_->getLogWidget());


    // Calls user defined method.
    init();

    timer_.start();


}

void PaintCanvas::paintGL() {
    easy3d_debug_log_gl_error

#if 1
        // QOpenGLWidget renders everything into an FBO. Internally it changes
        // QSurfaceFormat to always have samples = 0 and the OpenGL context is
        // not a multisample context. So we have to query the render-buffer
        // to know if it is using multisampling. At initializeGL() we were not
        // able to query the actual samples because the internal FBO has not
        // been created yet, so we do it here.
        static bool queried = false;
    if (!queried) {
#if 1
        func_->glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples_);
        easy3d_debug_log_frame_buffer_error

#else   // the samples can also be retrieved using glGetIntegerv()
        func_->glGetIntegerv(GL_SAMPLES, &samples_); easy3d_debug_log_gl_error
#endif
            // warn the user if the expected request was not satisfied
            int samples = QSurfaceFormat::defaultFormat().samples();
        int max_num = 0;
        func_->glGetIntegerv(GL_MAX_SAMPLES, &max_num);

        // a = output to the console log
        std::string a;


        if (samples > 0 && samples_ != samples) {
            if (samples_ == 0) {
                //a = "MSAA is not available. ("; a += std::to_string(samples); a += " samples requested)";
                //window_->log->set_log(log_type::Warning, a, window_->getLogWidget());
            }
            else {
                //a = "MSAA is available with "; a += std::to_string(samples_); a += " samples. ("; a += std::to_string(samples); a += " requested but max support is "; a += std::to_string(max_num); a += ")";
                //window_->log->set_log(log_type::Warning, a, window_->getLogWidget());
            }
        }
        else {
            //a = "Samples received: "; a += std::to_string(samples_); a += ". ("; a += std::to_string(samples); a += " requested, max support is "; a += std::to_string(max_num); a += ")";
           // window_->log->set_log(log_type::Info, a, window_->getLogWidget());
        }

        queried = true;

    }
#endif

    preDraw();

    draw();

    // Add visual hints: axis, camera, grid...
    postDraw();

    Q_EMIT drawFinished();
}

void PaintCanvas::preDraw() {
    // For normal drawing, i.e., drawing triggered by the paintEvent(),
    // the clearing is done before entering paintGL().
    // If you want to reuse the paintGL() method for offscreen rendering,
    // you have to clear both color and depth buffers beforehand.
    //func_->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void PaintCanvas::draw() {
    if (models_.empty())
        return;

    easy3d_debug_log_gl_error
        // Optimization tips: rendering with multi-effects (e.g., shadowing, SSAO)
        // can benefit from sharing the same geometry pass.

        std::vector<TrianglesDrawable*> surfaces;
    for (auto m : models_) {
        if (!m->renderer()->is_visible())
            continue;
        for (auto d : m->renderer()->triangles_drawables())
            surfaces.push_back(d);
    }

    if (edl())
        edl()->begin();

    if (shadow()) {
        shadow()->draw(surfaces); easy3d_debug_log_gl_error
            return;
    }
    else if (ssao()) {
        auto ssao_texture = ssao()->generate(models());
        for (const auto m : models_) {
            if (!m->renderer()->is_visible())
                continue;
            for (auto d : m->renderer()->lines_drawables())
                d->set_ssao_texture(ssao_texture);
            for (auto d : m->renderer()->points_drawables())
                d->set_ssao_texture(ssao_texture);
            for (auto d : m->renderer()->triangles_drawables())
                d->set_ssao_texture(ssao_texture);
        }
    }


    easy3d_debug_log_gl_error

        for (const auto m : models_) {
            if (!m->renderer()->is_visible())
                continue;

            // Let's check if edges and surfaces are both shown. If true, we
            // make the depth coordinates of the surface smaller, so that displaying
            // the mesh and the surface together does not cause Z-fighting.
            std::size_t count = 0;
            for (auto d : m->renderer()->lines_drawables()) {
                if (d->is_visible()) {
                    d->draw(camera()); easy3d_debug_log_gl_error
                        ++count;
                }
            }

            for (auto d : m->renderer()->points_drawables()) {
                if (d->is_visible())
                    d->draw(camera()); easy3d_debug_log_gl_error

            }

            if (count > 0) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(0.5f, -0.0001f);
            }
            for (auto d : m->renderer()->triangles_drawables()) {
                if (d->is_visible())
                    d->draw(camera()); easy3d_debug_log_gl_error
            }
            if (count > 0)
                glDisable(GL_POLYGON_OFFSET_FILL);
        }

    if (edl())
        edl()->end();
}

void PaintCanvas::postDraw() {
    //easy3d_debug_log_gl_error

        //    if (show_frame_rate_) {
        //        // FPS computation
        //        static unsigned int fps_count = 0;
        //        static double fps = 0.0;
        //        static const unsigned int max_count = 40;
        //        static QString fps_string("fps: ??");
        //        if (++fps_count == max_count) {
        //            fps = 1000.0 * max_count / static_cast<double>(timer_.restart());
        //            fps_string = tr("fps: %1").arg(fps, 0, 'f', ((fps < 10.0) ? 1 : 0));
        //            fps_count = 0;
        //        }
        //
        //#if 0   // draw frame rate text using Easy3D's built-in TextRenderer
        //        if (texter_ && texter_->num_fonts() >= 2)
        //            texter_->draw(fps_string.toStdString(), 20.0f * dpi_scaling(), 50.0f * dpi_scaling(), 16, 1);
        //#else   // draw frame rate text using Qt.
        //        QPainter painter; easy3d_debug_log_gl_error
        //        painter.begin(this);
        //        painter.setRenderHint(QPainter::Antialiasing);
        //        painter.setRenderHint(QPainter::TextAntialiasing);
        //        painter.setPen(Qt::black);
        //        painter.beginNativePainting(); easy3d_debug_log_gl_error
        //        painter.drawText(20, 20, fps_string);
        //        painter.endNativePainting();
        //        painter.end();  easy3d_debug_log_gl_error
        //        func_->glEnable(GL_DEPTH_TEST); // it seems QPainter disables depth test?
        //#endif
        //    }

            //// shown only when it is not animating
            //if (walkThrough() && !walkThrough()->interpolator()->is_interpolation_started())
            //    walkThrough()->draw();
            //easy3d_debug_log_gl_error

            //if (show_primitive_id_under_mouse_) {
            //    drawPickedFaceAndItsVerticesIDs(Qt::darkBlue, Qt::darkMagenta);
            //    drawPickedVertexID(Qt::darkMagenta);
            //}


            // ------- draw the axes indicating the orientation of the model  ----------

        drawCornerAxes(); easy3d_debug_log_gl_error

        //    // ------------------ draw elements from the tool --------------------------

        tool_manager()->draw_hint();    easy3d_debug_log_gl_error


    // line between two selected points
    if (show_line) {
        //QPainter* p = new QPainter(this);
        drawLine(new QPainter(this));
    }

    // show model size
    if (showSize) {
        if (currentModel())
            currentModel()->renderer()->set_selected(true);
    }
    else {
        if (currentModel())
            currentModel()->renderer()->set_selected(false);
    }

    if (showPhoto) {
        drawPhoto(photoPath);
    }

    // gcp manual
    if (startGcp) {
        //QPainter* p = new QPainter(this);
        drawGcp(new QPainter(this));
    }

    // -------------------- draw the clipping plane ----------------------------

    ClippingPlane::instance()->draw(camera());    easy3d_debug_log_gl_error

        //    // -------------------- draw a sphere outline

        Model* m = currentModel();
    if (m && m->renderer()->is_visible() && m->renderer()->is_selected()) {
        m->manipulator()->draw_frame(camera_);
    }
    else if (show_manip_sphere_) {
        if (!drawable_manip_sphere_)
            drawable_manip_sphere_ = new LinesDrawable;

        const int radius = 150; // pixels
        float ratio = camera_->pixelGLRatio(camera_->pivotPoint());
        auto manip = mat4::translation(camera_->pivotPoint()) * mat4::scale(radius * ratio);
        shape::draw_sphere_big_circles(drawable_manip_sphere_, camera_->modelViewProjectionMatrix(), manip);
    }
}


void PaintCanvas::mousePressEvent(QMouseEvent* e) {
    pressed_button_ = e->button();
    modifiers_ = e->modifiers();
    mouse_current_pos_ = e->pos();
    mouse_pressed_pos_ = e->pos();

    if (tool_manager()->current_tool()) {
        tools::ToolButton bt = tools::NO_BUTTON;
        if (e->button() == Qt::LeftButton)
            bt = tools::LEFT_BUTTON;
        else if (e->button() == Qt::RightButton)
            bt = tools::RIGHT_BUTTON;
        else if (e->button() == Qt::MiddleButton)
            bt = tools::MIDDLE_BUTTON;
        if (bt != tools::NO_BUTTON) {
            makeCurrent();
            tool_manager()->press(bt, e->pos().x(), e->pos().y());
            doneCurrent();

        }


    }
    else {
        if (e->modifiers() == Qt::AltModifier) {
            bool found = false;
            const vec3 p = pointUnderPixel(e->pos(), found);
            if (found && (walkThrough()->status() == WalkThrough::WALKING_MODE) &&
                (!walkThrough()->interpolator()->is_interpolation_started()))
            {
                walkThrough()->walk_to(p);
                update();
            }
            else if (walkThrough()->status() == WalkThrough::ROTATE_AROUND_AXIS)
            {
                if (walkThrough()->interpolator()->number_of_keyframes() > 0) {
                    auto answer = dialog::message("Rotate camera around axis", "Overwrite the existing camera path?", dialog::Choice::ok_cancel, dialog::Type::warning);
                    if (answer == dialog::Response::cancel)
                        return;
                    else if (answer == dialog::Response::ok)
                        walkThrough()->interpolator()->delete_path();
                }

                Picker picker(camera_);
                const Line3 line = picker.picking_line(e->pos().x(), e->pos().y());
                walkThrough()->generate_camera_path(line);
                update();
            }
            else if (walkThrough()->status() == WalkThrough::FREE_MODE) {
                LOG(WARNING) << "press 'K' to add a keyframe in the free mode";
            }
        }
        else {
            if ((e->button() == Qt::LeftButton || e->button() == Qt::RightButton) && e->modifiers() != Qt::AltModifier && e->modifiers() != Qt::ControlModifier)
                if (window_show_manip_sphere_)show_manip_sphere_ = true;

            if (pressed_key_ == Qt::Key_Z)
                showSize = true;

            if (showPhoto && e->button() == Qt::LeftButton && pressed_key_ == Qt::Key_K) {// 
                bool found = false;
                const easy3d::vec3 p = pointUnderPixel(e->pos(), found);
                if (found) {
                    cameraPoint = getCameraPoint(all_coor, p);
                }
                else {
                    std::cout << "not found" << std::endl;
                }

            }

            //if (window_->addGcp && e->button() == Qt::LeftButton) {
            //    bool found = false;
            //    const easy3d::vec3 p = pointUnderPixel(e->pos(), found);
            //    if (found) {
            //        window_->newGcpXYZ = p;
            //        window_->_gcp_addGcp_found();

            //    }
            //}

            //if (e->modifiers() == Qt::NoModifier && e->button() == Qt::LeftButton &&
            //    walkThrough()->status() == WalkThrough::STOPPED && allow_select_model_) {
            //    if (!model_picker_)
            //        model_picker_ = new ModelPicker(camera());
            //    makeCurrent();
            //    auto model = model_picker_->pick(models(), e->pos().x(), e->pos().y());
            //    doneCurrent();
            //    if (model) {
            //        setCurrentModel(model);
            //        window_->updateUi();
            //    }
            //    // select the picked model (if picked) and deselect others
            //    for (auto m : models_)
            //        m->renderer()->set_selected(m == model);
            //}

            //else {
            camera_->frame()->action_start();

            if (e->modifiers() == Qt::ShiftModifier) {
                if (e->button() == Qt::LeftButton) {
                    bool found = false;
                    const vec3& p = pointUnderPixel(e->pos(), found);
                    if (found)
                        camera_->setPivotPoint(p);
                }

            }
            //}
            update();
        }
    }

    QOpenGLWidget::mousePressEvent(e);
}


void PaintCanvas::mouseReleaseEvent(QMouseEvent* e) {
    if (tool_manager()->current_tool()) {
        tools::ToolButton bt = tools::NO_BUTTON;
        if (e->button() == Qt::LeftButton) {
            bt = tools::LEFT_BUTTON;
        }
        else if (e->button() == Qt::RightButton)
            bt = tools::RIGHT_BUTTON;
        else if (e->button() == Qt::MiddleButton)
            bt = tools::MIDDLE_BUTTON;
        if (bt != tools::NO_BUTTON) {
            makeCurrent();
            tool_manager()->release(bt, e->pos().x(), e->pos().y());
            doneCurrent();
            window_->updateRenderingPanel();

            QString coords = "XYZ = [-, -, -] ";
            bool found = false;
            const easy3d::vec3 p = pointUnderPixel(e->pos(), found);

            if (window_->pointClick && !window_->gcpManual) {
                          
                //QString length;
                if (found && bt == tools::LEFT_BUTTON) {

                    coords = QString("XYZ = [%1, %2, %3] ").arg(p.x).arg(p.y).arg(p.z);
                    no_of_points++;
                    first_two_points.push_back(p); // points
                    to_show_line.push_back(e->pos()); // lines btw 2 points

                    if (no_of_points == 2) {
 
                        float length_in_float = getDistBtwTwoPts(first_two_points.at(1), first_two_points.at(0));
                        //length = tr("Distance between two points: ") + QString::number(length_in_float);

                        // line btw points
                        show_line = true;
                        lines.push_back(length_in_float);

                        // two points
                        first_two_points.clear();
                        no_of_points = 0;
                    }
                    std::string coordinates = coords.toStdString();
                    window_->setStatus(coordinates);
                    //window_->log->set_log(log_type::Info, (coords + length).toStdString(), window_->getLogWidget());
                }

            }
            if (window_->gcpManual) {
                if (found && bt == tools::LEFT_BUTTON) {
                    startGcp = true;
                    gcp = e->pos();
                    coords = QString("XYZ = [%1, %2, %3] ").arg(p.x).arg(p.y).arg(p.z);
                    std::string coordinates = coords.toStdString();
                    window_->setStatus(coordinates);
                    window_->toAddxyzGcp(p.x, p.y, p.z);
                }
            }
        }
        update();
    }
    //else if (pressed_button_ == Qt::LeftButton && e->modifiers() == Qt::ControlModifier) { // ZOOM_ON_REGION
    //    /*int xmin = std::min(mouse_pressed_pos_.x(), e->pos().x());
    //    int xmax = std::max(mouse_pressed_pos_.x(), e->pos().x());
    //    int ymin = std::min(mouse_pressed_pos_.y(), e->pos().y());
    //    int ymax = std::max(mouse_pressed_pos_.y(), e->pos().y());
    //    camera_->fitScreenRegion(xmin, ymin, xmax, ymax);*/
    //    
    //}
    else {
        if (e->button() == Qt::LeftButton || e->button() == Qt::RightButton) {
            show_manip_sphere_ = false;
            showSize = false;
        }

        camera_->frame()->action_end();

        update();
    }

    pressed_button_ = Qt::NoButton;
    modifiers_ = Qt::NoModifier;
    mouse_current_pos_ = e->pos();

    QOpenGLWidget::mouseReleaseEvent(e);
}


void PaintCanvas::mouseMoveEvent(QMouseEvent* e) {
    int x = e->pos().x(), y = e->pos().y();
    if (x < 0 || x > width() || y < 0 || y > height() || walkThrough()->interpolator()->is_interpolation_started()) {
        e->ignore();
        return;
    }

    if (tool_manager()->current_tool()) {
        pressed_button_ = Qt::NoButton;
        tools::ToolButton bt = tools::NO_BUTTON;
        if (e->buttons() == Qt::LeftButton) {
            pressed_button_ = Qt::LeftButton;
            bt = tools::LEFT_BUTTON;
        }
        else if (e->buttons() == Qt::RightButton) {
            pressed_button_ = Qt::RightButton;
            bt = tools::RIGHT_BUTTON;
        }
        else if (e->buttons() == Qt::MiddleButton) {
            pressed_button_ = Qt::MiddleButton;
            bt = tools::MIDDLE_BUTTON;
        }

        makeCurrent();
        tool_manager()->move(bt, e->pos().x(), e->pos().y());
        doneCurrent();
        update();
    }
    else {
        // control modifier is reserved for zooming on region
        if (pressed_button_ != Qt::NoButton && modifiers_ != Qt::ControlModifier) {
            ManipulatedFrame* frame = camera()->frame();
            if (e->modifiers() == Qt::AltModifier) {
                if (ClippingPlane::instance()->is_enabled())
                    frame = ClippingPlane::instance()->manipulator()->frame();
                else if (currentModel() && currentModel()->renderer()->is_selected()) {
                    frame = currentModel()->manipulator()->frame();
                }
            }

            int dx = x - mouse_current_pos_.x();
            int dy = y - mouse_current_pos_.y();
            auto axis = ManipulatedFrame::NONE;
            if (pressed_button_ == Qt::LeftButton) {
                frame->action_rotate(x, y, dx, dy, camera_, axis);
            }
            else if (pressed_button_ == Qt::RightButton) {
                frame->action_translate(x, y, dx, dy, camera_, axis);
            }
            else if (pressed_button_ == Qt::MiddleButton) {
                if (dy != 0 && frame == camera_->frame()) // zoom is intended for camera manipulation only
                    frame->action_zoom(dy > 0 ? 1 : -1, camera_);
            }
        }
    }

    //if (show_primitive_id_under_mouse_ || show_primitive_property_under_mouse_) { // then need to pick primitives
    //    picked_face_index_ = -1;
    //    picked_vertex_index_ = -1;
    //    if (dynamic_cast<SurfaceMesh*>(currentModel())) {
    //        auto mesh = dynamic_cast<SurfaceMesh*>(currentModel());
    //        if (!surface_mesh_picker_)
    //            surface_mesh_picker_ = new SurfaceMeshPicker(camera());
    //        makeCurrent();
    //        picked_face_index_ = surface_mesh_picker_->pick_face(mesh, e->pos().x(), e->pos().y()).idx();
    //        doneCurrent();

    //        auto drawable = mesh->renderer()->get_triangles_drawable("faces");
    //        if (picked_face_index_ >= 0) { // highlight the picked face
    //            drawable->set_highlight(true);
    //            drawable->set_highlight_range({ picked_face_index_, picked_face_index_ });
    //        }
    //        else {
    //            drawable->set_highlight(false);
    //            drawable->set_highlight_range({ -1, -1 });
    //        }
    //        update();
    //    }
    //    else if (dynamic_cast<PointCloud*>(currentModel())) {
    //        auto cloud = dynamic_cast<PointCloud*>(currentModel());
    //        if (!point_cloud_picker_)
    //            point_cloud_picker_ = new PointCloudPicker(camera());
    //        makeCurrent();
    //        picked_vertex_index_ = point_cloud_picker_->pick_vertex(cloud, e->pos().x(), e->pos().y()).idx();
    //        doneCurrent();

    //        auto drawable = cloud->renderer()->get_points_drawable("vertices");
    //        if (picked_vertex_index_ >= 0) { // highlight the picked vertex
    //            drawable->set_highlight(true);
    //            drawable->set_highlight_range({ picked_vertex_index_, picked_vertex_index_ });
    //        }
    //        else {
    //            drawable->set_highlight(false);
    //            drawable->set_highlight_range({ -1, -1 });
    //        }
    //        update();
    //    }

    //    if (show_primitive_property_under_mouse_) {
    //        if (picked_vertex_index_ >= 0) { // picked point cloud vertex
    //            auto drawable = currentModel()->renderer()->get_points_drawable("vertices");
    //            if (drawable->coloring_method() == easy3d::State::SCALAR_FIELD) {
    //                const std::string& name = drawable->property_name();
    //                auto vertex = PointCloud::Vertex(picked_vertex_index_);
    //                auto cloud = dynamic_cast<PointCloud*>(currentModel());
    //                const std::string value_str = internal::get_vertex_scalar_property(cloud, vertex, name);
    //                std::stringstream stream;
    //                stream << "'" << name << "' on " << vertex << ": " << value_str;
    //                window_->statusBar()->showMessage(QString::fromStdString(stream.str()), 2000);
    //            }
    //            else
    //                window_->statusBar()->showMessage("no scalar property is being visualized", 2000);
    //        }
    //        else if (picked_face_index_ >= 0) { // picked surface mesh face
    //            auto drawable = currentModel()->renderer()->get_triangles_drawable("faces");
    //            if (drawable->coloring_method() == easy3d::State::SCALAR_FIELD && drawable->property_location() == easy3d::State::FACE) {
    //                const std::string& name = drawable->property_name();
    //                auto face = SurfaceMesh::Face(picked_face_index_);
    //                auto mesh = dynamic_cast<SurfaceMesh*>(currentModel());
    //                const std::string value_str = internal::get_face_scalar_property(mesh, face, name);
    //                std::stringstream stream;
    //                stream << "'" << name << "' on " << face << ": " << value_str;
    //                window_->statusBar()->showMessage(QString::fromStdString(stream.str()), 2000);
    //            }
    //            else
    //                window_->statusBar()->showMessage("no scalar property is being visualized", 2000);
    //        }
    //        else
    //            window_->statusBar()->showMessage("no primitive found under mouse", 2000);
    //    }
    //}

    mouse_current_pos_ = e->pos();
    QOpenGLWidget::mouseMoveEvent(e);

}


void PaintCanvas::wheelEvent(QWheelEvent* e) {
    if (walkThrough()->interpolator()->is_interpolation_started())
        return;

    if (tool_manager()->current_tool()) {
        update();
    }
    else {
        const int delta = e->angleDelta().y();
        if (delta <= -1 || delta >= 1) {
            int dy = e->angleDelta().y() > 0 ? 1 : -1;
            camera_->frame()->action_zoom(dy, camera_);

            //// get zoom dist
            //static const float WHEEL_SENSITIVITY_COEFF = 0.1f;
            //const float delta1 = static_cast<float>(dy) * 1.0 * WHEEL_SENSITIVITY_COEFF;
            //const vec3& direction = camera_->pivotPoint() - camera_->frame()->position();
            //const vec3& new_pos = camera_->frame()->position() + delta1 * direction;
            //const vec3& offset = camera_->pivotPoint() - new_pos;
            ///*float a = offset.length();
            //a = this->height() / (2 * tan(22.5) * (0.1 + 0.05));
            //zoomDist = a;*/
            //zoomDist = offset.length();
            //// =============

        }
    }

    QOpenGLWidget::wheelEvent(e); //this forwards the event(wheelevent) to the QOpenGLWidget
}
void PaintCanvas::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Space && e->modifiers() == Qt::NoModifier) {
        if (currentModel())
            fitScreen(currentModel());

    }
    else if (e->key() == Qt::Key_Minus && e->modifiers() == Qt::NoModifier) {
        for (auto m : models_) {
            for (auto d : m->renderer()->points_drawables()) {
                float size = d->point_size() - 1.0f;
                if (size < 1)
                    size = 1;
                d->set_point_size(size);
                window_->updateRenderingPanel();
            }
        }
    }
    else if (e->key() == Qt::Key_Equal && e->modifiers() == Qt::NoModifier) {
        for (auto m : models_) {
            for (auto d : m->renderer()->points_drawables()) {
                float size = d->point_size() + 1.0f;
                d->set_point_size(size);
                window_->updateRenderingPanel();
            }
        }
    }
    else if (e->key() == Qt::Key_F && e->modifiers() == Qt::NoModifier) {
        fitScreen(nullptr);
    }
    else if (e->key() == Qt::Key_L && e->modifiers() == Qt::NoModifier) {
        if (walkThrough()->status() == WalkThrough::FREE_MODE) {
            const vec3 pos = camera()->position();
            const quat q = camera()->orientation();
            walkThrough()->add_keyframe(Frame(pos, q));
        }
        else if (walkThrough()->status() == WalkThrough::WALKING_MODE)
            LOG(WARNING) << "'L' is for the free mode only. Use Alt + Left click to add a keyframe in the walking mode";
    }
    else {
        // for showing size now
        pressed_key_ = e->key();
    }


    QOpenGLWidget::keyPressEvent(e);
    update();
}

void PaintCanvas::keyReleaseEvent(QKeyEvent* e) {

    pressed_key_ = -1;

    QOpenGLWidget::keyReleaseEvent(e);
    update();
}


void PaintCanvas::mouseDoubleClickEvent(QMouseEvent* e) {
    mouse_current_pos_ = e->pos();
    if (e->button() == Qt::LeftButton) {
        bool found = false;
        const vec3& p = pointUnderPixel(e->pos(), found);
        if (found)
            camera_->setPivotPoint(p);

    }
    QOpenGLWidget::mouseDoubleClickEvent(e);
    update();
}


void PaintCanvas::drawCornerAxes() {
    ShaderProgram* program = ShaderManager::get_program("surface/surface");
    if (!program) {
        std::vector<ShaderProgram::Attribute> attributes;
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::TEXCOORD, "vtx_texcoord"));
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
        attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
        program = ShaderManager::create_program_from_files("surface/surface", attributes);
    }
    if (!program)
        return;

    if (!drawable_axes_) {
        const float base = 0.5f;   // the cylinder length, relative to the allowed region
        const float head = 0.2f;   // the cone length, relative to the allowed region
        std::vector<vec3> points, normals, colors;
        shape::create_cylinder(0.03, 10, vec3(0, 0, 0), vec3(base, 0, 0), vec3(1, 0, 0), points, normals, colors);
        shape::create_cylinder(0.03, 10, vec3(0, 0, 0), vec3(0, base, 0), vec3(0, 1, 0), points, normals, colors);
        shape::create_cylinder(0.03, 10, vec3(0, 0, 0), vec3(0, 0, base), vec3(0, 0, 1), points, normals, colors);
        shape::create_cone(0.06, 20, vec3(base, 0, 0), vec3(base + head, 0, 0), vec3(1, 0, 0), points, normals,
            colors);
        shape::create_cone(0.06, 20, vec3(0, base, 0), vec3(0, base + head, 0), vec3(0, 1, 0), points, normals,
            colors);
        shape::create_cone(0.06, 20, vec3(0, 0, base), vec3(0, 0, base + head), vec3(0, 0, 1), points, normals,
            colors);
        shape::create_sphere(vec3(0, 0, 0), 0.06, 20, 20, vec3(0, 1, 1), points, normals, colors);
        drawable_axes_ = new TrianglesDrawable("corner_axes");
        drawable_axes_->update_vertex_buffer(points);
        drawable_axes_->update_normal_buffer(normals);
        drawable_axes_->update_color_buffer(colors);
        drawable_axes_->set_property_coloring(State::VERTEX);
    }
    if (!drawable_axes_->is_visible())
        return;

    // The viewport is changed to fit the lower left corner.
    int viewport[4];
    func_->glGetIntegerv(GL_VIEWPORT, viewport);

    static const int corner_frame_size = static_cast<int>(100 * dpi_scaling());
    func_->glViewport(0, 0, corner_frame_size, corner_frame_size);

    // To make the axis appear over other objects: reserve a tiny bit of the
    // front depth range. NOTE: do remember to restore it later.
    func_->glDepthRangef(0, 0.01f);

    const mat4& proj = transform::ortho(-1, 1, -1, 1, -1, 1);
    const mat4& view = camera_->orientation().inverse().matrix();
    const mat4& MVP = proj * view;

    // camera position is defined in world coordinate system.
    const vec3& wCamPos = camera_->position();
    // it can also be computed as follows:
    //const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
    const mat4& MV = camera_->modelViewMatrix();
    const vec4& wLightPos = inverse(MV) * setting::light_position;

    program->bind();
    program->set_uniform("MVP", MVP)
        ->set_uniform("MANIP", mat4::identity())
        ->set_uniform("NORMAL", mat3::identity())   // needs be padded when using uniform blocks
        ->set_uniform("lighting", true)
        ->set_uniform("two_sides_lighting", false)
        ->set_uniform("smooth_shading", true)
        ->set_uniform("wLightPos", wLightPos)
        ->set_uniform("wCamPos", wCamPos)
        ->set_uniform("ssaoEnabled", false)
        ->set_uniform("per_vertex_color", true)
        ->set_uniform("distinct_back_color", false)
        ->set_block_uniform("Material", "ambient", setting::material_ambient)
        ->set_block_uniform("Material", "specular", setting::material_specular)
        ->set_block_uniform("Material", "shininess", &setting::material_shininess)
        ->set_uniform("highlight", false)
        ->set_uniform("clippingPlaneEnabled", false)
        ->set_uniform("selected", false)
        ->set_uniform("highlight_color", setting::highlight_color)
        ->set_uniform("use_texture", false);  easy3d_debug_log_gl_error

        drawable_axes_->gl_draw(); easy3d_debug_log_gl_error
        program->release();

    // restore the viewport
    func_->glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    func_->glDepthRangef(0.0f, 1.0f);

}

void PaintCanvas::drawLine(QPainter* p) {

    //p->begin(this);
    p->setRenderHint(QPainter::Antialiasing);
    p->setPen(Qt::red);
    p->beginNativePainting();
    
    int index = 0;
    for (int i = 0; i < to_show_line.size() - 1; i += 2) {
        int last = i + 1;
        int prevlast = i;
        p->drawLine(to_show_line.at(prevlast).x(), to_show_line.at(prevlast).y(), to_show_line.at(last).x(), to_show_line.at(last).y());
        p->setRenderHint(QPainter::TextAntialiasing);
        p->setFont(QFont("Microsoft YaHei UI", 13));

        QString t;
        t += QString::number(lines.at(index));// +QString(" in 3D , ");

        p->drawText((to_show_line.at(prevlast).x() + to_show_line.at(last).x()) / 2, (to_show_line.at(prevlast).y() + to_show_line.at(last).y()) / 2, t);
        
        index++;
        p->endNativePainting();
    }
    
    
    p->end();
    func_->glEnable(GL_DEPTH_TEST); // it seems QPainter disables depth test?

    //delete p;
    //QPainter painter(this);
    //painter.setBrush(QBrush(Qt::green));
    //painter.drawEllipse(to_show_line.at(0).x(), to_show_line.at(0).y(), 5, 5);
}
void PaintCanvas::drawGcp(QPainter* p) {

    //p->setRenderHint(QPainter::Antialiasing);
    p->setBrush(Qt::green);
    p->beginNativePainting();
    p->drawEllipse(gcp.x()-10, gcp.y()-10, 20, 20);
    p->endNativePainting();
    p->end();
    func_->glEnable(GL_DEPTH_TEST); // it seems QPainter disables depth test?

}

void PaintCanvas::drawPhoto(QString imagePath) {

    QString image;

    QDir dir(imagePath);
    for (const QFileInfo& file : dir.entryInfoList(QDir::Files))
    {

        int dest_pic = picName.at(cameraPoint);
        int current_pic_index = file.fileName().left(file.fileName().lastIndexOf(".")).toInt();
        if (dest_pic == current_pic_index) {
            image = file.absoluteFilePath();
            break;
        }

    }

    photo_texture_ = Texture::create(image.toStdString());
    if (!photo_texture_) {
        //window_->log->set_log(log_type::Warning, QString(tr("Invalid image path.")).toStdString(), window_->getLogWidget());
        return;
    }
    // pic
    //const Rect image_rect = Rect(static_cast<float>(10), static_cast<float>(width() / 3), height() * 0.1, static_cast<float>(height() * 0.8));
    const Rect image_rect = Rect(static_cast<float>(10), static_cast<float>(width() * 0.4), 25, static_cast<float>(width() / 3) * 3 / 4);
    const Rect quad(image_rect.x_min(), image_rect.x_max(),
        image_rect.y_min(), image_rect.y_max());

    const int w = static_cast<int>(static_cast<float>(width()));
    const int h = static_cast<int>(static_cast<float>(height()));
    shape::draw_quad_filled(quad, photo_texture_->id(), w, h, -0.9f);
    shape::draw_quad_wire(quad, vec4(0.98f, 0.63f, 0.16f, 1.0f), w, h, -0.99f);

}


Model* PaintCanvas::currentModel() {
    if (models_.empty()) {
        //std::cout << " c1 " << std::endl;
        return nullptr;
    }
    if (model_idx_ < models_.size()) {
        //std::cout << " c2 " << std::endl;
        return models_[model_idx_];
    }
    //std::cout << " c3 " << std::endl;
    return nullptr;
}

vec3 PaintCanvas::pointUnderPixel(const QPoint& p, bool& found) const {
    // Qt (same as GLFW) uses upper corner for its origin while GL uses the lower corner.
    int glx = p.x();
    int gly = height() - 1 - p.y();

    // NOTE: when dealing with OpenGL, all positions are relative to the viewer port.
    //       So we have to handle highdpi displays.
    glx = static_cast<int>(static_cast<float>(glx) * dpi_scaling());
    gly = static_cast<int>(static_cast<float>(gly) * dpi_scaling());

    const_cast<PaintCanvas*>(this)->makeCurrent();

    int samples = 0;
    func_->glGetIntegerv(GL_SAMPLES, &samples);
    easy3d_debug_log_gl_error

        float depth = 1.0f;
    if (samples > 0) {
        opengl::read_depth_ms(depth, glx, gly);
        easy3d_debug_log_gl_error
    }
    else {
        opengl::read_depth(depth, glx, gly);
        easy3d_debug_log_gl_error
    }

    const_cast<PaintCanvas*>(this)->doneCurrent();

    found = depth < 1.0f;
    if (found) {
        // The input to unprojectedCoordinatesOf() is defined in the screen coordinate system
        return camera_->unprojectedCoordinatesOf(vec3(static_cast<float>(p.x()), static_cast<float>(p.y()), depth));
    }

    return {};
}

void PaintCanvas::addModel(Model* model) {

    // temp = output to the console log
    QString temp;
    if (!model) {

        //temp = QString(tr("model is NULL."));
        //window_->log->set_log(log_type::Warning, temp.toStdString(), window_->getLogWidget());
        return;
    }
    for (auto m : models_) {
        if (model == m) {

            /*temp = QString(tr("model '%1' has already been added to the viewer.")).arg(QString::fromStdString(file_system::simple_name(model->name())));
            window_->log->set_log(log_type::Warning, temp.toStdString(), window_->getLogWidget());*/
            return;
        }
    }

    if (model->empty()) {
        /*temp = QString(tr("model does not have vertices. Only complete model can be added to the viewer."));
        window_->log->set_log(log_type::Warning, temp.toStdString(), window_->getLogWidget());*/
        return;
    }

    makeCurrent();
    model->set_renderer(new Renderer(model));
    model->set_manipulator(new Manipulator(model));
    model->manipulator()->frame()->modified.connect(this, static_cast<void (PaintCanvas::*)(void)>(&PaintCanvas::update));
    doneCurrent();

    models_.push_back(model);
    model_idx_ = static_cast<int>(models_.size()) - 1; // make the last one current
    adjustSceneRadius();

}

void PaintCanvas::adjustSceneRadius() {
    Box3 box;
    for (auto m : models_)
        box.grow(m->bounding_box(false));

    const std::size_t count = walkThrough()->interpolator()->number_of_keyframes();
    for (std::size_t i = 0; i < count; ++i)
        box.grow(walkThrough()->interpolator()->keyframe(i).position());

    // attention: the scene center is not changed.
    // Note: This is not accurate and just an estimate. If you see problem in rendering, use the max distance
    //       from the scene center to the eight vertices of the box.
    camera()->setSceneRadius(
        std::max(
            distance(camera()->sceneCenter(), box.min_point()),
            distance(camera()->sceneCenter(), box.max_point())
        )
    );
}

void PaintCanvas::fitScreen(const easy3d::Model* model) {
    if (!model && models_.empty())
        return;

    Box3 box_models;   // everything within box_models should be visible
    for (auto m : models_) {
        if (m->renderer()->is_visible())
            box_models.grow(m->bounding_box(false));
    }

    Box3 box_scene = box_models;   // everything within box_scene should be visible
    const std::size_t count = walkThrough()->interpolator()->number_of_keyframes();
    for (std::size_t i = 0; i < count; ++i)
        box_scene.grow(walkThrough()->interpolator()->keyframe(i).position());

    Box3 box;// this box will be fitted to the screen
    if (model && model->renderer()->is_visible())
        box = model->bounding_box(false);
    else
        box = box_models;

    if (!box.is_valid())
        return;

    // the proper scene radius (ensure everything within it will be visible) should be max of all distances from box's
    // center to all the vertices of box_scene
    float radius = box.radius();
    const std::vector<vec3> vts = {
            vec3(box_scene.min_coord(0), box_scene.min_coord(1), box_scene.min_coord(2)), // 0
            vec3(box_scene.max_coord(0), box_scene.min_coord(1), box_scene.min_coord(2)), // 1
            vec3(box_scene.max_coord(0), box_scene.max_coord(1), box_scene.min_coord(2)), // 2
            vec3(box_scene.min_coord(0), box_scene.max_coord(1), box_scene.min_coord(2)), // 3
            vec3(box_scene.min_coord(0), box_scene.min_coord(1), box_scene.max_coord(2)), // 4
            vec3(box_scene.max_coord(0), box_scene.min_coord(1), box_scene.max_coord(2)), // 5
            vec3(box_scene.max_coord(0), box_scene.max_coord(1), box_scene.max_coord(2)), // 6
            vec3(box_scene.min_coord(0), box_scene.max_coord(1), box_scene.max_coord(2)), // 7
    };
    for (const auto& p : vts)
        radius = std::max(radius, distance(box.center(), p));

    camera_->setSceneCenter(box.center());
    // make it a little bigger to ensure tiny objects (far away from other big objects) are also visible
    camera_->setSceneRadius(radius);
    camera_->fitBoundingBox(box.min_point(), box.max_point());
    update();
}

void PaintCanvas::deleteModel(Model* model) {
    if (!model) {
        LOG(WARNING) << "model is NULL.";
        return;
    }

    auto pos = std::find(models_.begin(), models_.end(), model);
    if (pos != models_.end()) {
        const std::string name = model->name();
        models_.erase(pos);
        makeCurrent();
        delete model->renderer();
        delete model->manipulator();
        delete model;
        doneCurrent();
        model_idx_ = static_cast<int>(models_.size()) - 1; // make the last one current

       /* QString temp;
        temp = QString(tr("model deleted: %1")).arg(QString::fromStdString(name));
        window_->log->set_log(log_type::Info, temp.toStdString(), window_->getLogWidget());*/

    }
    else
        LOG(WARNING) << "no such model: " << model->name();
}


void PaintCanvas::setCurrentModel(easy3d::Model* m) {
    for (int i = 0; i < models_.size(); ++i) {
        if (models_[i] == m) {
            model_idx_ = i;
            return;
        }
    }
}


void PaintCanvas::fitScreen() {
    fitScreen(nullptr);
}

void PaintCanvas::enableSsao(bool b) {
    if (b) {
        if (!ssao_)
            ssao_ = new AmbientOcclusion(camera_);
    }
    else {
        delete ssao_;
        ssao_ = nullptr;
    }

    for (const auto m : models_) {
        for (auto d : m->renderer()->lines_drawables()) {
            d->enable_ssao(b);
        }
        for (auto d : m->renderer()->points_drawables()) {
            d->enable_ssao(b);
        }
        for (auto d : m->renderer()->triangles_drawables()) {
            d->enable_ssao(b);
        }
    }
}

void PaintCanvas::enableShadow(bool b) {
    if (b) {
        if (!shadow_)
            shadow_ = new SoftShadow(camera_);

    }
    else {
        delete shadow_;
        shadow_ = nullptr;
    }
}

void PaintCanvas::enableEyeDomeLighting(bool b) {
    if (b) {
        if (!edl_)
            edl_ = new EyeDomeLighting(camera_);
    }
    else {
        delete edl_;
        edl_ = nullptr;
    }
}

void PaintCanvas::setCenterPivot(bool b) {
    window_show_manip_sphere_ = b;
}

void PaintCanvas::_PersOrtho_triggered(bool b) {
    if (b) {
        camera_->setType(Camera::PERSPECTIVE);
        //window_->log->set_log(log_type::Info, QString(tr("Camera set to Perspective mode")).toStdString(), window_->getLogWidget());
    }
    else {
        camera_->setType(Camera::ORTHOGRAPHIC);
        //window_->log->set_log(log_type::Info, QString(tr("Camera set to Orthographic mode")).toStdString(), window_->getLogWidget());
    }
    update();

}

//std::vector<float> PaintCanvas::getAxisFacing(easy3d::vec3 v) {
//    float temp = 0.0f;
//    int count = 0;
//    if (abs(temp) < abs(v.x)) {
//        temp = (v.x);
//    }
//    if (abs(temp) < abs(v.y)) {
//        temp = (v.y);
//    }
//    if (abs(temp) < abs(v.z)) {
//        temp = (v.z);
//    }
//
//    if (abs(temp) == abs(v.x))return { 0,temp };
//    else if (abs(temp) == abs(v.y))return { 1,temp };
//    else if (abs(temp) == abs(v.z))return{ 2,temp };
//}

float PaintCanvas::getDistBtwTwoPts(vec3 a, vec3 b)
{
    easy3d::vec3 length_in_vec3 = a - b;
    //float length_in_float = length_in_vec3.length();
    return length_in_vec3.length();
}

int PaintCanvas::getCameraPoint(std::vector<easy3d::vec3> list, easy3d::vec3 current)
{

    int current_index = 0;
    float shortest_dist = FLT_MAX;
    for (int i = 0; i < list.size(); i++) {
        float temp = getDistBtwTwoPts(list.at(i), current);
        if (temp < shortest_dist) {
            current_index = i;
            shortest_dist = temp;
        }
    }
    return current_index;
}

void PaintCanvas::pov(int pos)
{
    Frame frame;
    frame.setTranslation(camera_->pivotPoint());

    float threshold = 0.0f;
    vec3 directions[2][3];
    for (unsigned short d = 0; d < 3; ++d) {
        vec3 dir((d == 0) ? 1.0f : 0.0f, (d == 1) ? 1.0f : 0.0f, (d == 2) ? 1.0f : 0.0f);

        if (&frame) {
            directions[0][d] = (&frame)->inverseTransformOf(dir);
        }
        else {
            directions[0][d] = dir;
        }
        directions[1][d] = camera_->frame()->inverseTransformOf(dir);
    }

    float maxProj = 0.0f;
    unsigned short index[2];
    index[0] = index[1] = 0;
    for (unsigned short i = 0; i < 3; ++i)
        for (unsigned short j = 0; j < 3; ++j) {
            const float proj = fabs(dot(directions[0][i], directions[1][j]));
            if (proj >= maxProj) {
                index[0] = i;
                index[1] = j;
                maxProj = proj;
            }
        }

    Frame old;
    old = *camera_->frame();

    const float coef = dot(directions[0][index[0]], directions[1][index[1]]);
    vec3 axis = cross(directions[0][index[0]], directions[1][index[1]]);

    if (std::fabs(coef) >= threshold) {

        float angle = std::asin(axis.norm());

        if (coef >= 0.0)
            angle = -angle;
        camera_->frame()->rotate(camera_->frame()->rotation().inverse() * quat(axis, angle) * camera_->frame()->orientation());

        // Try to align another axis direction
        unsigned short d = (index[1] + 1) % 3;
        vec3 dir((d == 0) ? 1.0f : 0.0f, (d == 1) ? 1.0f : 0.0f, (d == 2) ? 1.0f : 0.0f);
        dir = camera_->frame()->inverseTransformOf(dir);


        float max = 0.0f;
        for (unsigned short i = 0; i < 3; ++i) {
            const float proj = std::fabs(dot(directions[0][i], dir));
            if (proj > max) {
                index[0] = i;
                max = proj;
            }
        }

        if (max >= threshold) {
            axis = cross(directions[0][index[0]], dir);
            angle = std::asin(axis.norm());

            if (dot(directions[0][index[0]], dir) >= 0.0f)
                angle = -angle;
            camera_->frame()->rotate(camera_->frame()->rotation().inverse() * quat(axis, angle) * camera_->frame()->orientation());


        }
    }

    //std::vector<float> facing = getAxisFacing(camera_->viewDirection());
    //std::cout << "axis: " << std::to_string(facing.at(0)) << " pov" << std::to_string(facing.at(1)) << std::endl;


    if (pos == 0) {
        camera_->setViewDirection(vec3(0, 0, 1));
        camera_->setOrientation(quat(vec3(0, 0, 1), 0));

    }
    else if (pos == 1) {
        camera_->setViewDirection(vec3(0, 0, 1));
        camera_->setOrientation(quat(vec3(0, 0, 1), 0));
        camera_->setViewDirection(vec3(0, 0, 1));
    }
    else if (pos == 2) {
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setOrientation(quat(vec3(0, 1, 0), 0));
        camera_->setViewDirection(vec3(0, 1, 0));
    }
    else if (pos == 3) {
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setOrientation(quat(vec3(0, 1, 0), 0));
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setViewDirection(vec3(0, -1, 0));
    }
    else if (pos == 4) {
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setOrientation(quat(vec3(0, 1, 0), 0));
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setViewDirection(vec3(0, -1, 0));
        camera_->setViewDirection(vec3(1, 0, 0));
    }
    else if (pos == 5) {
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setOrientation(quat(vec3(0, 1, 0), 0));
        camera_->setViewDirection(vec3(0, 1, 0));
        camera_->setViewDirection(vec3(0, -1, 0));
        camera_->setViewDirection(vec3(1, 0, 0));
        camera_->setViewDirection(vec3(-1, 0, 0));
    }


    vec3 center;
    if (&frame)
        center = (&frame)->position();
    camera_->frame()->translate(center - camera_->frame()->orientation().rotate(old.coordinatesOf(center)) - camera_->frame()->translation());

}
