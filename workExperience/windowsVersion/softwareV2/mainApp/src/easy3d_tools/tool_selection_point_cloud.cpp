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

#include "../include/easy3d_inc/tools/tool_selection_point_cloud.h"
#include "../include/easy3d_inc/tools/tool_manager.h"
#include "../include/easy3d_inc/viewer.h"

#include <easy3d/gui/picker_model.h>
#include <easy3d/gui/picker_point_cloud.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/buffer.h>
#include <easy3d/core/point_cloud.h>
#include <easy3d/util/logging.h>


namespace easy3d {

    namespace tools {

        bool pointClick = false;

        // -------------------- ToolPointCloudSelection ----------------------


        ToolPointCloudSelection::ToolPointCloudSelection(ToolManager* mgr)
            : Tool(mgr) {
        }


        void ToolPointCloudSelection::update_render_buffer(PointCloud* cloud) const {
            auto d = cloud->renderer()->get_points_drawable("vertices");
            
            if (pointClick) {
                if ((d->coloring_method() != easy3d::State::SCALAR_FIELD) || (d->property_location() != State::VERTEX) || (d->property_name() != "v:selectPoint")) {
                    if (!cloud->get_vertex_property<bool>("v:selectPoint"))
                        cloud->add_vertex_property<bool>("v:selectPoint", false);

                    d->set_coloring(State::SCALAR_FIELD, State::VERTEX, "v:selectPoint");

                    buffer::update(cloud, d);
                }
                else {
                    auto select = cloud->vertex_property<bool>("v:selectPoint", false);
                    // update the drawable's texcoord buffer
                    std::vector<vec2> texcoords(d->num_vertices());

                    for (auto v : cloud->vertices())
                        texcoords[v.idx()] = vec2(select[v], 0.5f);
                    d->update_texcoord_buffer(texcoords);

                    d->set_coloring(State::SCALAR_FIELD, State::VERTEX, "v:selectPoint");
                }
                
            }
            else {
                if ((d->coloring_method() != easy3d::State::SCALAR_FIELD) || (d->property_location() != State::VERTEX) || (d->property_name() != "v:select")) {
                    if (!cloud->get_vertex_property<bool>("v:select"))
                        cloud->add_vertex_property<bool>("v:select", false);

                    d->set_coloring(State::SCALAR_FIELD, State::VERTEX, "v:select");

                    buffer::update(cloud, d);
                }
                else {
                    auto select = cloud->vertex_property<bool>("v:select", false);
                    // update the drawable's texcoord buffer
                    std::vector<vec2> texcoords(d->num_vertices());

                    for (auto v : cloud->vertices())
                        texcoords[v.idx()] = vec2(select[v], 0.5f);
                    d->update_texcoord_buffer(texcoords);

                    d->set_coloring(State::SCALAR_FIELD, State::VERTEX, "v:select");
                }
                
            }
            
        }


        // -------------------- Click Select ----------------------

        ToolPointCloudSelectionClick::ToolPointCloudSelectionClick(ToolManager* mgr, PointCloudPicker* picker, SelectMode mode)
            : ToolPointCloudSelection(mgr)
            , picker_(picker)
            , select_mode_(mode)
        {
            model_picker_ = new ModelPicker(mgr->viewer()->camera());
            pointClick = true;
        }


        ToolPointCloudSelectionClick::~ToolPointCloudSelectionClick() {
            if (model_picker_)
                delete model_picker_;
        }


        void ToolPointCloudSelectionClick::press(int x, int y) {
            PointCloud::Vertex picked_vertex = PointCloud::Vertex();

            PointCloud* cloud = multiple_pick(picked_vertex, x, y);

            if (cloud && picked_vertex.is_valid()) {
                auto selected = cloud->vertex_property<bool>("v:selectPoint");
                
                // finer check to avoid unnecessary buffer update
                if (selected[picked_vertex] != (select_mode_ != SM_DESELECT)) {
                    selected[picked_vertex] = (select_mode_ != SM_DESELECT);
                    pointClick = true;
                    update_render_buffer(cloud);

                }
            }
        }


        PointCloud* ToolPointCloudSelectionClick::multiple_pick(PointCloud::Vertex& vertex, int x, int y) {
            vertex = PointCloud::Vertex();

            Model* model = model_picker_->pick(tool_manager()->viewer()->models(), x, y);
            PointCloud* picked_cloud = dynamic_cast<PointCloud*>(model);
            if (!picked_cloud)
                return nullptr;

            vertex = picker_->pick_vertex(picked_cloud, x, y);
            
            return picked_cloud;
        }


        MultitoolPointCloudSelectionClick::MultitoolPointCloudSelectionClick(ToolManager* mgr)
            : MultiTool(mgr) {
            picker_ = new PointCloudPicker(mgr->viewer()->camera());
            set_tool(LEFT_BUTTON, new ToolPointCloudSelectionClick(mgr, picker_, SM_SELECT));
            set_tool(RIGHT_BUTTON, new ToolPointCloudSelectionClick(mgr, picker_, SM_DESELECT));
        }


        MultitoolPointCloudSelectionClick::~MultitoolPointCloudSelectionClick() {
            delete picker_;
        }


        void MultitoolPointCloudSelectionClick::prepare_hint(ToolButton button, int x, int y) {
            clear_hint();
            if (button == NO_BUTTON && picker_) {
                PointCloud::Vertex vertex;
                auto cloud = dynamic_cast<ToolPointCloudSelectionClick*>(get_tool(LEFT_BUTTON))->multiple_pick(vertex, x, y);
                if (cloud && vertex.is_valid()) {
                    auto drawable = cloud->renderer()->get_points_drawable("vertices");
                    drawable->set_highlight(true);
                    drawable->set_highlight_range({ vertex.idx(), vertex.idx() });
                }
            }
        }

        void MultitoolPointCloudSelectionClick::clear_hint() {
            for (auto model : tool_manager()->viewer()->models()) {
                auto cloud = dynamic_cast<PointCloud*>(model);
                if (cloud) {
                    auto drawable = cloud->renderer()->get_points_drawable("vertices");
                    drawable->set_highlight(false);
                    drawable->set_highlight_range({ -1, -1 });
                }
            }
        }


        // ------------------ Lasso Select -----------------------

        ToolPointCloudSelectionLasso::ToolPointCloudSelectionLasso(ToolManager* mgr, PointCloudPicker* picker, SelectMode mode)
            : ToolPointCloudSelection(mgr), picker_(picker), select_mode_(mode) {
            pointClick = false;
        }

        void ToolPointCloudSelectionLasso::press(int x, int y) {
            lasso_.clear();
            lasso_.push_back(vec2(float(x), float(y)));

        }


        void ToolPointCloudSelectionLasso::drag(int x, int y) {
            lasso_.push_back(vec2(float(x), float(y)));
        }


        void ToolPointCloudSelectionLasso::release(int x, int y) {
            if (lasso_.size() < 3)
                return;

            for (auto model : tool_manager()->viewer()->models()) {
                auto cloud = dynamic_cast<PointCloud*>(model);
                if (cloud && cloud->renderer()->is_visible()) {
                    picker_->pick_vertices(cloud, lasso_, select_mode_ != SM_SELECT);

                    pointClick = false;
                    update_render_buffer(cloud);
                }
            }
            lasso_.clear();
        }


        MultitoolPointCloudSelectionLasso::MultitoolPointCloudSelectionLasso(ToolManager* mgr)
            : MultiTool(mgr) {
            picker_ = new PointCloudPicker(mgr->viewer()->camera());
            set_tool(LEFT_BUTTON, new ToolPointCloudSelectionLasso(mgr, picker_, SM_SELECT));
            set_tool(RIGHT_BUTTON, new ToolPointCloudSelectionLasso(mgr, picker_, SM_DESELECT));

            clear_hint();
        }


        MultitoolPointCloudSelectionLasso::~MultitoolPointCloudSelectionLasso() {
            delete picker_;
        }


        void MultitoolPointCloudSelectionLasso::press(ToolButton button, int x, int y) {
            MultiTool::press(button, x, y);
            lasso_.clear();
            lasso_.push_back(vec2(float(x), float(y)));
        }


        void MultitoolPointCloudSelectionLasso::prepare_hint(ToolButton button, int x, int y) {
            if (button != NO_BUTTON)
                lasso_.push_back(vec2(float(x), float(y)));
        }

        void MultitoolPointCloudSelectionLasso::clear_hint() {
            lasso_.clear();
        }

        void MultitoolPointCloudSelectionLasso::draw_hint() const {
            draw_lasso(lasso_);
        }

    }

}
