
#include "../include/easy3d_inc//dialog/dialog_photo.h"
#include "../include/easy3d_inc/viewer.h"
#include "../include/mainwindow.h"
#include <QFileDialog>

#include <easy3d/core/model.h>
#include <easy3d/util/file_system.h>


using namespace easy3d;

DialogPhoto::DialogPhoto(mainwindow* window)
    : Dialog(window),a(0) {
    setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

	lineEdit->setText(tr("Choose a image path."));
	lineEdit_2->setText(tr("Choose a .csv file."));


	connect(btnPath, SIGNAL(clicked()), this, SLOT(_imagePath_clicked()), Qt::UniqueConnection);
	connect(btnCsv, SIGNAL(clicked()), this, SLOT(_csvPath_clicked()), Qt::UniqueConnection);
	connect(pushButtonOK, SIGNAL(clicked()), this, SLOT(_ok_clicked()), Qt::UniqueConnection);

}

void DialogPhoto::closeEvent(QCloseEvent* event) {

	std::cout << "photo closed\n";
	emit(_cancel());

}

void DialogPhoto::_imagePath_clicked() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Image Path"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		a++;
	}
	imagePath = dir;
	lineEdit->setText(dir);
}

void DialogPhoto::_csvPath_clicked() {
	QString file = QFileDialog::getOpenFileName(
		this,
		"Csv File",
		QDir::currentPath(),
		"Supported formats (*.csv)"
	);
	if (!file.isEmpty()) {
		a++;
	}
	csvPath = file;
	lineEdit_2->setText(file);
}
void DialogPhoto::_ok_clicked() {
	emit(_proceed(imagePath, csvPath,a));

}

