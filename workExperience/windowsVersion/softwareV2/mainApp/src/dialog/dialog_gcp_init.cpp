#include "../include/dialog_gcp_init.h"
#include "../include/mainwindow.h"

dialog_gcp_init::dialog_gcp_init(mainwindow* parent)
	: QWidget(parent), savingPath(QString()), pointCloud(QString()), routeCloud(QString()),
	checkPc(false), checkSp(false), checkNoG(false)
{
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setupUi(this);
	setFixedHeight(222);
	setFixedWidth(330);

	window_ = parent;

	_NoofGcp(); // integer only for input

	connect(btnPc, SIGNAL(clicked()), this, SLOT(_pointCloud()));
	connect(btnSp, SIGNAL(clicked()), this, SLOT(_savingPath()));
	connect(btnRp, SIGNAL(clicked()), this, SLOT(_routeCloud()));
	connect(btnDone, SIGNAL(clicked()), this, SLOT(_done()));
}

dialog_gcp_init::~dialog_gcp_init()
{
}

void dialog_gcp_init::_pointCloud()
{
	QString file = QFileDialog::getOpenFileName(
		this,
		"Ply File",
		QDir::currentPath(),
		"Supported formats (*.ply)"
	);
	if (!file.isEmpty()) {
		checkPc = true;
	}
	else {
		checkPc = false;
	}

	lineEditPc->setText(file);
	pointCloud = file;
}
void dialog_gcp_init::_savingPath()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Saving Path"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		checkSp = true;
	}
	else {
		checkSp = false;
	}
	lineEditSp->setText(dir);
	savingPath = dir;
}

void dialog_gcp_init::_NoofGcp()
{
	QRegExp regx("[0-9]+$");
	QValidator* validator = new QRegExpValidator(regx, lineEditGcp);
	lineEditGcp->setValidator(validator);
}

void dialog_gcp_init::_routeCloud()
{
	QString file = QFileDialog::getOpenFileName(
		this,
		"Ply File",
		QDir::currentPath(),
		"Supported formats (*.ply)"
	);
	/*if (!file.isEmpty()) {
		check++;
	}*/

	lineEditRc->setText(file);
	routeCloud = file;
}

void dialog_gcp_init::_done()
{
	if (checkPc && checkSp) {

		window_->current_pointcloud_path = lineEditPc->text();
		window_->current_routecloud_path = lineEditRc->text();
		window_->current_saving_path = lineEditSp->text();

		emit(_proceed());
		
	}
	else {
		window_->log->set_log(log_type::Warning, QString(tr("Please make sure all *options have inputted.")).toStdString(), window_->getLogWidget());
		return;
	}
}
