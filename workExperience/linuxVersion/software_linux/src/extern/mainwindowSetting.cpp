#include "../include/include_gui/mainwindowSetting.h"
#include "../include/include_gui/mainApp.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>
//using namespace easy3d;
mainwindowSettingDialog::mainwindowSettingDialog(mainApp *parent)
	: QDialog(parent)
    , languageIndex(-1)
    ,rescaleIn(nullptr)
    ,rescaleIndex(0)
    ,rescaleBool(false)
    ,bagReindex_(nullptr)
    ,bagReIndex(0)
    ,bagReindex2_(nullptr)
    ,bagReindexFile(QString())
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setupUi(this);

	parent_ = dynamic_cast<mainApp*>(parent);
    init();
}

mainwindowSettingDialog::~mainwindowSettingDialog()
{}
void mainwindowSettingDialog::closeEvent(QCloseEvent* event) {
    if (rescaleIn != NULL) {

        disconnect(rescaleIn, 0, 0, 0);
        rescaleIn->kill();
        rescaleIn = NULL;
        std::cout << "rescale x " << std::endl;
    }
    if (bagReindex_ != NULL) {

        disconnect(bagReindex_, 0, 0, 0);
        bagReindex_->kill();
        bagReindex_ = NULL;
        std::cout << "bagReindex1 x " << std::endl;
    }
    if (bagReindex2_ != NULL) {

        disconnect(bagReindex2_, 0, 0, 0);
        bagReindex2_->kill();
        bagReindex2_ = NULL;
        std::cout << "bagReindex2 x " << std::endl;
    }
    emit(_closed());
}

void mainwindowSettingDialog::init() {
    languageIndex = comboBoxLang->currentIndex();

	connect(pushButtonBg, SIGNAL(clicked()), this, SLOT(btnBg_triggered()), Qt::UniqueConnection);
	connect(comboBoxLang, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxLang(int)), Qt::UniqueConnection);
    connect(pushButtonLangOk, SIGNAL(clicked()), this, SLOT(btnLangOk_triggered()), Qt::UniqueConnection);
    connect(pushButtonIntensity, SIGNAL(clicked()), this, SLOT(btnIntensity_triggered()), Qt::UniqueConnection);
    connect(pushButtonBagRe, SIGNAL(clicked()), this, SLOT(btnBagRe_triggered()), Qt::UniqueConnection);
} 


void mainwindowSettingDialog::btnBg_triggered() {
    
    // const vec4& c = parent_->viewer()->backGroundColor();
    // QColor orig(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255), static_cast<int>(c.a * 255));
    // orig.setHsv(0, 0, 255);
    // const QColor& color = QColorDialog::getColor(orig, parent_);
    // if (color.isValid()) {
    //     const vec4 newColor(
    //         static_cast<float>(color.redF()),
    //         static_cast<float>(color.greenF()),
    //         static_cast<float>(color.blueF()),
    //         static_cast<float>(color.alphaF())
    //     );
    //     parent_->viewer()->setBackgroundColor(newColor);
    //     parent_->viewer()->update();
    // }
}

void mainwindowSettingDialog::comboboxLang(int i) {
     
    switch (i) {
    case 0:
        languageIndex = 0;
        break;
    case 1:
        languageIndex = 1;
        break;
    default:
        break;
    }
}

void mainwindowSettingDialog::btnLangOk_triggered() {
    switch (languageIndex) {
    case 0:
        if (parent_->lang->current_language != Language::en_) {
            parent_->lang->changeLanguage(Language::en_);
            parent_->languageStorage(1);
            parent_->retranslateUi(parent_);
            std::cout << "eng " << std::endl;
        }
        break;
    case 1:
        if (parent_->lang->current_language != Language::zh_) {
            parent_->lang->changeLanguage(Language::zh_);
            parent_->languageStorage(2);
            parent_->retranslateUi(parent_);
            std::cout << "chi " << std::endl;
        }

        break;
    default:
        break;
    }

    retranslateUi(this);
    update();
}

void mainwindowSettingDialog::btnIntensity_triggered() {

    QString file = QFileDialog::getOpenFileName(
        this,
        "Point Cloud File",
        QDir::currentPath(),
        "Supported formats (*.las *.laz)"
    );
    if (file.isEmpty()) {
        return;
    }

    if (!parent_->checkFilePath(file)) {
        return;
    }
    lineEditIntensity->setText(file);

    labelIntensity->setText(tr("A point cloud is selected."));

    QMessageBox::StandardButton check;
    check = QMessageBox::question(0, QString(tr("Confirmation")), QString(tr("Ready to rescale the intensity?")));

    if (check == QMessageBox::Yes) {

        rescaleIntensity(file);
    }
    else {
        lineEditIntensity->clear();
        labelIntensity->setText("");
    }

}

void mainwindowSettingDialog::btnBagRe_triggered() {

    QString file = QFileDialog::getOpenFileName(
        this,
        "Active Bag File",
        QDir::currentPath(),
        "Supported formats (*.active)"
    );
    if (file.isEmpty()) {
        return;
    }

    if (!parent_->checkFilePath(file)) {
        return;
    }
    lineEditBagRe->setText(file);

    labelBagRe->setText(tr("A bag file is selected."));

    QMessageBox::StandardButton check;
    check = QMessageBox::question(0, QString(tr("Confirmation")), QString(tr("Ready to fix the bag?")));

    if (check == QMessageBox::Yes) {

        bagReindex(file);
    }
    else {
        lineEditBagRe->clear();
        labelBagRe->setText("");
    }

}

void mainwindowSettingDialog::rescaleIntensity(QString inputFile) {
    if (!rescaleIn) {
        rescaleIn = new QProcess;

        QString outputFile = parent_->getFilePath(inputFile);
        //outputFile = outputFile.replace('/', '\\');
        outputFile = outputFile + "/" + parent_->getFileName(inputFile) +QString("_rescaled.laz");
        std::cout << "output: " << outputFile.toStdString() << std::endl;

        QString command = QString("../Lib/company/las_rescale %1 %2 -p 1\n").arg(inputFile).arg(outputFile);
        rescaleIn->start(QString(command));
        labelIntensity->setText(tr("Loading.."));
        connect(rescaleIn, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotRescaleIntensity_(int, QProcess::ExitStatus)));
    }
}


void mainwindowSettingDialog::slotRescaleIntensity_(int index, QProcess::ExitStatus exitstatus)
{

    //if (exitstatus == QProcess::ExitStatus::NormalExit)std::cout << "byebye" << std::endl;
    //if (exitstatus == QProcess::ExitStatus::CrashExit)std::cout << "gg" << std::endl;

    if (index == 0) {
        labelIntensity->setText(tr("Done. Saved in the same path as the input file."));
        disconnect(rescaleIn, 0, 0, 0);
        rescaleIn->kill();
        rescaleIn = NULL;
        return;
    }
    else {
        labelIntensity->setText(tr("Failed. Make sure the point cloud has intensity."));
        disconnect(rescaleIn, 0, 0, 0);
        rescaleIn->kill();
        rescaleIn = NULL;
        return;
    }
}

void mainwindowSettingDialog::bagReindex(QString inputFile) {
    if (!bagReindex_) {
        
        bagReindexFile = inputFile;
        bagReindex_ = new QProcess;
        
        //bagReindex_->start("sh");

        //QString command = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' rosbag info %1").arg(inputFile);
        QString command = QString("source ../ros/noetic/setup.bash && rosbag info %1\n").arg(inputFile);

        //bagReindex_->write(QString(command + "\r\n").toUtf8());
        bagReindex_->start("bash", QStringList() << "-c" << command);
        std::cout << "info first" << std::endl;
        connect(bagReindex_, SIGNAL(readyReadStandardOutput()), this, SLOT(slotBagReindex_()));
        connect(bagReindex_, SIGNAL(readyReadStandardError()), this, SLOT(slotBagReindex_()));
    
    }
}


void mainwindowSettingDialog::slotBagReindex_()
{
    QString str = QString(bagReindex_->readAllStandardOutput());
    QString err = QString(bagReindex_->readAllStandardError());
    
    //std::cout << "---    " << str.toStdString() << std::endl;
    int check = err.indexOf("ERROR bag unindexed:");
    int exc = str.indexOf("sensor_msgs/PointCloud2");

    if (check != -1) {
        labelBagRe->setText(tr("Checked. This bag can be fixed.."));
        bagReindex2();
        disconnect(bagReindex_, 0, 0, 0);
        bagReindex_->kill();
        bagReindex_ = NULL;
        std::cout << "bag fixing.." << std::endl;
        return;
    }
        
    if (exc != -1) {
        labelBagRe->setText(tr("Failed. This bag cannot/no need to be fixed."));
        disconnect(bagReindex_, 0, 0, 0);
        bagReindex_->kill();
        bagReindex_ = NULL;
        bagReindexFile = "";
        std::cout << "This bag cannot/no need to be fixed." << std::endl;
        return;
    }
}
void mainwindowSettingDialog::bagReindex2() {
    if (!bagReindex2_) {
        bagReindex2_ = new QProcess;

        //bagReindex2_->start("sh");
        labelBagRe->setText(tr("Fixing the bag.."));

       //QString command = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' rosbag reindex %1").arg(bagReindexFile);
        QString command = QString("source ../ros/noetic/setup.bash && rosbag reindex %1\n").arg(bagReindexFile);
        
        QString filename = parent_->getFileName(bagReindexFile);
        QString filepath = parent_->getFilePath(bagReindexFile);
        //bagReindexFile
        QString check = QString(filepath + "/" + filename + "_fixed.bag");
        QFile fi(check);
        if (fi.exists()) {

            labelBagRe->setText(tr("Failed. A file with the same output name format has already exists."));
        }
        else {

            //bagReindex2_->write(command.toLocal8Bit() + '\n');
            bagReindex2_->start("bash", QStringList() << "-c" << command);
            connect(bagReindex2_, SIGNAL(readyReadStandardOutput()), this, SLOT(slotBagReindex2_()));
            connect(bagReindex2_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotReindexExitCode(int, QProcess::ExitStatus)));
        }


    }

}
void mainwindowSettingDialog::slotReindexExitCode(int a, QProcess::ExitStatus e) {
    std::cout << a << std::endl;
    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "reindex done" << std::endl;

        labelBagRe->setText(tr("Fixed. Saved in the same path as the input file."));

        QString filename = parent_->getFileName(bagReindexFile);
        QString filepath = parent_->getFilePath(bagReindexFile);
        //bagReindexFile
        bool check = QFile::rename(bagReindexFile, filepath + "/" + filename + "_fixed.bag");

        disconnect(bagReindex2_, 0, 0, 0);
        bagReindex2_->kill();
        bagReindex2_ = NULL;
        bagReindexFile = "";
        std::cout << "bag fixed." << std::endl;
        if (check)std::cout << "bag name changed." << std::endl;
        else std::cout << "bag name not changed." << std::endl;
        return;

    }
    else {
        std::cout << "reindex failed" << std::endl;
        //labelBagRe->setText(tr("Failed. A file with the same name has already exists."));
        labelBagRe->setText(tr("Failed to fix."));
        disconnect(bagReindex2_, 0, 0, 0);
        bagReindex2_->kill();
        bagReindex2_ = NULL;
        bagReindexFile = "";
        return;
    }
}
void mainwindowSettingDialog::slotBagReindex2_()
{
    // QString str = QString(bagReindex2_->readAllStandardOutput());
    // int check = str.indexOf("100%");
    // int err1 = str.indexOf("Skipping");
    // int err2 = str.indexOf("ERROR");
    // std::cout<< str.toStdString()<< std::endl;
    // if (check != -1) {
    //     labelBagRe->setText(tr("Fixed. Saved in the same path as the input file."));

    //     QString filename = parent_->getFileName(bagReindexFile);
    //     QString filepath = parent_->getFilePath(bagReindexFile);
    //     //bagReindexFile
    //     bool check = QFile::rename(bagReindexFile, filepath+"/"+filename+"_fixed.bag");

    //     disconnect(bagReindex2_, 0, 0, 0);
    //     bagReindex2_->kill();
    //     bagReindex2_ = NULL;
    //     bagReindexFile = "";
    //     std::cout << "bag fixed." << std::endl;
    //     if(check)std::cout << "bag name changed."  << std::endl;
    //     else std::cout << "bag name not changed." << std::endl;
    //     return;
    // }

    // if (err1 != -1 || err2 != -1) {
    //     labelBagRe->setText(tr("Failed. A file with the same name has already exists."));
    //     disconnect(bagReindex2_, 0, 0, 0);
    //     bagReindex2_->kill();
    //     bagReindex2_ = NULL;
    //     bagReindexFile = "";
    //     return;
    // }


}