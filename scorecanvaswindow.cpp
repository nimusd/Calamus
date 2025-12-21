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

    setupToolbarColors();
}


void ScoreCanvasWindow::setupToolbarColors()
{
    // Get the tool buttons from the top toolbar
    QToolButton* drawDiscreteBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionDrawDiscreteNotes));

    QToolButton* drawContinuousBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionDrawContinuousNotes));

    QToolButton* drawPhraseBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionDrawPhrase));

    QToolButton* recordDiscreteBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionRecordDiscreteNotes));

    QToolButton* recordContinuousBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionRecordContinuousNotes));

    QToolButton* recordPhraseBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionRecordPhrase));

    QToolButton* selectBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionScoreCanvasSelect));

    QToolButton* zoomBtn = qobject_cast<QToolButton*>(
        ui->toolBar->widgetForAction(ui->actionScoreCanvasZoom));

    // Apply colors
    if (drawDiscreteBtn) {
        drawDiscreteBtn->setStyleSheet("QToolButton { background-color: #90CAF9; }");  // Light blue
    }
    if (drawContinuousBtn) {
        drawContinuousBtn->setStyleSheet("QToolButton { background-color: #BBDEFB; }");
    }
    if (drawPhraseBtn) {
        drawPhraseBtn->setStyleSheet("QToolButton { background-color: #90CAF9; }");
    }

    if (recordDiscreteBtn) {
        recordDiscreteBtn->setStyleSheet("QToolButton { background-color: #FFCC80; }");  // Light orange
    }
    if (recordContinuousBtn) {
        recordContinuousBtn->setStyleSheet("QToolButton { background-color: #FFE0B2; }");
    }
    if (recordPhraseBtn) {
        recordPhraseBtn->setStyleSheet("QToolButton { background-color: #FFCC80; }");
    }

    if (selectBtn) {
        selectBtn->setStyleSheet("QToolButton { background-color: #A5D6A7; }");  // Light green
    }
    if (zoomBtn) {
        zoomBtn->setStyleSheet("QToolButton { background-color: #C8E6C9; }");
    }
}

ScoreCanvasWindow::~ScoreCanvasWindow()
{
    delete ui;
}
