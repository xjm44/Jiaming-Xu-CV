#ifndef DIALOG_PHOTO_H
#define DIALOG_PHOTO_H

#include "dialog.h"
#include "ui_dialog_photo.h"

class DialogPhoto : public Dialog, public Ui::DialogPhoto
{
    Q_OBJECT

public:
    explicit DialogPhoto(mainwindow* window);
    ~DialogPhoto() override = default;

	

protected:

	void closeEvent(QCloseEvent* event) override;

	QString imagePath;
	QString csvPath;
	int a; // check both lineedit are filled

private Q_SLOTS:
	void _imagePath_clicked();
	void _csvPath_clicked();
	void _ok_clicked();

Q_SIGNALS:

	void _proceed(QString imagePath, QString csvPath, int a); // press ok
	void _cancel(); // close btn -> photo button change 


};

#endif // DIALOG_PHOTO_H
