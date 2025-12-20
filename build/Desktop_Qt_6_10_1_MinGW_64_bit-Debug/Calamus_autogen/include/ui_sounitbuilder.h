/********************************************************************************
** Form generated from reading UI file 'sounitbuilder.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SOUNITBUILDER_H
#define UI_SOUNITBUILDER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SounitBuilderCanvas
{
public:
    QAction *actionHarmonicGenerator;
    QAction *actionSpectrum_to_Signal;
    QAction *actionRollofff_processor;
    QAction *actionFormant_body;
    QAction *actionBreath_Turbulence;
    QAction *actionNoise_Color_Filter;
    QAction *actionPhysics_System;
    QAction *actionEasing_Applicator;
    QAction *actionEnvelope_Engine;
    QAction *actionDrift_Engine;
    QAction *actionGate_Processor;
    QAction *actionMarquee_Select;
    QAction *actionplay;
    QAction *actionstop;
    QWidget *centralsounitbuildercanvaswidget;
    QToolBar *toolBar;
    QToolBar *sounitBuilderBottomToolbar;

    void setupUi(QMainWindow *SounitBuilderCanvas)
    {
        if (SounitBuilderCanvas->objectName().isEmpty())
            SounitBuilderCanvas->setObjectName("SounitBuilderCanvas");
        SounitBuilderCanvas->resize(1215, 713);
        SounitBuilderCanvas->setMinimumSize(QSize(0, 0));
        actionHarmonicGenerator = new QAction(SounitBuilderCanvas);
        actionHarmonicGenerator->setObjectName("actionHarmonicGenerator");
        actionHarmonicGenerator->setMenuRole(QAction::MenuRole::NoRole);
        actionSpectrum_to_Signal = new QAction(SounitBuilderCanvas);
        actionSpectrum_to_Signal->setObjectName("actionSpectrum_to_Signal");
        actionSpectrum_to_Signal->setMenuRole(QAction::MenuRole::NoRole);
        actionRollofff_processor = new QAction(SounitBuilderCanvas);
        actionRollofff_processor->setObjectName("actionRollofff_processor");
        actionRollofff_processor->setMenuRole(QAction::MenuRole::NoRole);
        actionFormant_body = new QAction(SounitBuilderCanvas);
        actionFormant_body->setObjectName("actionFormant_body");
        actionFormant_body->setMenuRole(QAction::MenuRole::NoRole);
        actionBreath_Turbulence = new QAction(SounitBuilderCanvas);
        actionBreath_Turbulence->setObjectName("actionBreath_Turbulence");
        actionBreath_Turbulence->setMenuRole(QAction::MenuRole::NoRole);
        actionNoise_Color_Filter = new QAction(SounitBuilderCanvas);
        actionNoise_Color_Filter->setObjectName("actionNoise_Color_Filter");
        actionNoise_Color_Filter->setMenuRole(QAction::MenuRole::NoRole);
        actionPhysics_System = new QAction(SounitBuilderCanvas);
        actionPhysics_System->setObjectName("actionPhysics_System");
        actionPhysics_System->setMenuRole(QAction::MenuRole::NoRole);
        actionEasing_Applicator = new QAction(SounitBuilderCanvas);
        actionEasing_Applicator->setObjectName("actionEasing_Applicator");
        actionEasing_Applicator->setMenuRole(QAction::MenuRole::NoRole);
        actionEnvelope_Engine = new QAction(SounitBuilderCanvas);
        actionEnvelope_Engine->setObjectName("actionEnvelope_Engine");
        actionEnvelope_Engine->setMenuRole(QAction::MenuRole::NoRole);
        actionDrift_Engine = new QAction(SounitBuilderCanvas);
        actionDrift_Engine->setObjectName("actionDrift_Engine");
        actionDrift_Engine->setMenuRole(QAction::MenuRole::NoRole);
        actionGate_Processor = new QAction(SounitBuilderCanvas);
        actionGate_Processor->setObjectName("actionGate_Processor");
        actionGate_Processor->setMenuRole(QAction::MenuRole::NoRole);
        actionMarquee_Select = new QAction(SounitBuilderCanvas);
        actionMarquee_Select->setObjectName("actionMarquee_Select");
        actionMarquee_Select->setMenuRole(QAction::MenuRole::NoRole);
        actionplay = new QAction(SounitBuilderCanvas);
        actionplay->setObjectName("actionplay");
        actionplay->setMenuRole(QAction::MenuRole::NoRole);
        actionstop = new QAction(SounitBuilderCanvas);
        actionstop->setObjectName("actionstop");
        actionstop->setMenuRole(QAction::MenuRole::NoRole);
        centralsounitbuildercanvaswidget = new QWidget(SounitBuilderCanvas);
        centralsounitbuildercanvaswidget->setObjectName("centralsounitbuildercanvaswidget");
        centralsounitbuildercanvaswidget->setStyleSheet(QString::fromUtf8("background-color: white;"));
        SounitBuilderCanvas->setCentralWidget(centralsounitbuildercanvaswidget);
        toolBar = new QToolBar(SounitBuilderCanvas);
        toolBar->setObjectName("toolBar");
        SounitBuilderCanvas->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
        sounitBuilderBottomToolbar = new QToolBar(SounitBuilderCanvas);
        sounitBuilderBottomToolbar->setObjectName("sounitBuilderBottomToolbar");
        sounitBuilderBottomToolbar->setMinimumSize(QSize(0, 30));
        sounitBuilderBottomToolbar->setMaximumSize(QSize(16777215, 30));
        SounitBuilderCanvas->addToolBar(Qt::ToolBarArea::BottomToolBarArea, sounitBuilderBottomToolbar);

        toolBar->addAction(actionHarmonicGenerator);
        toolBar->addAction(actionSpectrum_to_Signal);
        toolBar->addSeparator();
        toolBar->addAction(actionRollofff_processor);
        toolBar->addAction(actionFormant_body);
        toolBar->addAction(actionBreath_Turbulence);
        toolBar->addAction(actionNoise_Color_Filter);
        toolBar->addSeparator();
        toolBar->addAction(actionPhysics_System);
        toolBar->addAction(actionEasing_Applicator);
        toolBar->addAction(actionEnvelope_Engine);
        toolBar->addAction(actionDrift_Engine);
        toolBar->addAction(actionGate_Processor);
        toolBar->addSeparator();
        toolBar->addAction(actionMarquee_Select);
        sounitBuilderBottomToolbar->addAction(actionplay);
        sounitBuilderBottomToolbar->addSeparator();
        sounitBuilderBottomToolbar->addAction(actionstop);
        sounitBuilderBottomToolbar->addSeparator();

        retranslateUi(SounitBuilderCanvas);

        QMetaObject::connectSlotsByName(SounitBuilderCanvas);
    } // setupUi

    void retranslateUi(QMainWindow *SounitBuilderCanvas)
    {
        SounitBuilderCanvas->setWindowTitle(QCoreApplication::translate("SounitBuilderCanvas", "Sounit Builser", nullptr));
        actionHarmonicGenerator->setText(QCoreApplication::translate("SounitBuilderCanvas", "Harmonic Generator", nullptr));
        actionSpectrum_to_Signal->setText(QCoreApplication::translate("SounitBuilderCanvas", "Spectrum to Signal", nullptr));
        actionRollofff_processor->setText(QCoreApplication::translate("SounitBuilderCanvas", "Rollofff processor", nullptr));
        actionFormant_body->setText(QCoreApplication::translate("SounitBuilderCanvas", "Formant body", nullptr));
        actionBreath_Turbulence->setText(QCoreApplication::translate("SounitBuilderCanvas", "Breath Turbulence", nullptr));
        actionNoise_Color_Filter->setText(QCoreApplication::translate("SounitBuilderCanvas", "Noise Color Filter", nullptr));
        actionPhysics_System->setText(QCoreApplication::translate("SounitBuilderCanvas", "Physics System", nullptr));
        actionEasing_Applicator->setText(QCoreApplication::translate("SounitBuilderCanvas", "Easing Applicator", nullptr));
        actionEnvelope_Engine->setText(QCoreApplication::translate("SounitBuilderCanvas", "Envelope Engine", nullptr));
        actionDrift_Engine->setText(QCoreApplication::translate("SounitBuilderCanvas", "Drift Engine", nullptr));
        actionGate_Processor->setText(QCoreApplication::translate("SounitBuilderCanvas", "Gate Processor", nullptr));
        actionMarquee_Select->setText(QCoreApplication::translate("SounitBuilderCanvas", "Marquee Select", nullptr));
        actionplay->setText(QCoreApplication::translate("SounitBuilderCanvas", "\342\226\266", nullptr));
        actionstop->setText(QCoreApplication::translate("SounitBuilderCanvas", "\342\226\240", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("SounitBuilderCanvas", "toolBar", nullptr));
        sounitBuilderBottomToolbar->setWindowTitle(QCoreApplication::translate("SounitBuilderCanvas", "toolBar_2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SounitBuilderCanvas: public Ui_SounitBuilderCanvas {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOUNITBUILDER_H
