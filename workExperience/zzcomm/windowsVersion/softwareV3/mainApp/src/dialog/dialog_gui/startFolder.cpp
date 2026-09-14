#include "../../include/include_gui/startFolder.h"
#include "../../include/include_gui/mainApp.h"

QStringList arrangeMP4Names(QStringList files) //const QString& s1,const QString& s2
{
    //apply section(),mid() or whatever to take out the integer part and compare
    for (int i = 0; i < files.length(); i++) {
        for (int j = 0; j < files.length() - i - 1; j++) {
            QString cur = files.at(j);
            QString tbc = files.at(j + 1); // to be compared
            //GX010013.MP4
            QString temp1 = cur.section("GX", 1);//temp1=="XXX.mp4";
            temp1 = temp1.section('.', 0, 0);//temp1=="XXX"

            QString temp2 = tbc.section("GX", 1);
            temp2 = temp2.section('.', 0, 0);

            if (temp1.toInt() > temp2.toInt()) {
                QString to_be_del = tbc;
                files.removeAt(j + 1);
                files.insert(j, to_be_del);
            }
            else {
            }
        }
    }

    QStringList f = files;
    return f;
}
startFolder::startFolder(mainApp*parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

	
	setupUi(this);
	this->parent = dynamic_cast<mainApp*>(parent);
	this->setWindowTitle("Choose Project");
	setMinimumHeight(parent->height() / 4.3);
	setMinimumWidth(parent->width() * 4 / 7);

	init();
}

startFolder::~startFolder()
{}
void startFolder::closeEvent(QCloseEvent* event) {
	emit(_exit());
	std::cout << "folder closed " << std::endl;
}


void startFolder::init() {



	connect(btnSlam, SIGNAL(clicked()), this, SLOT(_slam_triggered()), Qt::UniqueConnection);
	connect(btnColor, SIGNAL(clicked()), this, SLOT(_color_triggered()), Qt::UniqueConnection);
	connect(btnRtk, SIGNAL(clicked()), this, SLOT(_rtk_triggered()), Qt::UniqueConnection);
	connect(btnGcp, SIGNAL(clicked()), this, SLOT(_gcp_triggered()), Qt::UniqueConnection);
	


}

void startFolder::_slam_triggered() {
	emit(_project("slam"));
}
void startFolder::_color_triggered() {
	emit(_project("color"));
}
void startFolder::_rtk_triggered() {
	emit(_project("rtk"));
}
void startFolder::_gcp_triggered() {
	emit(_project("gcp"));
}

QString startFolder::findFile(QString reg, QFileInfo file) {
    //QRegularExpression regex("map.yaml$");
    //QRegularExpressionMatchIterator i = regex.globalMatch(a.fileName());
    //while (i.hasNext()) {
    //    QRegularExpressionMatch match = i.next();
    //    if (match.hasMatch()) {
    //        //lineEditTraj->setText(pathLocation + s);
    //        checkNo++;
    //        std::cout << "Matched string: " << match.captured(0).toStdString() << std::endl;
    //        yaml = a.filePath();
    //    }
    //}
    QString res;
    QRegularExpression regex(reg);
    QRegularExpressionMatchIterator i = regex.globalMatch(file.fileName());
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()) {
            std::cout << "Matched string: " << match.captured(0).toStdString() << std::endl;
            res = file.filePath();
        }
    }

    if (res.isEmpty()) {
        return "nil";
    }
    return res;
}
QString startFolder::findFileInFolder(QString filter, QString path, QString reg) {

    //QStringList filter("*.bag");
    //QDir dir(pathLocation);
    //QStringList bags = dir.entryList(filter);
    //for (QString s : bags) { // filter out slam.bag
    //    QRegularExpression regex("_SLAM");
    //    QRegularExpressionMatchIterator i = regex.globalMatch(s);
    //    if (!i.hasNext()) {
    //        bag = (pathLocation + s);
    //        checkNo++;
    //    }
    //}
    QString res;
    QStringList filter_(filter);
    QDir dir(path);
    QStringList bags = dir.entryList(filter_);
    for (QString s : bags) { // filter out slam.bag
        QRegularExpression regex(reg);
        QRegularExpressionMatchIterator i = regex.globalMatch(s);
        if (i.hasNext()) {
            res = (path + s);
        }
    }
   
    if (res.isEmpty()) {
        return "nil";
    }
    return res;

}
QStringList startFolder::findFilesInFolder(QString filter, QString path, bool mp4) {
    QStringList res;
    //QStringList filters3("*.MP4"); // need rearrange order
    //QDir dirs3(pathLocation);
    //QStringList mp4 = dirs3.entryList(filters3);
    //for (const auto& name : mp4) {
    //    videoFilesMP4.push_back(name);
    //}
    //videoFilesMP4 = arrangeMP4Names(videoFilesMP4);

    QStringList filters(filter); // need rearrange order
    QDir dirs(path);
    QStringList mp4_ = dirs.entryList(filters);
    for (const auto& name : mp4_) {
        res.push_back(path + name);
    }
    if(mp4)
        res = arrangeMP4Names(res);

    return res;
}

bool startFolder::checkDir(QString str) {

    if (str == "slam") {
        std::cout << "slam " << std::endl;
        QString bag;
        QString yaml;
        for (QFileInfo a : dir.entryInfoList()) {
            if (a.isFile()) {
                //std::cout << "file " << a.fileName().toStdString() << std::endl;
                // check
                // bag              
                if(bag =="nil" || bag.isEmpty())
                    bag = findFile(".bag$", a);
            }
            else if (a.isDir()) {
                std::cout << "dir " << a.fileName().toStdString() << std::endl;
                // find yaml
                std::cout << "dir path " << a.filePath().toStdString() << std::endl;
                QDir subDir(a.filePath());
                for (QFileInfo aa : subDir.entryInfoList()) {
                    if (aa.isFile()) {
                        if (yaml == "nil" || yaml.isEmpty())
                            yaml = findFile("map.yaml$", aa);
                    }
                    //std::cout << "test " << aa.fileName().toStdString() << std::endl;
                }
            }
            else {
                std::cout << "else " << a.fileName().toStdString() << std::endl;
            }
        }

        if (yaml != "nil" && bag != "nil") {
            std::cout << "bag: " << bag.toStdString() << std::endl;
            std::cout << "yaml: " << yaml.toStdString() << std::endl;
            slamBag = bag;
            slamYaml = yaml;
            projPath = dir.path();
            return true;
        }
        else {
            return false;
        }

    }
    else if (str == "color") {
        std::cout << "color " << std::endl;
        QStringList videoFilesMP4;
        QStringList videoFilesPano;
        QString bag;
        QString csv;
        QString caliGp;
        QString caliPa;
        QString mask;
        QString pc;

        QString pathLocation = dir.path() + "/";
        pathLocation = pathLocation.replace('/', '\\');
        projPath = pathLocation;
        std::cout << "path: " << pathLocation.toStdString() << std::endl;

        //imu - COMPULSORY
        QStringList filter("*.bag");
        QDir dir(pathLocation);
        QStringList bags = dir.entryList(filter);
        for (QString s : bags) { // filter out slam.bag
            QRegularExpression regex("_SLAM|_optimized");
            QRegularExpressionMatchIterator i = regex.globalMatch(s);
            if (!i.hasNext()) {
                bag = (pathLocation + s);
            }
        }

        // traj - COMPULSORY
        csv = findFileInFolder("*.csv", pathLocation, "_begin|_optimized");

        // cali - COMPULSORY
        caliGp = findFileInFolder("*.yaml", pathLocation + "\\params\\", "Gp");
        caliPa = findFileInFolder("*.yaml", pathLocation + "\\params\\", "Pa");

        // pointcloud - COMPULSORY
        // for now, use _1.laz only
        pc = findFileInFolder("*.laz", pathLocation, "_1");


        // ================================== 1 out of 2 =====================================
        // video - mp4
        videoFilesMP4 = findFilesInFolder("*.MP4", pathLocation, true);

        // video - insv
        videoFilesPano = findFilesInFolder("*.insv", pathLocation, false);
        // ==================================================================================
        
        // mask file
        mask = findFileInFolder("*.png", pathLocation + "\\params\\", "mask");



        if (!bag.isEmpty() && csv != "nil" && pc != "nil") { //imu,traj,pointcloud - COMPULSORY
            std::cout << "bag: " << bag.toStdString() << std::endl;
            std::cout << "csv: " << csv.toStdString() << std::endl;       
            std::cout << "point cloud: " << pc.toStdString() << std::endl;


            // for gopro and pano
            if (videoFilesMP4.size() > 0 && videoFilesPano.size() == 0) {
                for (QString v : videoFilesMP4)std::cout << "mp4 video: " << v.toStdString() << std::endl;
                if (caliGp != "nil") {
                    std::cout << "gp yaml: " << caliGp.toStdString() << std::endl;
                }
                else {
                    // missing cali file
                    QMessageBox::warning(nullptr, tr("Warning"), tr("Missing calibration file (GoPro) in this folder."));
                    return false;
                }

            }else if (videoFilesPano.size() == 1 && videoFilesMP4.size() == 0) {
                //for (QString v : videoFilesPano)std::cout << "panoramic video: " << v.toStdString() << std::endl;
                if (mask != "nil") {
                    std::cout << "mask: " << mask.toStdString() << std::endl;
                }
                else {
                    // missing cali file
                    QMessageBox::warning(nullptr, tr("Warning"), tr("Missing mask file in this folder."));
                    return false;
                }
                if (caliPa != "nil") {
                    std::cout << "pa yaml: " << caliPa.toStdString() << std::endl;
                }
                else {
                    // missing cali file
                    QMessageBox::warning(nullptr, tr("Warning"), tr("Missing calibration file (Panoramic) in this folder."));
                    return false;
                }


            }else if (videoFilesPano.size() > 1 && videoFilesMP4.size() == 0) {
                // more than 1 insv file
                QMessageBox::warning(nullptr, tr("Warning"), tr("More than 1 .insv files in this folder."));
                return false;

            }


            slamBag = bag;
            colorCsv = csv;
            colorCaliGp = caliGp;
            colorCaliPa = caliPa;
            mp4Files = videoFilesMP4;
            panoFile = videoFilesPano;
            pointCloud = pc;
            colorMask = mask;
            if (mp4Files.size() > 0 && panoFile.size() > 0) {
                QMessageBox::warning(nullptr, tr("Warning"), tr("Invalid video files in this folder.\nPlease ensure n MP4 files or 1 insv file only."));
                return false;
            }
            else if (mp4Files.size() == 0 && panoFile.size() == 0) {
                QMessageBox::warning(nullptr, tr("Warning"), tr("No video file(s) in this folder.\nPlease ensure n MP4 files or 1 insv file only."));
                return false;
            }
            return true;
        }
        else {
            QMessageBox::warning(nullptr, tr("Warning"), tr("Invalid or missing files in this folder."));
            return false;
        }
    }
    else if (str == "rtk") {
        std::cout << "rtk " << std::endl;
        QString bag;
        QString yaml;
        QString txt;
        for (QFileInfo a : dir.entryInfoList()) {
            if (a.isFile()) {

                //if(yaml.isEmpty() || yaml == "nil")
                //    yaml = findFile("map.yaml$", a);
                if(bag.isEmpty() || bag == "nil")
                    bag = findFile(".bag$", a);
                //if(txt.isEmpty() || txt == "nil")
                //    txt = findFile(".txt$", a);

            }
            else if (a.isDir()) {
                std::cout << "dir " << a.fileName().toStdString() << std::endl;
                // find yaml
                std::cout << "dir path " << a.filePath().toStdString() << std::endl;
                QDir subDir(a.filePath());
                for (QFileInfo aa : subDir.entryInfoList()) {
                    if (aa.isFile()) {
                        if (yaml == "nil" || yaml.isEmpty())
                            yaml = findFile("map.yaml$", aa);
                    }
                }
            }
            else {
                std::cout << "else " << a.fileName().toStdString() << std::endl;
            }
        }
        if (yaml != "nil" && bag != "nil") { //  && txt != "nil"
            std::cout << "bag: " << bag.toStdString() << std::endl;
            std::cout << "yaml: " << yaml.toStdString() << std::endl;
            std::cout << "txt: " << txt.toStdString() << std::endl;
            slamBag = bag;
            slamYaml = yaml;
            projPath = dir.path();
            rtkTxt = txt;
            return true;
        }
        else {
            return false;
        }

    }
    else if (str == "gcp") {
        std::cout << "gcp " << std::endl;
        QString bag;
        QString yaml;
        QString txt;
        for (QFileInfo a : dir.entryInfoList()) {
            if (a.isFile()) {
                //if (yaml.isEmpty() || yaml == "nil")
                //    yaml = findFile("map.yaml$", a);
                if (bag.isEmpty() || bag == "nil")
                    bag = findFile(".bag$", a);
                //if (txt.isEmpty() || txt == "nil")
                //    txt = findFile(".txt$", a);

            }
            else if (a.isDir()) {
                std::cout << "dir " << a.fileName().toStdString() << std::endl;
                // find yaml
                std::cout << "dir path " << a.filePath().toStdString() << std::endl;
                QDir subDir(a.filePath());
                for (QFileInfo aa : subDir.entryInfoList()) {
                    if (aa.isFile()) {
                        if (yaml == "nil" || yaml.isEmpty())
                            yaml = findFile("map.yaml$", aa);
                    }
                }
            }
            else {
                std::cout << "else " << a.fileName().toStdString() << std::endl;
            }
        }
        if (yaml != "nil" && bag != "nil" ) { // && txt != "nil"
            std::cout << "bag: " << bag.toStdString() << std::endl;
            std::cout << "yaml: " << yaml.toStdString() << std::endl;
            std::cout << "txt: " << txt.toStdString() << std::endl;
            slamBag = bag;
            slamYaml = yaml;
            projPath = dir.path();
            gcpTxt = txt;
            return true;
        }
        else {
            return false;
        }

    }

    return false;

}

