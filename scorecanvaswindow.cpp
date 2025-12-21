#include "scorecanvaswindow.h"
#include "ui_scorecanvas.h"
#include <QToolButton>
#include <QLabel>

ScoreCanvasWindow::ScoreCanvasWindow(QWidget *parent)
    : QMainWindow{parent}
    ,ui(new Ui::scorecanvas)
{
    ui->setupUi(this);

    // Create transport labels
    QLabel *labelPosition = new QLabel("Playback time: 0:00:00  ", this);
    QLabel *labelTempo = new QLabel("Tempo: 120 BPM   ", this);
    QLabel *labelTimeSignature = new QLabel("Time signature: 4/4", this);

    // Add to bottom toolbar
    ui->scoreCanvasBottomToolbar->addWidget(labelPosition);
    ui->scoreCanvasBottomToolbar->addWidget(labelTempo);
    ui->scoreCanvasBottomToolbar->addWidget(labelTimeSignature);
}

ScoreCanvasWindow::~ScoreCanvasWindow()
{
    delete ui;
}
