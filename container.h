#ifndef CONTAINER_H
#define CONTAINER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QColor>
#include <QStringList>
#include <QVector>
#include <QLabel>
#include <QMoveEvent>

namespace Ui {
class Container;
}

class Container : public QWidget
{
    Q_OBJECT

public:
    explicit Container(QWidget *parent = nullptr,
                       const QString &name = "Container",
                       const QColor &color = Qt::blue,
                       const QStringList &inputs = {},
                       const QStringList &outputs = {});
    struct PortInfo {
        QString name;
        bool isOutput;
        QLabel *circle;
    };

    QVector<PortInfo> getPorts() const { return ports; }
    QString getName() const { return containerName; }
    QColor getColor() const { return containerColor; }
    void setSelected(bool selected);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
     void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
     void moveEvent(QMoveEvent *event) override;
 signals:
     void portClicked(Container *container, const QString &portName, bool isOutput, QPoint globalPos);
      void moved();
      void clicked(Container *container);


private:
    Ui::Container *ui;
    bool dragging;
    QPoint dragStartPosition;
    QColor containerColor;
    void positionPortCircles();
    QVector<PortInfo> ports;
    QString containerName;
    bool isSelected = false;

 ~Container();
};

#endif // CONTAINER_H
