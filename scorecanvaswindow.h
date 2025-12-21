#ifndef SCORECANVASWINDOW_H
#define SCORECANVASWINDOW_H

#include "ui_scorecanvas.h"
#include <QMainWindow>
#include <QLabel>

namespace Ui {
class ScoreCanvasWindow;
}

class ScoreCanvasWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ScoreCanvasWindow(QWidget *parent = nullptr);
    ~ScoreCanvasWindow();
private:
    Ui::scorecanvas *ui;

signals:
};

#endif // SCORECANVASWINDOW_H
