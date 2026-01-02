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
#include <QMap>
#include "envelopelibraryDialog.h"

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
    QString getInstanceName() const { return instanceName; }
    void setInstanceName(const QString &name) { instanceName = name; }
    QColor getColor() const { return containerColor; }
    void setSelected(bool selected);

    // Get input and output port lists separately
    QStringList getInputPorts() const;
    QStringList getOutputPorts() const;

    // Parameter management
    void setParameter(const QString &name, double value);
    double getParameter(const QString &name, double defaultValue = 0.0) const;
    QMap<QString, double> getParameters() const { return parameters; }

    // Batch parameter updates (to avoid triggering rebuild for each parameter)
    void beginParameterUpdate();
    void endParameterUpdate();

    // Custom envelope data storage
    void setCustomEnvelopeData(const EnvelopeData &data);
    EnvelopeData getCustomEnvelopeData() const { return customEnvelopeData; }
    bool hasCustomEnvelopeData() const { return customEnvelopeData.points.size() > 0; }


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
      void parameterChanged();


private:
    Ui::Container *ui;
    bool dragging;
    QPoint dragStartPosition;
    QColor containerColor;
    void positionPortCircles();
    QVector<PortInfo> ports;
    QString containerName;
    QString instanceName;  // User-editable instance name
    bool isSelected = false;
    QMap<QString, double> parameters;  // Internal parameters (config)
    EnvelopeData customEnvelopeData;  // Custom envelope data (for Envelope Engine)
    bool batchUpdateInProgress = false;  // True when batching parameter updates

 ~Container();
};

#endif // CONTAINER_H
