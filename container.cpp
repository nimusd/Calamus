#include "container.h"
#include "ui_container.h"

Container::Container(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Container)
    , dragging(false)
{
    ui->setupUi(this);
}
Container::~Container()
{
    delete ui;
}

void Container::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->pos();
    }
}

void Container::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        QPoint newPos = mapToParent(event->pos() - dragStartPosition);
        move(newPos);
    }
}

void Container::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}
