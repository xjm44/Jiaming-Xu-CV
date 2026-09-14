#include "../../include/process/rtk.h"
#include "../../include/process/processGui.h"

rtk::rtk(gui *parent)
	: QWidget(parent), countBag(false), countTxt(false)
{
	//const Qt::WindowFlags flags = windowFlags();
	//setWindowFlags(flags | Qt::Tool);
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	this->setFixedHeight(140);
	this->setFixedWidth(450);
	setAcceptDrops(true); // drag file
	ui.setupUi(this);

	gui_ = parent;

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(_done_clicked()), Qt::UniqueConnection);
}

rtk::~rtk()
{
}

void rtk::closeEvent(QCloseEvent* event) {
	countBag = false; countTxt = false;
	bagPath = QString();
	txtPath = QString();
	ui.bagInput->setText(tr(".bag file"));
	ui.txtInput->setText(tr(".txt file"));
	ui.checkBox->setCheckState(Qt::Unchecked);
	std::cout << "rtk closed\n";
	emit(rtk_close());
}

void rtk::paintEvent(QPaintEvent*) {
	ui.bagInput->setGeometry(ui.dataInput1->x()+5, ui.dataInput1->y() + 5, ui.dataInput1->width()*0.95, ui.dataInput1->height() * 0.7);
	ui.txtInput->setGeometry(ui.dataInput2->x() + 5, ui.dataInput2->y() + 5, ui.dataInput2->width() * 0.95, ui.dataInput2->height() * 0.7);

}

void rtk::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls()) {

		if((e->pos().y()) > ui.dataInput1->y() && (e->pos().y()) < ui.dataInput1->y() + ui.dataInput1->height())
			e->acceptProposedAction();
	}
}

void rtk::dropEvent(QDropEvent* e)
{
	if (e->pos().x() < this->width() / 2) {
		QString filename = e->mimeData()->urls().first().toString();
		if (filename.toStdString().substr(filename.toStdString().find_last_of(".") + 1) == "bag") {
			std::string draggedfilename = filename.toStdString().substr(filename.toStdString().find_last_of("/") + 1);
			QString qdraggedfilename = QString::fromStdString(draggedfilename);
			ui.bagInput->setText(QString(tr("\"%1\" received!")).arg(qdraggedfilename));
			countBag = true;
			bagPath = filename;
		}
		else {
			ui.bagInput->setText(QString(tr(".bag file only!")));
			countBag = false;
		}

	}
	else {
		QString filename = e->mimeData()->urls().first().toString();
		if (filename.toStdString().substr(filename.toStdString().find_last_of(".") + 1) == "txt") {
			std::string draggedfilename = filename.toStdString().substr(filename.toStdString().find_last_of("/") + 1);
			QString qdraggedfilename = QString::fromStdString(draggedfilename);
			ui.txtInput->setText(QString(tr("\"%1\" received!")).arg(qdraggedfilename));
			countTxt = true;
			txtPath = filename;
		}
		else {
			ui.txtInput->setText(QString(tr(".txt file only!")));
			countTxt = false;
		}
	}

}
void rtk::_done_clicked() {

	if (countBag && countTxt) {
		int lidarbagIndex = 8; // remove file:/.......
		//bagPath = bagPath.mid(lidarbagIndex).simplified();
		txtPath = txtPath.mid(lidarbagIndex).simplified();


		if (ui.checkBox->checkState() == Qt::Checked) 
			gui_->setGcpChecked(true);
		
		else 
			gui_->setGcpChecked(false);
		

		emit(rtk_ready(bagPath,txtPath));
		this->close();
	}
	else {
		QMessageBox::information(NULL, tr("Info"), tr("Make sure both files are dragged in."));
	}

}
