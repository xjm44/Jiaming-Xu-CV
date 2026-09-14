#include "../include/dialog_gcp_merge.h"
#include "../include/mainwindow.h"

dialog_gcp_merge::dialog_gcp_merge(mainwindow *parent)
	: QWidget(parent), checkFirst(false), checkSecond(false),
	firstTable(nullptr), secondTable(nullptr)
{
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setupUi(this);
	init(parent);

}

dialog_gcp_merge::~dialog_gcp_merge()
{}

void dialog_gcp_merge::init(mainwindow* parent)
{
	window_ = parent;

	connect(btnTxt1, SIGNAL(clicked()), this, SLOT(_txt1()), Qt::UniqueConnection);
	connect(btnTxt2, SIGNAL(clicked()), this, SLOT(_txt2()), Qt::UniqueConnection);
	connect(btnMerge, SIGNAL(clicked()), this, SLOT(_autoMerge()), Qt::UniqueConnection);

	firstTable = new QStandardItemModel(); // items in the table
	secondTable = new QStandardItemModel(); // items in the table
	firstTable->setHorizontalHeaderItem(0, new QStandardItem(tr("name")));
	firstTable->setHorizontalHeaderItem(1, new QStandardItem(tr("x")));
	firstTable->setHorizontalHeaderItem(2, new QStandardItem(tr("y")));
	firstTable->setHorizontalHeaderItem(3, new QStandardItem(tr("z")));
	secondTable->setHorizontalHeaderItem(0, new QStandardItem(tr("name")));
	secondTable->setHorizontalHeaderItem(1, new QStandardItem(tr("x")));
	secondTable->setHorizontalHeaderItem(2, new QStandardItem(tr("y")));
	secondTable->setHorizontalHeaderItem(3, new QStandardItem(tr("z")));

	tableView1->setEditTriggers(QTableView::NoEditTriggers);		// no edit	
	tableView2->setEditTriggers(QTableView::NoEditTriggers);		// no edit	
	tableView1->setAlternatingRowColors(true);                       
	tableView2->setAlternatingRowColors(true);                       
	tableView1->horizontalHeader()->setDefaultSectionSize(75);
	tableView2->horizontalHeader()->setDefaultSectionSize(75);
	tableView1->setFocusPolicy(Qt::NoFocus);                         
	tableView2->setFocusPolicy(Qt::NoFocus);                         

	tableView1->setModel(firstTable);
	tableView2->setModel(secondTable);
}

void dialog_gcp_merge::_txt1()
{
	// choose 1st cloud txt
	QString file = QFileDialog::getOpenFileName(
		this,
		"Txt File",
		QDir::currentPath(),
		"Supported formats (*.txt)"
	);
	if (!file.isEmpty()) {
		checkFirst = true;
		first_namexyz.clear();
		readTxt_and_show(file, 1);
	}
	else {
		checkFirst = false;
	}

	lineEdit1->setText(file);
}

void dialog_gcp_merge::_txt2()
{
	// choose 2nd cloud txt
	QString file = QFileDialog::getOpenFileName(
		this,
		"Txt File",
		QDir::currentPath(),
		"Supported formats (*.txt)"
	);
	if (!file.isEmpty()) {
		checkSecond = true;
		second_namexyz.clear();
		readTxt_and_show(file, 2);
	}
	else {
		checkSecond = false;
	}

	lineEdit2->setText(file);
}

void dialog_gcp_merge::_autoMerge()
{
	// make a saved path
	if (checkFirst && checkSecond) {

		// combine two vectorYuan and make first vector's real = second vector's xyz
		std::vector<Yuan> res = first_namexyz;	
		for (int i = 0; i < res.size(); i++) {
			res[i].rx = second_namexyz[i].x;
			res[i].ry = second_namexyz[i].y;
			res[i].rz = second_namexyz[i].z;
		}

		emit(_toMerge(res));
	}
	else {
		window_->log->set_log(log_type::Warning, QString(tr("Two txt files must be present.")).toStdString(), window_->getLogWidget());
	}
}

void dialog_gcp_merge::readTxt_and_show(QString filename, int a)
{
	// read txt
	QString confirmedTxt = filename;
	QFile file(confirmedTxt);
	if (!file.open(QIODevice::ReadOnly)) {
		window_->log->set_log(log_type::Info, QString(tr("Current file cannot be read.")).toStdString(), window_->getLogWidget());
		return;
	}

	int prevent_first_line = 0;

	if (a == 1) {  //tableview1
		// clear first
		firstTable->removeRows(0, firstTable->rowCount());

		while (!file.atEnd()) { // store all coord.

			QByteArray line = file.readLine();
			prevent_first_line++;

			if (prevent_first_line != 1) { // begin with 2nd line

				// save: name x y z only
				float x, y, z;
				x = line.split(' ')[1].toFloat();
				y = line.split(' ')[2].toFloat();
				z = line.split(' ')[3].toFloat();

				// create new Yuan
				Yuan temp_yuan;
				temp_yuan.name = line.split(' ')[0].toInt();
				temp_yuan.x = x;
				temp_yuan.y = y;
				temp_yuan.z = z;

				first_namexyz.emplace_back(temp_yuan);

			}
		}

		// show on table view
		for (int i = 0; i < first_namexyz.size(); i++)
		{
			// name
			firstTable->setItem(i, 0, new QStandardItem(QString("gcp_") + QString::number(first_namexyz.at(i).name)));
			// xyz
			firstTable->setItem(i, 1, new QStandardItem(QString::number(first_namexyz.at(i).x)));
			firstTable->setItem(i, 2, new QStandardItem(QString::number(first_namexyz.at(i).y)));
			firstTable->setItem(i, 3, new QStandardItem(QString::number(first_namexyz.at(i).z)));
		}

		tableView1->selectRow(0);

	}
	else if (a == 2) {  //tableview2
		// clear first
		secondTable->removeRows(0, secondTable->rowCount());

		while (!file.atEnd()) { // store all coord.

			QByteArray line = file.readLine();
			prevent_first_line++;

			if (prevent_first_line != 1) { // begin with 2nd line

				// save: name x y z only
				float x, y, z;
				x = line.split(' ')[1].toFloat();
				y = line.split(' ')[2].toFloat();
				z = line.split(' ')[3].toFloat();

				// create new Yuan
				Yuan temp_yuan;
				temp_yuan.name = line.split(' ')[0].toInt();
				temp_yuan.x = x;
				temp_yuan.y = y;
				temp_yuan.z = z;

				second_namexyz.emplace_back(temp_yuan);

			}
		}

		// show on table view
		for (int i = 0; i < second_namexyz.size(); i++)
		{
			// name
			secondTable->setItem(i, 0, new QStandardItem(QString("gcp_") + QString::number(second_namexyz.at(i).name)));
			// xyz
			secondTable->setItem(i, 1, new QStandardItem(QString::number(second_namexyz.at(i).x)));
			secondTable->setItem(i, 2, new QStandardItem(QString::number(second_namexyz.at(i).y)));
			secondTable->setItem(i, 3, new QStandardItem(QString::number(second_namexyz.at(i).z)));
		}

		tableView2->selectRow(0);

	}

	update();
	window_->log->set_log(log_type::Info, QString(tr("Result loaded.")).toStdString(), window_->getLogWidget());
}

