#include "../include/report.h"



report::report(QWidget* parent)
    : QWidget(parent)

{
    ui.setupUi(this);

	this->setWindowTitle(QString("Generate Report"));
	this->setWindowIcon(QIcon("../resources/images/new_logo.png"));

	this->setFixedWidth(360);
	this->setFixedHeight(290);


    connect(ui.btnGet, SIGNAL(clicked()), this, SLOT(on_btnGet_clicked()), Qt::UniqueConnection);
    connect(ui.btnPicture, SIGNAL(clicked()), this, SLOT(on_btnPic_clicked()), Qt::UniqueConnection);
    connect(ui.btnLocation, SIGNAL(clicked()), this, SLOT(on_btnLoc_clicked()), Qt::UniqueConnection);

} 

void report::on_btnGet_clicked() {


    if (generateReport()) {
        
        QMessageBox::about(NULL, "Notification", "Report generated.");
    }
    else {
        QMessageBox::about(NULL, "Notification", "Report not generated.");
    }

}

void report::on_btnLoc_clicked() {

    if (ui.locationLine->text() != "")
    {
        QFileDialog fileDialog;
        QString strTargetFile = fileDialog.getExistingDirectory(this, tr("Choose a Folder"), location);
        if (!strTargetFile.isEmpty()) {
             ui.locationLine->setText(strTargetFile);
             location = strTargetFile;
        }
    }
    else {
        QMessageBox::information(NULL, "Error", "Invalid file path.");
    }

}

void report::on_btnPic_clicked() {

    std::cout << "pic \n";
    QStringList fileNameList = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("Choose a Picture"), location, tr("picture(*.png *.jpg)"));
    if (fileNameList.size()!=0) {
        ui.pictureLine->setText(fileNameList.join(","));
        pictureList = fileNameList;
    }
}
    
void report::paintEvent(QPaintEvent*) {

    ui.groupBox->setGeometry(0, 0, this->width(), this->height() * 0.8);

    ui.btnGet->setGeometry(this->width() * 0.5 - 60, this->height() * 0.85, 120, 25);


}


report::~report() {
	delete& ui;
}

bool report::generateReport() {

    PDFWriter pdfWriter;
    std::string loc; // pdf location
    loc = location.toStdString()+"/"+ ui.nameLine->text().toStdString() + ".pdf";
    pdfWriter.StartPDF(loc, ePDFVersion13); // file name
    PDFPage* page = new PDFPage();
    page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
    PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
    PDFUsedFont* font = pdfWriter.GetFontForFile("E:\\pdf\\PDF-Writer\\PDFWriterTesting\\Materials\\fonts\\arial.ttf");


    AbstractContentContext::TextOptions textOptions(font, 14, AbstractContentContext::eGray, 0);

    std::string cloudname;
    cloudname = "Cloud Name:  " + ui.nameLine->text().toStdString();
    std::string totalpoints;
    totalpoints = "Points:  " + points.toStdString();
    contentContext->WriteText(50, 790, cloudname, textOptions);
    contentContext->WriteText(50, 750, totalpoints, textOptions);


    pdfWriter.EndPageContentContext(contentContext);

    PageContentContext* cxt = pdfWriter.StartPageContentContext(page);

    AbstractContentContext::ImageOptions imageOptions;
    imageOptions.transformationMethod = AbstractContentContext::eMatrix;
    
    // check pic size ======================================================
    QImageReader reader(ui.pictureLine->text());
    QSize sizeOfImage = reader.size();
    int height = sizeOfImage.height();
    int width = sizeOfImage.width();
    //std::cout << QString::number(height).toStdString() + "  h\n";
    //std::cout << QString::number(width).toStdString() + "  w\n";
    float newHeight = height;
    float newWidth = width;
    if (height > 300.0 || width > 300.0) {
        newHeight = 300.0 / height;
        newWidth = 300.0 / width;

        if (newWidth > newHeight) {
            imageOptions.matrix[0] = imageOptions.matrix[3] = newWidth;
        }
        else if (newWidth < newHeight) {
            imageOptions.matrix[0] = imageOptions.matrix[3] = newHeight;
        }
    }
    //std::cout << QString::number(newHeight,'f', 1).toStdString() + "  new-h\n";
    //std::cout << QString::number(newWidth,'f', 1).toStdString() + "  new-w\n";
    //=======================================================================
    cxt->DrawImage(50, 100, ui.pictureLine->text().toStdString(),imageOptions);

    

    pdfWriter.EndPageContentContext(cxt);

    
    
    if (pdfWriter.WritePageAndRelease(page) == eSuccess) {
        pdfWriter.EndPDF();
        return true;
    }

    return false;

}



void report::setName(QString name) {
    this->name = name;
}
void report::setPoints(QString no) {
    points = no;
}
void report::setLocation(QString loc) {
    location = loc;
}
void report::refresh() {
    ui.nameLine->setText(name);
    ui.locationLine->setText(location);
}
