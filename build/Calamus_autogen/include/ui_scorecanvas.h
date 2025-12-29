/********************************************************************************
** Form generated from reading UI file 'scorecanvas.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCORECANVAS_H
#define UI_SCORECANVAS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_scorecanvas
{
public:
    QAction *actionDrawDiscreteNotes;
    QAction *actionDrawContinuousNotes;
    QAction *actionDrawPhrase;
    QAction *actionRecordDiscreteNotes;
    QAction *actionRecordContinuousNotes;
    QAction *actionRecordPhrase;
    QAction *actionScoreCanvasSelect;
    QAction *actionScoreCanvasZoom;
    QAction *actionPlay;
    QAction *actionstop;
    QWidget *centralwidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayoutTimeline;
    QScrollArea *scrollAreatracks;
    QWidget *scrollAreaWidgetContents;
    QToolBar *toolBar;
    QToolBar *scoreCanvasBottomToolbar;

    void setupUi(QMainWindow *scorecanvas)
    {
        if (scorecanvas->objectName().isEmpty())
            scorecanvas->setObjectName("scorecanvas");
        scorecanvas->resize(1171, 930);
        actionDrawDiscreteNotes = new QAction(scorecanvas);
        actionDrawDiscreteNotes->setObjectName("actionDrawDiscreteNotes");
        actionDrawDiscreteNotes->setMenuRole(QAction::MenuRole::NoRole);
        actionDrawContinuousNotes = new QAction(scorecanvas);
        actionDrawContinuousNotes->setObjectName("actionDrawContinuousNotes");
        actionDrawContinuousNotes->setMenuRole(QAction::MenuRole::NoRole);
        actionDrawPhrase = new QAction(scorecanvas);
        actionDrawPhrase->setObjectName("actionDrawPhrase");
        actionDrawPhrase->setMenuRole(QAction::MenuRole::NoRole);
        actionRecordDiscreteNotes = new QAction(scorecanvas);
        actionRecordDiscreteNotes->setObjectName("actionRecordDiscreteNotes");
        actionRecordDiscreteNotes->setMenuRole(QAction::MenuRole::NoRole);
        actionRecordContinuousNotes = new QAction(scorecanvas);
        actionRecordContinuousNotes->setObjectName("actionRecordContinuousNotes");
        actionRecordContinuousNotes->setMenuRole(QAction::MenuRole::NoRole);
        actionRecordPhrase = new QAction(scorecanvas);
        actionRecordPhrase->setObjectName("actionRecordPhrase");
        actionRecordPhrase->setMenuRole(QAction::MenuRole::NoRole);
        actionScoreCanvasSelect = new QAction(scorecanvas);
        actionScoreCanvasSelect->setObjectName("actionScoreCanvasSelect");
        actionScoreCanvasSelect->setMenuRole(QAction::MenuRole::NoRole);
        actionScoreCanvasZoom = new QAction(scorecanvas);
        actionScoreCanvasZoom->setObjectName("actionScoreCanvasZoom");
        actionScoreCanvasZoom->setMenuRole(QAction::MenuRole::NoRole);
        actionPlay = new QAction(scorecanvas);
        actionPlay->setObjectName("actionPlay");
        actionPlay->setMenuRole(QAction::MenuRole::NoRole);
        actionstop = new QAction(scorecanvas);
        actionstop->setObjectName("actionstop");
        actionstop->setMenuRole(QAction::MenuRole::NoRole);
        centralwidget = new QWidget(scorecanvas);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8("background-color: white;"));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(49, -1, 751, 61));
        horizontalLayoutTimeline = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayoutTimeline->setObjectName("horizontalLayoutTimeline");
        horizontalLayoutTimeline->setContentsMargins(0, 0, 0, 0);
        scrollAreatracks = new QScrollArea(centralwidget);
        scrollAreatracks->setObjectName("scrollAreatracks");
        scrollAreatracks->setGeometry(QRect(0, -1, 51, 551));
        scrollAreatracks->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 49, 549));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        scrollAreatracks->setWidget(scrollAreaWidgetContents);
        scorecanvas->setCentralWidget(centralwidget);
        toolBar = new QToolBar(scorecanvas);
        toolBar->setObjectName("toolBar");
        scorecanvas->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
        scoreCanvasBottomToolbar = new QToolBar(scorecanvas);
        scoreCanvasBottomToolbar->setObjectName("scoreCanvasBottomToolbar");
        scoreCanvasBottomToolbar->setMinimumSize(QSize(0, 30));
        scoreCanvasBottomToolbar->setMaximumSize(QSize(16777215, 30));
        scorecanvas->addToolBar(Qt::ToolBarArea::BottomToolBarArea, scoreCanvasBottomToolbar);

        toolBar->addAction(actionDrawDiscreteNotes);
        toolBar->addAction(actionDrawContinuousNotes);
        toolBar->addAction(actionDrawPhrase);
        toolBar->addSeparator();
        toolBar->addAction(actionRecordDiscreteNotes);
        toolBar->addAction(actionRecordContinuousNotes);
        toolBar->addSeparator();
        toolBar->addAction(actionScoreCanvasSelect);
        toolBar->addAction(actionScoreCanvasZoom);
        scoreCanvasBottomToolbar->addAction(actionPlay);
        scoreCanvasBottomToolbar->addAction(actionstop);

        retranslateUi(scorecanvas);

        QMetaObject::connectSlotsByName(scorecanvas);
    } // setupUi

    void retranslateUi(QMainWindow *scorecanvas)
    {
        scorecanvas->setWindowTitle(QCoreApplication::translate("scorecanvas", "scoreCanvas", nullptr));
        actionDrawDiscreteNotes->setText(QCoreApplication::translate("scorecanvas", "Draw Discrete Notes", nullptr));
        actionDrawContinuousNotes->setText(QCoreApplication::translate("scorecanvas", "Draw Continuous Notes", nullptr));
        actionDrawPhrase->setText(QCoreApplication::translate("scorecanvas", "Draw Phrase", nullptr));
        actionRecordDiscreteNotes->setText(QCoreApplication::translate("scorecanvas", "Record Discrete Notes", nullptr));
        actionRecordContinuousNotes->setText(QCoreApplication::translate("scorecanvas", "Record Continuous Notes", nullptr));
        actionRecordPhrase->setText(QCoreApplication::translate("scorecanvas", "Record Phrase", nullptr));
        actionScoreCanvasSelect->setText(QCoreApplication::translate("scorecanvas", "Select", nullptr));
        actionScoreCanvasZoom->setText(QCoreApplication::translate("scorecanvas", "Zoom", nullptr));
        actionPlay->setText(QCoreApplication::translate("scorecanvas", "\342\226\266", nullptr));
        actionstop->setText(QCoreApplication::translate("scorecanvas", "\342\226\240", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("scorecanvas", "toolBar", nullptr));
        scoreCanvasBottomToolbar->setWindowTitle(QCoreApplication::translate("scorecanvas", "toolBar_2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class scorecanvas: public Ui_scorecanvas {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCORECANVAS_H
