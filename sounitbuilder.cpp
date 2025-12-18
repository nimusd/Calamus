#include "sounitbuilder.h"
#include "ui_sounitbuilder.h"
#include "container.h"
#include <QToolButton>

SounitBuilder::SounitBuilder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SounitBuilderCanvas)
{
    ui->setupUi(this);

    // Style toolbar buttons by category
    QString blueStyle = "QToolButton { background-color: #3498db; color: white; }";
    QString orangeStyle = "QToolButton { background-color: #e67e22; color: white; }";
    QString greenStyle = "QToolButton { background-color: #27ae60; color: white; }";

    // Essential - Blue
    ui->toolBar->widgetForAction(ui->actionHarmonicGenerator)->setStyleSheet(blueStyle);
    ui->toolBar->widgetForAction(ui->actionSpectrum_to_Signal)->setStyleSheet(blueStyle);

    // Shaping - Orange
    ui->toolBar->widgetForAction(ui->actionRollofff_processor)->setStyleSheet(orangeStyle);
    ui->toolBar->widgetForAction(ui->actionFormant_body)->setStyleSheet(orangeStyle);
    ui->toolBar->widgetForAction(ui->actionBreath_Turbulence)->setStyleSheet(orangeStyle);
    ui->toolBar->widgetForAction(ui->actionNoise_Color_Filter)->setStyleSheet(orangeStyle);

    // Modifiers - Green
    ui->toolBar->widgetForAction(ui->actionPhysics_System)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionEasing_Applicator)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionEnvelope_Engine)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionDrift_Engine)->setStyleSheet(greenStyle);
    ui->toolBar->widgetForAction(ui->actionGate_Processor)->setStyleSheet(greenStyle);

    Container *testContainer = new Container(ui->centralsounitbuildercanvaswidget);
    testContainer->move(100, 100);
    testContainer->show();

    Container *testContainer1 = new Container(ui->centralsounitbuildercanvaswidget);
    testContainer->move(400, 400);
    testContainer->show();
}

SounitBuilder::~SounitBuilder()
{
    delete ui;
}
