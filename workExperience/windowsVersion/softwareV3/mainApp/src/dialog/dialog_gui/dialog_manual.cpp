#include "../../include/include_gui/mainApp.h"
#include "../../include/include_gui/dialog_manual.h"
#include <QtCore>
#include <QUrl>
#include <QDesktopServices>

dialog_manual::dialog_manual(mainApp* parent) {

	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setupUi(this);
	this->parent = parent;
	setAttribute(Qt::WA_QuitOnClose,false);
}
dialog_manual::~dialog_manual() {

}

void dialog_manual::init() {
	connect(btnFeedBack, SIGNAL(clicked()), this, SLOT(_btnFeedback_clicked()), Qt::UniqueConnection);
	// resources
	setLabel(*labelDownloadSw, QString("www.baidu.com"));
	setLabel(*labelSwFaq, QString("www.baidu.com"));
	// process tips
	setLabel(*processSlam, QString("www.baidu.com"));
	setLabel(*processColor, QString("www.baidu.com"));
	setLabel(*processRtk, QString("www.baidu.com"));
	setLabel(*processGcp, QString("www.baidu.com"));
	// learn
	setLabel(*learnEnv, QString("www.baidu.com"));
	setLabel(*learnParam, QString("www.baidu.com"));
	setLabel(*learnInstall, QString("www.baidu.com"));
	// quick ref
	setLabel(*quickScan, QString("www.baidu.com"));



    //labelDownloadSw->installEventFilter(this);
}

void dialog_manual::setLabel(QLabel& l, QString link) {

	l.setText(QString("<a style= text-decoration:none; style=color:#d9d7d7; href=\"%1\">%2</a>").arg(link).arg(l.text()));
	l.setTextFormat(Qt::RichText);
	l.setTextInteractionFlags(Qt::TextBrowserInteraction);
	l.setOpenExternalLinks(true);
	l.setFocusPolicy(Qt::NoFocus);

}

void dialog_manual::_btnFeedback_clicked() {
	std::cout << "feedback btn" << std::endl;
	QDesktopServices::openUrl(QUrl("http://www.baidu.com", QUrl::TolerantMode));


}

bool dialog_manual::eventFilter(QObject* watched, QEvent* event) {

  //  if (event->type() == QEvent::CursorChange ) { // watched == qobject_cast<QLabel*>(watched) && 
  //      //auto&& cursor = qobject_cast<QLabel*>(watched)->cursor();
  //      if (watched) {
  //          //watched->metaObject()->className();
  //          QLabel* label = qobject_cast<QLabel*>(watched);
  //          auto&& cursor = label->cursor();
  //          if (cursor.shape() == Qt::PointingHandCursor) {
  //              label->setStyleSheet("QLabel#labelDownloadSw{color:red}");
  //              update();
  //          }
  //          else {
  //              //QTimer::singleShot(10, [this]() {
  //              //    label->setText(u8"����һ�����Ե�����\
  //              //    <style> a {text-decoration: none;} </style>\
  //              //    <a href = www.baidu.com>�ٶ�</a>\
  //              //    ����������Hover");
  //              //    });
  //              label->setStyleSheet("QLabel{color:green}");
  //          }
  //          //
  //          //if (cursor.shape() == Qt::PointingHandCursor) {
  //          //std::cout << "hi " << label->text().toStdString() << std::endl;
  //          //std::cout << "hi " << watched->metaObject()->className() << std::endl;
  //          //}
  //          //else {
  //          //    std::cout << "bye" << std::endl;
  //          //}


  //          //if (watched->metaObject()->className() == "QLabel")
  //          //{
  //              //QLabel* label = qobject_cast<QLabel*>(watched);
  //              //std::cout << "hi "<< label->text().toStdString() << std::endl;
  //              /*label->setText(QString("<a style= text-decoration:none; style=color:#d9d7d7; href=\"%1\">%2</a>").arg("link").arg("l.text()aa"));*/
  ///*          }
  //          else
  //              std::cout << "bye " << std::endl;*/
  //      }

  //  }

    return QDialog::eventFilter(watched, event);




}