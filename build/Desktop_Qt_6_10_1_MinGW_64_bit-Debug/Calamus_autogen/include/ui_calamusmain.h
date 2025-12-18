/********************************************************************************
** Form generated from reading UI file 'calamusmain.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALAMUSMAIN_H
#define UI_CALAMUSMAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalamusMain
{
public:
    QAction *actionNew_project_Ctrl_N;
    QAction *actionOpen_ctrl_o;
    QAction *actionOpen_Recent;
    QAction *actionSave_ctrl_S;
    QAction *actionSave_AS_Ctrl_SHift_S;
    QAction *actionExport_Audio_Ctrl_E;
    QAction *actionExit_Alt_F4;
    QAction *actionUndo_Ctrl_Z;
    QAction *actionRedo_Ctrl_Y;
    QAction *actionCut_Ctrl_X;
    QAction *actionCopy_Ctrl_C;
    QAction *actionPaste_Ctrl_V;
    QAction *actionDuplicate_Ctrl_D;
    QAction *actionDelete_Del;
    QAction *actionSelect_All_ctrl_A;
    QAction *actionDeselect_Esc;
    QAction *actionNew_Sounit;
    QAction *actionLoad_Sounit;
    QAction *actionSave_Sounit_As;
    QAction *actionHarmonic_Generator;
    QAction *actionSpectrum_To_Signal;
    QAction *actionRolloff_Processor;
    QAction *actionFormant_Body;
    QAction *actionBreth_Turbulance;
    QAction *actionNoise_Color_Filter;
    QAction *actionPhysics_System;
    QAction *actionEasing_Applicator;
    QAction *actionEnvelope_Engine;
    QAction *actionDrift_Engine;
    QAction *actionGate_Processor;
    QAction *actionDelete_Container;
    QAction *actionDelete_Container_2;
    QAction *actionDNA_Library;
    QAction *actionEnvelope_Library;
    QAction *actionEasing_Library;
    QAction *actionNew_track;
    QAction *actionDelete_Track;
    QAction *actionNew_Phrase;
    QAction *actionLoad_Phrase;
    QAction *actionSave_phrase_As;
    QAction *actionScale_Settings;
    QAction *actionAssign_Sounit_to_Track;
    QAction *actionCanvas_Window;
    QAction *actionFull_Screen_Canvas_F11;
    QAction *actionZoom_In_Ctrl;
    QAction *actionZoom_Out_Crl;
    QAction *actionZoom_to_fit_Ctrl_0;
    QAction *actionSound_Engine_Tab_Ctrl_1;
    QAction *actionCompisition_Tab_Ctrl_2;
    QAction *actionPreferences_Tab_Ctrl_3;
    QAction *actionDocumentation_F1;
    QAction *actionKeyboard_Shortcuts;
    QAction *actionAbout_Calamus;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *MainTab;
    QWidget *soundEngineTab;
    QGroupBox *groupSounitInfo;
    QFormLayout *formLayout_2;
    QLabel *label;
    QLineEdit *editSounitName;
    QLabel *label_2;
    QLineEdit *editSounitUsedBy;
    QLabel *label_3;
    QLineEdit *editSounitComment;
    QGroupBox *groupConnectionInspector;
    QFormLayout *formLayout_3;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *labelConnectionTo;
    QLabel *label_8;
    QComboBox *comboConnectionFunction;
    QLabel *label_9;
    QDoubleSpinBox *spinConnectionWeight;
    QPushButton *buttonDisconnect;
    QLabel *labelConnectionFrom;
    QGroupBox *groupContainerInspector;
    QFrame *frame_2;
    QLabel *label_4;
    QLabel *labelContainerType;
    QLabel *label_5;
    QLineEdit *editInstanceName;
    QGroupBox *groupConfig;
    QListWidget *listWidget_3;
    QGroupBox *groupInputs;
    QListWidget *listWidget;
    QGroupBox *groupOutputs;
    QListWidget *listWidget_2;
    QWidget *compositionTab;
    QWidget *preferencesTab;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuSound;
    QMenu *menuAdd_Container;
    QMenu *menuEssential;
    QMenu *menuShaping;
    QMenu *menuModifiers;
    QMenu *menuCompose;
    QMenu *menuView;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *CalamusMain)
    {
        if (CalamusMain->objectName().isEmpty())
            CalamusMain->setObjectName("CalamusMain");
        CalamusMain->resize(1089, 760);
        actionNew_project_Ctrl_N = new QAction(CalamusMain);
        actionNew_project_Ctrl_N->setObjectName("actionNew_project_Ctrl_N");
        actionOpen_ctrl_o = new QAction(CalamusMain);
        actionOpen_ctrl_o->setObjectName("actionOpen_ctrl_o");
        actionOpen_Recent = new QAction(CalamusMain);
        actionOpen_Recent->setObjectName("actionOpen_Recent");
        actionSave_ctrl_S = new QAction(CalamusMain);
        actionSave_ctrl_S->setObjectName("actionSave_ctrl_S");
        actionSave_AS_Ctrl_SHift_S = new QAction(CalamusMain);
        actionSave_AS_Ctrl_SHift_S->setObjectName("actionSave_AS_Ctrl_SHift_S");
        actionExport_Audio_Ctrl_E = new QAction(CalamusMain);
        actionExport_Audio_Ctrl_E->setObjectName("actionExport_Audio_Ctrl_E");
        actionExit_Alt_F4 = new QAction(CalamusMain);
        actionExit_Alt_F4->setObjectName("actionExit_Alt_F4");
        actionUndo_Ctrl_Z = new QAction(CalamusMain);
        actionUndo_Ctrl_Z->setObjectName("actionUndo_Ctrl_Z");
        actionRedo_Ctrl_Y = new QAction(CalamusMain);
        actionRedo_Ctrl_Y->setObjectName("actionRedo_Ctrl_Y");
        actionCut_Ctrl_X = new QAction(CalamusMain);
        actionCut_Ctrl_X->setObjectName("actionCut_Ctrl_X");
        actionCopy_Ctrl_C = new QAction(CalamusMain);
        actionCopy_Ctrl_C->setObjectName("actionCopy_Ctrl_C");
        actionPaste_Ctrl_V = new QAction(CalamusMain);
        actionPaste_Ctrl_V->setObjectName("actionPaste_Ctrl_V");
        actionDuplicate_Ctrl_D = new QAction(CalamusMain);
        actionDuplicate_Ctrl_D->setObjectName("actionDuplicate_Ctrl_D");
        actionDelete_Del = new QAction(CalamusMain);
        actionDelete_Del->setObjectName("actionDelete_Del");
        actionSelect_All_ctrl_A = new QAction(CalamusMain);
        actionSelect_All_ctrl_A->setObjectName("actionSelect_All_ctrl_A");
        actionDeselect_Esc = new QAction(CalamusMain);
        actionDeselect_Esc->setObjectName("actionDeselect_Esc");
        actionNew_Sounit = new QAction(CalamusMain);
        actionNew_Sounit->setObjectName("actionNew_Sounit");
        actionLoad_Sounit = new QAction(CalamusMain);
        actionLoad_Sounit->setObjectName("actionLoad_Sounit");
        actionSave_Sounit_As = new QAction(CalamusMain);
        actionSave_Sounit_As->setObjectName("actionSave_Sounit_As");
        actionHarmonic_Generator = new QAction(CalamusMain);
        actionHarmonic_Generator->setObjectName("actionHarmonic_Generator");
        actionSpectrum_To_Signal = new QAction(CalamusMain);
        actionSpectrum_To_Signal->setObjectName("actionSpectrum_To_Signal");
        actionRolloff_Processor = new QAction(CalamusMain);
        actionRolloff_Processor->setObjectName("actionRolloff_Processor");
        actionFormant_Body = new QAction(CalamusMain);
        actionFormant_Body->setObjectName("actionFormant_Body");
        actionBreth_Turbulance = new QAction(CalamusMain);
        actionBreth_Turbulance->setObjectName("actionBreth_Turbulance");
        actionNoise_Color_Filter = new QAction(CalamusMain);
        actionNoise_Color_Filter->setObjectName("actionNoise_Color_Filter");
        actionPhysics_System = new QAction(CalamusMain);
        actionPhysics_System->setObjectName("actionPhysics_System");
        actionEasing_Applicator = new QAction(CalamusMain);
        actionEasing_Applicator->setObjectName("actionEasing_Applicator");
        actionEnvelope_Engine = new QAction(CalamusMain);
        actionEnvelope_Engine->setObjectName("actionEnvelope_Engine");
        actionDrift_Engine = new QAction(CalamusMain);
        actionDrift_Engine->setObjectName("actionDrift_Engine");
        actionGate_Processor = new QAction(CalamusMain);
        actionGate_Processor->setObjectName("actionGate_Processor");
        actionDelete_Container = new QAction(CalamusMain);
        actionDelete_Container->setObjectName("actionDelete_Container");
        actionDelete_Container_2 = new QAction(CalamusMain);
        actionDelete_Container_2->setObjectName("actionDelete_Container_2");
        actionDNA_Library = new QAction(CalamusMain);
        actionDNA_Library->setObjectName("actionDNA_Library");
        actionEnvelope_Library = new QAction(CalamusMain);
        actionEnvelope_Library->setObjectName("actionEnvelope_Library");
        actionEasing_Library = new QAction(CalamusMain);
        actionEasing_Library->setObjectName("actionEasing_Library");
        actionNew_track = new QAction(CalamusMain);
        actionNew_track->setObjectName("actionNew_track");
        actionDelete_Track = new QAction(CalamusMain);
        actionDelete_Track->setObjectName("actionDelete_Track");
        actionNew_Phrase = new QAction(CalamusMain);
        actionNew_Phrase->setObjectName("actionNew_Phrase");
        actionLoad_Phrase = new QAction(CalamusMain);
        actionLoad_Phrase->setObjectName("actionLoad_Phrase");
        actionSave_phrase_As = new QAction(CalamusMain);
        actionSave_phrase_As->setObjectName("actionSave_phrase_As");
        actionScale_Settings = new QAction(CalamusMain);
        actionScale_Settings->setObjectName("actionScale_Settings");
        actionAssign_Sounit_to_Track = new QAction(CalamusMain);
        actionAssign_Sounit_to_Track->setObjectName("actionAssign_Sounit_to_Track");
        actionCanvas_Window = new QAction(CalamusMain);
        actionCanvas_Window->setObjectName("actionCanvas_Window");
        actionFull_Screen_Canvas_F11 = new QAction(CalamusMain);
        actionFull_Screen_Canvas_F11->setObjectName("actionFull_Screen_Canvas_F11");
        actionZoom_In_Ctrl = new QAction(CalamusMain);
        actionZoom_In_Ctrl->setObjectName("actionZoom_In_Ctrl");
        actionZoom_Out_Crl = new QAction(CalamusMain);
        actionZoom_Out_Crl->setObjectName("actionZoom_Out_Crl");
        actionZoom_to_fit_Ctrl_0 = new QAction(CalamusMain);
        actionZoom_to_fit_Ctrl_0->setObjectName("actionZoom_to_fit_Ctrl_0");
        actionSound_Engine_Tab_Ctrl_1 = new QAction(CalamusMain);
        actionSound_Engine_Tab_Ctrl_1->setObjectName("actionSound_Engine_Tab_Ctrl_1");
        actionCompisition_Tab_Ctrl_2 = new QAction(CalamusMain);
        actionCompisition_Tab_Ctrl_2->setObjectName("actionCompisition_Tab_Ctrl_2");
        actionPreferences_Tab_Ctrl_3 = new QAction(CalamusMain);
        actionPreferences_Tab_Ctrl_3->setObjectName("actionPreferences_Tab_Ctrl_3");
        actionDocumentation_F1 = new QAction(CalamusMain);
        actionDocumentation_F1->setObjectName("actionDocumentation_F1");
        actionKeyboard_Shortcuts = new QAction(CalamusMain);
        actionKeyboard_Shortcuts->setObjectName("actionKeyboard_Shortcuts");
        actionAbout_Calamus = new QAction(CalamusMain);
        actionAbout_Calamus->setObjectName("actionAbout_Calamus");
        centralwidget = new QWidget(CalamusMain);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        MainTab = new QTabWidget(centralwidget);
        MainTab->setObjectName("MainTab");
        soundEngineTab = new QWidget();
        soundEngineTab->setObjectName("soundEngineTab");
        groupSounitInfo = new QGroupBox(soundEngineTab);
        groupSounitInfo->setObjectName("groupSounitInfo");
        groupSounitInfo->setGeometry(QRect(10, 0, 341, 111));
        formLayout_2 = new QFormLayout(groupSounitInfo);
        formLayout_2->setObjectName("formLayout_2");
        label = new QLabel(groupSounitInfo);
        label->setObjectName("label");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        editSounitName = new QLineEdit(groupSounitInfo);
        editSounitName->setObjectName("editSounitName");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, editSounitName);

        label_2 = new QLabel(groupSounitInfo);
        label_2->setObjectName("label_2");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        editSounitUsedBy = new QLineEdit(groupSounitInfo);
        editSounitUsedBy->setObjectName("editSounitUsedBy");
        editSounitUsedBy->setReadOnly(true);

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, editSounitUsedBy);

        label_3 = new QLabel(groupSounitInfo);
        label_3->setObjectName("label_3");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        editSounitComment = new QLineEdit(groupSounitInfo);
        editSounitComment->setObjectName("editSounitComment");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::FieldRole, editSounitComment);

        groupConnectionInspector = new QGroupBox(soundEngineTab);
        groupConnectionInspector->setObjectName("groupConnectionInspector");
        groupConnectionInspector->setGeometry(QRect(10, 510, 351, 151));
        formLayout_3 = new QFormLayout(groupConnectionInspector);
        formLayout_3->setObjectName("formLayout_3");
        label_6 = new QLabel(groupConnectionInspector);
        label_6->setObjectName("label_6");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::LabelRole, label_6);

        label_7 = new QLabel(groupConnectionInspector);
        label_7->setObjectName("label_7");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::LabelRole, label_7);

        labelConnectionTo = new QLabel(groupConnectionInspector);
        labelConnectionTo->setObjectName("labelConnectionTo");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::FieldRole, labelConnectionTo);

        label_8 = new QLabel(groupConnectionInspector);
        label_8->setObjectName("label_8");

        formLayout_3->setWidget(2, QFormLayout::ItemRole::LabelRole, label_8);

        comboConnectionFunction = new QComboBox(groupConnectionInspector);
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->setObjectName("comboConnectionFunction");

        formLayout_3->setWidget(2, QFormLayout::ItemRole::FieldRole, comboConnectionFunction);

        label_9 = new QLabel(groupConnectionInspector);
        label_9->setObjectName("label_9");

        formLayout_3->setWidget(3, QFormLayout::ItemRole::LabelRole, label_9);

        spinConnectionWeight = new QDoubleSpinBox(groupConnectionInspector);
        spinConnectionWeight->setObjectName("spinConnectionWeight");

        formLayout_3->setWidget(3, QFormLayout::ItemRole::FieldRole, spinConnectionWeight);

        buttonDisconnect = new QPushButton(groupConnectionInspector);
        buttonDisconnect->setObjectName("buttonDisconnect");

        formLayout_3->setWidget(4, QFormLayout::ItemRole::LabelRole, buttonDisconnect);

        labelConnectionFrom = new QLabel(groupConnectionInspector);
        labelConnectionFrom->setObjectName("labelConnectionFrom");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::FieldRole, labelConnectionFrom);

        groupContainerInspector = new QGroupBox(soundEngineTab);
        groupContainerInspector->setObjectName("groupContainerInspector");
        groupContainerInspector->setGeometry(QRect(10, 119, 1021, 381));
        frame_2 = new QFrame(groupContainerInspector);
        frame_2->setObjectName("frame_2");
        frame_2->setGeometry(QRect(0, 20, 1011, 335));
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        label_4 = new QLabel(frame_2);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(8, 8, 26, 16));
        labelContainerType = new QLabel(frame_2);
        labelContainerType->setObjectName("labelContainerType");
        labelContainerType->setGeometry(QRect(55, 8, 41, 16));
        label_5 = new QLabel(frame_2);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(8, 27, 42, 16));
        editInstanceName = new QLineEdit(frame_2);
        editInstanceName->setObjectName("editInstanceName");
        editInstanceName->setGeometry(QRect(55, 27, 113, 19));
        groupConfig = new QGroupBox(frame_2);
        groupConfig->setObjectName("groupConfig");
        groupConfig->setGeometry(QRect(500, 200, 315, 111));
        listWidget_3 = new QListWidget(groupConfig);
        listWidget_3->setObjectName("listWidget_3");
        listWidget_3->setGeometry(QRect(30, 20, 241, 91));
        listWidget_3->setMinimumSize(QSize(0, 0));
        groupInputs = new QGroupBox(frame_2);
        groupInputs->setObjectName("groupInputs");
        groupInputs->setGeometry(QRect(0, 70, 481, 251));
        listWidget = new QListWidget(groupInputs);
        listWidget->setObjectName("listWidget");
        listWidget->setGeometry(QRect(30, 20, 261, 211));
        listWidget->setMinimumSize(QSize(0, 0));
        groupOutputs = new QGroupBox(frame_2);
        groupOutputs->setObjectName("groupOutputs");
        groupOutputs->setGeometry(QRect(500, 70, 381, 109));
        listWidget_2 = new QListWidget(groupOutputs);
        listWidget_2->setObjectName("listWidget_2");
        listWidget_2->setGeometry(QRect(30, 20, 241, 81));
        listWidget_2->setMinimumSize(QSize(0, 0));
        MainTab->addTab(soundEngineTab, QString());
        compositionTab = new QWidget();
        compositionTab->setObjectName("compositionTab");
        MainTab->addTab(compositionTab, QString());
        preferencesTab = new QWidget();
        preferencesTab->setObjectName("preferencesTab");
        MainTab->addTab(preferencesTab, QString());

        verticalLayout->addWidget(MainTab);

        CalamusMain->setCentralWidget(centralwidget);
        menubar = new QMenuBar(CalamusMain);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1089, 18));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        menuSound = new QMenu(menubar);
        menuSound->setObjectName("menuSound");
        menuAdd_Container = new QMenu(menuSound);
        menuAdd_Container->setObjectName("menuAdd_Container");
        menuEssential = new QMenu(menuAdd_Container);
        menuEssential->setObjectName("menuEssential");
        menuShaping = new QMenu(menuAdd_Container);
        menuShaping->setObjectName("menuShaping");
        menuModifiers = new QMenu(menuAdd_Container);
        menuModifiers->setObjectName("menuModifiers");
        menuCompose = new QMenu(menubar);
        menuCompose->setObjectName("menuCompose");
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        CalamusMain->setMenuBar(menubar);
        statusbar = new QStatusBar(CalamusMain);
        statusbar->setObjectName("statusbar");
        CalamusMain->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuSound->menuAction());
        menubar->addAction(menuCompose->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew_project_Ctrl_N);
        menuFile->addAction(actionOpen_ctrl_o);
        menuFile->addAction(actionOpen_Recent);
        menuFile->addAction(actionSave_ctrl_S);
        menuFile->addAction(actionSave_AS_Ctrl_SHift_S);
        menuFile->addSeparator();
        menuFile->addAction(actionExport_Audio_Ctrl_E);
        menuFile->addSeparator();
        menuFile->addAction(actionExit_Alt_F4);
        menuEdit->addAction(actionUndo_Ctrl_Z);
        menuEdit->addAction(actionRedo_Ctrl_Y);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCut_Ctrl_X);
        menuEdit->addAction(actionCopy_Ctrl_C);
        menuEdit->addAction(actionPaste_Ctrl_V);
        menuEdit->addAction(actionDuplicate_Ctrl_D);
        menuEdit->addAction(actionDelete_Del);
        menuEdit->addSeparator();
        menuEdit->addAction(actionSelect_All_ctrl_A);
        menuEdit->addAction(actionDeselect_Esc);
        menuSound->addAction(actionNew_Sounit);
        menuSound->addAction(actionLoad_Sounit);
        menuSound->addAction(actionSave_Sounit_As);
        menuSound->addSeparator();
        menuSound->addAction(menuAdd_Container->menuAction());
        menuSound->addAction(actionDelete_Container_2);
        menuSound->addSeparator();
        menuSound->addAction(actionDNA_Library);
        menuSound->addAction(actionEnvelope_Library);
        menuSound->addAction(actionEasing_Library);
        menuAdd_Container->addAction(menuEssential->menuAction());
        menuAdd_Container->addAction(menuShaping->menuAction());
        menuAdd_Container->addAction(menuModifiers->menuAction());
        menuAdd_Container->addAction(actionGate_Processor);
        menuEssential->addAction(actionHarmonic_Generator);
        menuEssential->addAction(actionSpectrum_To_Signal);
        menuShaping->addAction(actionRolloff_Processor);
        menuShaping->addAction(actionFormant_Body);
        menuShaping->addAction(actionBreth_Turbulance);
        menuShaping->addAction(actionNoise_Color_Filter);
        menuModifiers->addAction(actionPhysics_System);
        menuModifiers->addAction(actionEasing_Applicator);
        menuModifiers->addAction(actionEnvelope_Engine);
        menuModifiers->addAction(actionDrift_Engine);
        menuCompose->addAction(actionNew_track);
        menuCompose->addAction(actionDelete_Track);
        menuCompose->addSeparator();
        menuCompose->addAction(actionNew_Phrase);
        menuCompose->addAction(actionLoad_Phrase);
        menuCompose->addAction(actionSave_phrase_As);
        menuCompose->addSeparator();
        menuCompose->addAction(actionScale_Settings);
        menuCompose->addSeparator();
        menuCompose->addAction(actionAssign_Sounit_to_Track);
        menuView->addAction(actionCanvas_Window);
        menuView->addAction(actionFull_Screen_Canvas_F11);
        menuView->addSeparator();
        menuView->addAction(actionZoom_In_Ctrl);
        menuView->addAction(actionZoom_Out_Crl);
        menuView->addAction(actionZoom_to_fit_Ctrl_0);
        menuView->addSeparator();
        menuView->addAction(actionSound_Engine_Tab_Ctrl_1);
        menuView->addAction(actionCompisition_Tab_Ctrl_2);
        menuView->addAction(actionPreferences_Tab_Ctrl_3);
        menuHelp->addAction(actionDocumentation_F1);
        menuHelp->addAction(actionKeyboard_Shortcuts);
        menuHelp->addSeparator();
        menuHelp->addSeparator();
        menuHelp->addAction(actionAbout_Calamus);

        retranslateUi(CalamusMain);

        MainTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CalamusMain);
    } // setupUi

    void retranslateUi(QMainWindow *CalamusMain)
    {
        CalamusMain->setWindowTitle(QCoreApplication::translate("CalamusMain", "Calamus", nullptr));
        actionNew_project_Ctrl_N->setText(QCoreApplication::translate("CalamusMain", "New project (Ctrl + N)", nullptr));
        actionOpen_ctrl_o->setText(QCoreApplication::translate("CalamusMain", "Open (ctrl +o)", nullptr));
        actionOpen_Recent->setText(QCoreApplication::translate("CalamusMain", "Open Recent ", nullptr));
        actionSave_ctrl_S->setText(QCoreApplication::translate("CalamusMain", "Save (ctrl + S)", nullptr));
        actionSave_AS_Ctrl_SHift_S->setText(QCoreApplication::translate("CalamusMain", "Save AS (Ctrl+SHift+S)", nullptr));
        actionExport_Audio_Ctrl_E->setText(QCoreApplication::translate("CalamusMain", "Export Audio (Ctrl + E)", nullptr));
        actionExit_Alt_F4->setText(QCoreApplication::translate("CalamusMain", "Exit (Alt + F4)", nullptr));
        actionUndo_Ctrl_Z->setText(QCoreApplication::translate("CalamusMain", "Undo (Ctrl+Z)", nullptr));
        actionRedo_Ctrl_Y->setText(QCoreApplication::translate("CalamusMain", "Redo (Ctrl+Y)", nullptr));
        actionCut_Ctrl_X->setText(QCoreApplication::translate("CalamusMain", "Cut (Ctrl +X)", nullptr));
        actionCopy_Ctrl_C->setText(QCoreApplication::translate("CalamusMain", "Copy (Ctrl +C)", nullptr));
        actionPaste_Ctrl_V->setText(QCoreApplication::translate("CalamusMain", "Paste (Ctrl + V)", nullptr));
        actionDuplicate_Ctrl_D->setText(QCoreApplication::translate("CalamusMain", "Duplicate (Ctrl + D)", nullptr));
        actionDelete_Del->setText(QCoreApplication::translate("CalamusMain", "Delete (Del)", nullptr));
        actionSelect_All_ctrl_A->setText(QCoreApplication::translate("CalamusMain", "Select All (ctrl A)", nullptr));
        actionDeselect_Esc->setText(QCoreApplication::translate("CalamusMain", "Deselect (Esc)", nullptr));
        actionNew_Sounit->setText(QCoreApplication::translate("CalamusMain", "New Sounit", nullptr));
        actionLoad_Sounit->setText(QCoreApplication::translate("CalamusMain", "Load Sounit", nullptr));
        actionSave_Sounit_As->setText(QCoreApplication::translate("CalamusMain", "Save Sounit As", nullptr));
        actionHarmonic_Generator->setText(QCoreApplication::translate("CalamusMain", "Harmonic Generator", nullptr));
        actionSpectrum_To_Signal->setText(QCoreApplication::translate("CalamusMain", "Spectrum To Signal", nullptr));
        actionRolloff_Processor->setText(QCoreApplication::translate("CalamusMain", "Rolloff Processor", nullptr));
        actionFormant_Body->setText(QCoreApplication::translate("CalamusMain", "Formant Body", nullptr));
        actionBreth_Turbulance->setText(QCoreApplication::translate("CalamusMain", "Breath Turbulence", nullptr));
        actionNoise_Color_Filter->setText(QCoreApplication::translate("CalamusMain", "Noise Color Filter", nullptr));
        actionPhysics_System->setText(QCoreApplication::translate("CalamusMain", "Physics System", nullptr));
        actionEasing_Applicator->setText(QCoreApplication::translate("CalamusMain", "Easing Applicator", nullptr));
        actionEnvelope_Engine->setText(QCoreApplication::translate("CalamusMain", "Envelope Engine", nullptr));
        actionDrift_Engine->setText(QCoreApplication::translate("CalamusMain", "Drift Engine", nullptr));
        actionGate_Processor->setText(QCoreApplication::translate("CalamusMain", "Gate Processor", nullptr));
        actionDelete_Container->setText(QCoreApplication::translate("CalamusMain", "Delete Container", nullptr));
        actionDelete_Container_2->setText(QCoreApplication::translate("CalamusMain", "Delete Container", nullptr));
        actionDNA_Library->setText(QCoreApplication::translate("CalamusMain", "DNA Library", nullptr));
        actionEnvelope_Library->setText(QCoreApplication::translate("CalamusMain", "Envelope Library", nullptr));
        actionEasing_Library->setText(QCoreApplication::translate("CalamusMain", "Easing Library", nullptr));
        actionNew_track->setText(QCoreApplication::translate("CalamusMain", "New track", nullptr));
        actionDelete_Track->setText(QCoreApplication::translate("CalamusMain", "Delete Track", nullptr));
        actionNew_Phrase->setText(QCoreApplication::translate("CalamusMain", "New Phrase", nullptr));
        actionLoad_Phrase->setText(QCoreApplication::translate("CalamusMain", "Load Phrase", nullptr));
        actionSave_phrase_As->setText(QCoreApplication::translate("CalamusMain", "Save phrase As", nullptr));
        actionScale_Settings->setText(QCoreApplication::translate("CalamusMain", "Scale Settings", nullptr));
        actionAssign_Sounit_to_Track->setText(QCoreApplication::translate("CalamusMain", "Assign Sounit to Track", nullptr));
        actionCanvas_Window->setText(QCoreApplication::translate("CalamusMain", "Canvas Window", nullptr));
        actionFull_Screen_Canvas_F11->setText(QCoreApplication::translate("CalamusMain", "Full Screen Canvas (F11)", nullptr));
        actionZoom_In_Ctrl->setText(QCoreApplication::translate("CalamusMain", "Zoom In (Ctrl ++)", nullptr));
        actionZoom_Out_Crl->setText(QCoreApplication::translate("CalamusMain", "Zoom Out (Crl +-)", nullptr));
        actionZoom_to_fit_Ctrl_0->setText(QCoreApplication::translate("CalamusMain", "Zoom to fit (Ctrl + 0)", nullptr));
        actionSound_Engine_Tab_Ctrl_1->setText(QCoreApplication::translate("CalamusMain", "Sound Engine Tab (Ctrl + 1)", nullptr));
        actionCompisition_Tab_Ctrl_2->setText(QCoreApplication::translate("CalamusMain", "Composition Tab (Ctrl + 2)", nullptr));
        actionPreferences_Tab_Ctrl_3->setText(QCoreApplication::translate("CalamusMain", "Preferences Tab (Ctrl + 3)", nullptr));
        actionDocumentation_F1->setText(QCoreApplication::translate("CalamusMain", "Documentation (F1)", nullptr));
        actionKeyboard_Shortcuts->setText(QCoreApplication::translate("CalamusMain", "Keyboard Shortcuts", nullptr));
        actionAbout_Calamus->setText(QCoreApplication::translate("CalamusMain", "About Calamus", nullptr));
        groupSounitInfo->setTitle(QCoreApplication::translate("CalamusMain", "Sounit Info", nullptr));
        label->setText(QCoreApplication::translate("CalamusMain", "name:", nullptr));
        label_2->setText(QCoreApplication::translate("CalamusMain", "Used by:", nullptr));
        label_3->setText(QCoreApplication::translate("CalamusMain", "Comment", nullptr));
        groupConnectionInspector->setTitle(QCoreApplication::translate("CalamusMain", "Connection Inspector", nullptr));
        label_6->setText(QCoreApplication::translate("CalamusMain", "From:", nullptr));
        label_7->setText(QCoreApplication::translate("CalamusMain", "To:", nullptr));
        labelConnectionTo->setText(QCoreApplication::translate("CalamusMain", "TextLabel", nullptr));
        label_8->setText(QCoreApplication::translate("CalamusMain", "Function:", nullptr));
        comboConnectionFunction->setItemText(0, QCoreApplication::translate("CalamusMain", "passthrough", nullptr));
        comboConnectionFunction->setItemText(1, QCoreApplication::translate("CalamusMain", "add", nullptr));
        comboConnectionFunction->setItemText(2, QCoreApplication::translate("CalamusMain", "multiply", nullptr));
        comboConnectionFunction->setItemText(3, QCoreApplication::translate("CalamusMain", "substract", nullptr));
        comboConnectionFunction->setItemText(4, QCoreApplication::translate("CalamusMain", "replace", nullptr));
        comboConnectionFunction->setItemText(5, QCoreApplication::translate("CalamusMain", "modulate", nullptr));

        label_9->setText(QCoreApplication::translate("CalamusMain", "Weight:", nullptr));
        buttonDisconnect->setText(QCoreApplication::translate("CalamusMain", "Disconnect", nullptr));
        labelConnectionFrom->setText(QCoreApplication::translate("CalamusMain", "TextLabel", nullptr));
        groupContainerInspector->setTitle(QCoreApplication::translate("CalamusMain", "Container inspector", nullptr));
        label_4->setText(QCoreApplication::translate("CalamusMain", "Type: ", nullptr));
        labelContainerType->setText(QCoreApplication::translate("CalamusMain", "TextLabel", nullptr));
        label_5->setText(QCoreApplication::translate("CalamusMain", "Instance: ", nullptr));
        groupConfig->setTitle(QCoreApplication::translate("CalamusMain", "Config", nullptr));
        groupInputs->setTitle(QCoreApplication::translate("CalamusMain", "Inputs", nullptr));
        groupOutputs->setTitle(QCoreApplication::translate("CalamusMain", "Outputs", nullptr));
        MainTab->setTabText(MainTab->indexOf(soundEngineTab), QCoreApplication::translate("CalamusMain", "Sound Engine", nullptr));
        MainTab->setTabText(MainTab->indexOf(compositionTab), QCoreApplication::translate("CalamusMain", "Composition", nullptr));
        MainTab->setTabText(MainTab->indexOf(preferencesTab), QCoreApplication::translate("CalamusMain", "Preferences", nullptr));
        menuFile->setTitle(QCoreApplication::translate("CalamusMain", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("CalamusMain", "Edit", nullptr));
        menuSound->setTitle(QCoreApplication::translate("CalamusMain", "Sound", nullptr));
        menuAdd_Container->setTitle(QCoreApplication::translate("CalamusMain", "Add Container", nullptr));
        menuEssential->setTitle(QCoreApplication::translate("CalamusMain", "Essential", nullptr));
        menuShaping->setTitle(QCoreApplication::translate("CalamusMain", "Shaping", nullptr));
        menuModifiers->setTitle(QCoreApplication::translate("CalamusMain", "Modifiers", nullptr));
        menuCompose->setTitle(QCoreApplication::translate("CalamusMain", "Compose", nullptr));
        menuView->setTitle(QCoreApplication::translate("CalamusMain", "View", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("CalamusMain", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CalamusMain: public Ui_CalamusMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALAMUSMAIN_H
