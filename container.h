#ifndef CONTAINER_H
#define CONTAINER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>

namespace Ui {
class Container;
}

class Container : public QWidget
{
    Q_OBJECT

public:
    explicit Container(QWidget *parent = nullptr);
    ~Container();


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::Container *ui;
    bool dragging;
    QPoint dragStartPosition;
};

#endif // CONTAINER_H
