#include "calamusmain.h"
#include "ui_calamusmain.h"


CalamusMain::CalamusMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalamusMain)
{
    ui->setupUi(this);
    sounitBuilder = new SounitBuilder(this);
    connect(ui->MainTab, &QTabWidget::currentChanged, this, &CalamusMain::onTabChanged);
    sounitBuilder->show();
}

void CalamusMain::onTabChanged(int index)
{
    if (index == 0) {  // Sound Engine tab
        sounitBuilder->show();
    } else if (index == 1) {  // Composition tab
        sounitBuilder->hide();
        // scoreCanvas->show();  // for later
    }
    // index == 2 (Preferences) — leave as-is
}

CalamusMain::~CalamusMain()
{
    delete ui;
}
