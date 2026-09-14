#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QLabel>
#include <QMouseEvent>
#include <QStandardItemModel>



class gcpTableView : public QTableView
{
    Q_OBJECT
public:
    gcpTableView(QWidget* parent = Q_NULLPTR);
    ~gcpTableView() override = default;

    void SetModel(QStandardItemModel* model); // contents

    QStandardItemModel* getModel() { return mModel; } // to get contents in row/col

Q_SIGNALS:
    void _dragged(); // after dragged manually


protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);


private:
    void DoDrag();                      // to drag
    void MoveRow(int from, int to);     // when moving row

private:
    QStandardItemModel* mModel; // all contents
    int mRowHeight;             // when dragged, the translucent item

    bool mValidPress;           // when dragged, check if within the table 
    int mRowFrom;				// starting row
    int mRowTo;                 // ending row

    QString mDragText;          // when dragged, text to be shown
    QPoint mDragPoint;          // dragged starting point
    QPoint mDragPointAtItem;    // dragged to the point

};

#endif // MYTABLEVIEW_H