#ifndef SOUNITBUILDER_H
#define SOUNITBUILDER_H

#include <QMainWindow>
#include <QLabel>
#include "container.h"
#include "canvas.h"

namespace Ui {
class SounitBuilderCanvas;
}

class SounitBuilder : public QMainWindow
{
    Q_OBJECT




public:
    explicit SounitBuilder(QWidget *parent = nullptr);
    ~SounitBuilder();


private:
    Ui::SounitBuilderCanvas *ui;
    // transport labels
    QLabel *labelPosition ;
    QLabel *labelTempo ;
    QLabel *labelTimeSignature ;
    Container *connectionStartContainer = nullptr;
    QString connectionStartPort;
    bool connectionStartIsOutput = false;
    QVector<Container*> containers;

    Canvas *canvas;

private slots:
    void onAddContainer(const QString &name, const QColor &color,
                        const QStringList &inputs, const QStringList &outputs);

    void onPortClicked(Container *container, const QString &portName, bool isOutput, QPoint globalPos);






};

#endif // SOUNITBUILDER_H
