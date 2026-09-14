#include "../include/widget_gcp_list.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QHeaderView>
#include <iostream>
#include <QMessageBox>


gcpTableView::gcpTableView(QWidget* parent)
    : QTableView(parent)
{
    setAcceptDrops(true);

    mModel = NULL;
    mRowHeight = 30;
    mValidPress = false;
    mRowFrom = 0;
    mRowTo = 0;

}

void gcpTableView::SetModel(QStandardItemModel* model)
{
    mModel = model;
    QTableView::setModel(model);
}


void gcpTableView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        QModelIndex index = indexAt(e->pos());
        if (index.isValid())
        {
            mValidPress = true;
            mDragPoint = e->pos();
            mDragText = mModel->item(index.row(), 0)->text();
            mDragPointAtItem = mDragPoint - QPoint(0, index.row() * mRowHeight);
            mRowFrom = index.row();
        }
        else {
            clearSelection();
            mValidPress = false;
        }
    }
    QTableView::mousePressEvent(e);
}

void gcpTableView::mouseMoveEvent(QMouseEvent* e)
{

    if (!mValidPress)
    {
        return;
    }
    if (!(e->buttons() & Qt::LeftButton))
        return;

    if ((e->pos() - mDragPoint).manhattanLength() < QApplication::startDragDistance())
        return;


    DoDrag();        

    mValidPress = false;
}

void  gcpTableView::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasText())
    {
        e->acceptProposedAction();
    }
    else
    {
        e->ignore();
        QTableView::dragEnterEvent(e);
    }
}

void  gcpTableView::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasText())
    {
        int nCurRow = 0;
        QModelIndex index = indexAt(e->pos());
        if (index.isValid())
        {
            if (e->pos().y() - index.row() * mRowHeight >= mRowHeight / 2)
            {
                nCurRow = index.row() + 1;
            }
            else
            {
                nCurRow = index.row();
            }
        }
        else
        {
            nCurRow = mModel->rowCount();
        }
        //        if (nCurRow != mRowFrom)
        {
            mRowTo = nCurRow;
        }
        e->acceptProposedAction();
        return;
    }

    e->ignore();
    QTableView::dragMoveEvent(e);
}

void  gcpTableView::dropEvent(QDropEvent* e)
{
    if (e->mimeData()->hasText())
    {
        if (mRowTo != mRowFrom)
        {
            MoveRow(mRowFrom, mRowTo);
        }
        e->acceptProposedAction();
        return;
    }

    e->ignore();
    QTableView::dropEvent(e);
}

void gcpTableView::DoDrag()
{
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(mDragText);
    drag->setMimeData(mimeData);


    QPixmap drag_img(width(), mRowHeight);
    drag_img.fill(QColor(255, 255, 255, 100));
    QPainter painter(&drag_img);
    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(QRectF(40, 0, width(), mRowHeight), mDragText, QTextOption(Qt::AlignVCenter));

    drag->setPixmap(drag_img);
    drag->setHotSpot(mDragPointAtItem);


    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
    {

    }
}


void gcpTableView::MoveRow(int from, int to)
{
    if (from == to)
    {
        return;
    }
    QStandardItem* prev_item = mModel->item(from, 0);
    if (prev_item)
    {


        QList<QStandardItem*> prev_items;
        QStandardItem* item0 = new QStandardItem(prev_item->text()); // first col no need rewrite name
        prev_items.append(item0);

        // second col onwards, replace numbers
        for (int i = 0; i < 7; i++) {

            if (!mModel->item(from, i + 1)) {
                break;
            }
            QStandardItem* temp = new QStandardItem(mModel->item(from, i + 1)->text());
            prev_items.append(temp);
        }
        //for (QStandardItem* item : prev_items) {
        //    item->setTextAlignment(Qt::AlignCenter);
        //}

        // insert first, then delete duplicated row
        mModel->insertRow(to, prev_items);
        if (from < to)
        {
            mModel->removeRow(from);
            selectRow(to - 1);
        }
        else
        {
            mModel->removeRow(from + 1);
            selectRow(to);
        }

        emit(_dragged());
    }
}

