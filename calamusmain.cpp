#include "calamusmain.h"
#include "ui_calamusmain.h"
#include "scorecanvaswindow.h"

CalamusMain::CalamusMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalamusMain)
    , sounitBuilder(nullptr)    // Initialize pointers
    , scoreCanvasWindow(nullptr) // Initialize pointers
{
    ui->setupUi(this);

    // Create SounitBuilder
    sounitBuilder = new SounitBuilder(this);

    // Create ScoreCanvasWindow - NOT parented to this to make it independent
    scoreCanvasWindow = new ScoreCanvasWindow();
    scoreCanvasWindow->hide();  // Hide initially

    connect(ui->MainTab, &QTabWidget::currentChanged, this, &CalamusMain::onTabChanged);

    // Show initial tab content
    if(ui->MainTab->currentIndex() == 0)  {
        sounitBuilder->show();
    }
    else if (ui->MainTab->currentIndex() == 1)  {  // Fixed: was also checking for 0
        scoreCanvasWindow->show();
    }
}

void CalamusMain::onTabChanged(int index)
{
    if (index == 0) {  // Sound Engine tab
        if (sounitBuilder) {
            sounitBuilder->show();
        }
        if (scoreCanvasWindow) {
            scoreCanvasWindow->hide();  // Hide other window
        }
    } else if (index == 1) {  // Composition tab
        if (sounitBuilder) {
            sounitBuilder->hide();
        }
        if (scoreCanvasWindow) {
            scoreCanvasWindow->show();  // Show the score canvas
        }
    }
    // index == 2 (Preferences) — leave as-is
}

CalamusMain::~CalamusMain()
{
    delete ui;
    delete sounitBuilder;
    delete scoreCanvasWindow;  // Clean up
}
