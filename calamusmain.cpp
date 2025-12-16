#include "calamusmain.h"
#include "ui_calamusmain.h"

CalamusMain::CalamusMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalamusMain)
{
    ui->setupUi(this);
}

CalamusMain::~CalamusMain()
{
    delete ui;
}
