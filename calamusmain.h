#ifndef CALAMUSMAIN_H
#define CALAMUSMAIN_H

#include <QMainWindow>
#include "sounitbuilder.h"
#include "scorecanvaswindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class CalamusMain;
}
QT_END_NAMESPACE

class CalamusMain : public QMainWindow
{
    Q_OBJECT

public:
    CalamusMain(QWidget *parent = nullptr);
    ~CalamusMain();

private:
    Ui::CalamusMain *ui;

private:
    SounitBuilder *sounitBuilder;
    ScoreCanvasWindow *scoreCanvasWindow;

private slots:
    void onTabChanged(int index);
};

#endif // CALAMUSMAIN_H
