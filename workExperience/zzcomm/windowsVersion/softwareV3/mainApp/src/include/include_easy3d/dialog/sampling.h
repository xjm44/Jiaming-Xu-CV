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

#pragma once
#include <easy3d/core/point_cloud.h>
#include <QDialog>
#include "ui_sampling.h"
class mainApp;
class PaintCanvas;
namespace easy3d {
	class KdTreeSearch;
}
class sampling : public QDialog, Ui::samplingClass
{
	Q_OBJECT

public:
	sampling(mainApp*parent);
	~sampling();
	void init();

protected:
	void closeEvent(QCloseEvent* event);
	void showEvent(QShowEvent* e) override;

	void constructKdTree();

Q_SIGNALS:
	void _closed();      

private Q_SLOTS:

	void computeAvgSpacing();
	void apply();
	void query();
	void sor();

private:
	mainApp* window_;
	PaintCanvas* viewer_;
	easy3d::KdTreeSearch* kdtree_;
	std::vector<easy3d::PointCloud::Vertex> points_to_remove_;
};
