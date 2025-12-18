#include "calamusmain.h"
#include "ui_calamusmain.h"


CalamusMain::CalamusMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalamusMain)
{
    ui->setupUi(this);
    sounitBuilder = new SounitBuilder(this);
    sounitBuilder->show();
}

CalamusMain::~CalamusMain()
{
    delete ui;
}
