#ifndef SOUNITBUILDER_H
#define SOUNITBUILDER_H

#include <QMainWindow>

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
};

#endif // SOUNITBUILDER_H
