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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
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
    QVBoxLayout *verticalLayout_4;
    QTabWidget *MainTab;
    QWidget *soundEngineTab;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupSounitInfo;
    QFormLayout *formLayout_2;
    QLabel *label;
    QLineEdit *editSounitName;
    QLabel *label_2;
    QLineEdit *editSounitUsedBy;
    QLabel *label_3;
    QLineEdit *editSounitComment;
    QGroupBox *groupContainerInspector;
    QVBoxLayout *verticalLayout_7;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_8;
    QLabel *labelContainerType;
    QLabel *label_4;
    QLineEdit *editInstanceName;
    QLabel *label_5;
    QGroupBox *groupOutputs;
    QVBoxLayout *verticalLayout_11;
    QListWidget *listWidget_2;
    QGroupBox *groupInputs;
    QVBoxLayout *verticalLayout_10;
    QListWidget *listWidget;
    QGroupBox *groupConfig;
    QVBoxLayout *verticalLayout_9;
    QListWidget *listWidget_3;
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
    QWidget *compositionTab;
    QVBoxLayout *verticalLayout_14;
    QGroupBox *groupPhraseSelector;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboPhraseSelector;
    QPushButton *btnPhraseLoad;
    QPushButton *btnPhraseSave;
    QGroupBox *groupInspector;
    QVBoxLayout *verticalLayout_6;
    QTabWidget *tabInspector;
    QWidget *tabNoteInspector;
    QGridLayout *gridLayout;
    QGroupBox *positiongroupBox;
    QVBoxLayout *verticalLayout;
    QLabel *label_11;
    QDoubleSpinBox *spinNoteStart;
    QLabel *label_14;
    QDoubleSpinBox *spinNoteDuration;
    QLabel *label_16;
    QSpinBox *spinNotePitch;
    QGroupBox *vibratoGroupBox;
    QFormLayout *formLayout;
    QCheckBox *checkNoteVibrato;
    QComboBox *comboNoteVibratoPreset;
    QPushButton *btnNoteVibratoEdit;
    QGroupBox *physicsGroupBox;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_20;
    QDoubleSpinBox *spinNoteMass;
    QGroupBox *commentGroupBox;
    QHBoxLayout *horizontalLayout_11;
    QLineEdit *editNoteComment;
    QWidget *tabPhraseInspector;
    QHBoxLayout *horizontalLayout_10;
    QGroupBox *groupPhraseIdentity;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *editPhraseName;
    QGroupBox *groupPhraseTranspose;
    QFormLayout *formLayout_4;
    QLabel *label_12;
    QComboBox *comboPhraseTransposeMode;
    QLabel *label_21;
    QSpinBox *spinPhraseTransposeValue;
    QGroupBox *groupPhraseVibrato;
    QFormLayout *formLayout_5;
    QLabel *label_22;
    QComboBox *comboPhraseVibratoPreset;
    QGroupBox *groupPhraseComment;
    QHBoxLayout *horizontalLayout_9;
    QLineEdit *editPhraseComment;
    QWidget *tabGestureInspector;
    QHBoxLayout *horizontalLayout_15;
    QGroupBox *groupGestureIdentity;
    QVBoxLayout *verticalLayout_16;
    QGroupBox *groupGestureNav;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *btnGesturePrev;
    QLabel *labelGestureIndex;
    QPushButton *btnGestureNext;
    QLabel *label_28;
    QLineEdit *lineEdit;
    QGroupBox *groupGesturePreview;
    QHBoxLayout *horizontalLayout_14;
    QFrame *frameGesturePreview;
    QGroupBox *groupGestureMappings;
    QGridLayout *gridLayout_2;
    QLabel *label_27;
    QComboBox *comboBox;
    QLabel *label_13;
    QComboBox *comboBox_6;
    QLabel *label_25;
    QComboBox *comboBox_4;
    QLabel *label_23;
    QComboBox *comboBox_2;
    QLabel *label_26;
    QComboBox *comboBox_5;
    QLabel *label_24;
    QComboBox *comboBox_3;
    QGroupBox *groupGestureActions;
    QVBoxLayout *verticalLayout_12;
    QPushButton *btnGestureSaveToLibrary;
    QPushButton *btnGestureRemove;
    QGroupBox *groupTracks;
    QHBoxLayout *horizontalLayout_6;
    QFrame *frameTracks;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupPenSettings;
    QVBoxLayout *verticalLayout_15;
    QFrame *frameModeLine;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelMode;
    QRadioButton *radioDraw;
    QRadioButton *radioSelect;
    QRadioButton *radioEdit;
    QLabel *labelDrawType;
    QRadioButton *radioDrawNotes;
    QRadioButton *radioDrawGesture;
    QFrame *frameMappingLine;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_10;
    QComboBox *comboMapPressure;
    QLabel *label_17;
    QComboBox *comboMapTiltX;
    QLabel *label_18;
    QComboBox *comboMapTiltY;
    QLabel *label_19;
    QComboBox *comboMapRotation;
    QGroupBox *groupScale;
    QHBoxLayout *horizontalLayout_5;
    QLabel *labelScaleDisplay;
    QPushButton *btnScaleEdit;
    QWidget *preferencesTab;
    QVBoxLayout *verticalLayout_13;
    QLabel *label_15;
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
        verticalLayout_4 = new QVBoxLayout(centralwidget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        MainTab = new QTabWidget(centralwidget);
        MainTab->setObjectName("MainTab");
        soundEngineTab = new QWidget();
        soundEngineTab->setObjectName("soundEngineTab");
        verticalLayout_5 = new QVBoxLayout(soundEngineTab);
        verticalLayout_5->setObjectName("verticalLayout_5");
        groupSounitInfo = new QGroupBox(soundEngineTab);
        groupSounitInfo->setObjectName("groupSounitInfo");
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


        verticalLayout_5->addWidget(groupSounitInfo);

        groupContainerInspector = new QGroupBox(soundEngineTab);
        groupContainerInspector->setObjectName("groupContainerInspector");
        verticalLayout_7 = new QVBoxLayout(groupContainerInspector);
        verticalLayout_7->setObjectName("verticalLayout_7");
        frame_2 = new QFrame(groupContainerInspector);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_8 = new QVBoxLayout(frame_2);
        verticalLayout_8->setObjectName("verticalLayout_8");
        labelContainerType = new QLabel(frame_2);
        labelContainerType->setObjectName("labelContainerType");

        verticalLayout_8->addWidget(labelContainerType);

        label_4 = new QLabel(frame_2);
        label_4->setObjectName("label_4");

        verticalLayout_8->addWidget(label_4);

        editInstanceName = new QLineEdit(frame_2);
        editInstanceName->setObjectName("editInstanceName");

        verticalLayout_8->addWidget(editInstanceName);

        label_5 = new QLabel(frame_2);
        label_5->setObjectName("label_5");

        verticalLayout_8->addWidget(label_5);

        groupOutputs = new QGroupBox(frame_2);
        groupOutputs->setObjectName("groupOutputs");
        verticalLayout_11 = new QVBoxLayout(groupOutputs);
        verticalLayout_11->setObjectName("verticalLayout_11");
        listWidget_2 = new QListWidget(groupOutputs);
        listWidget_2->setObjectName("listWidget_2");
        listWidget_2->setMinimumSize(QSize(0, 0));

        verticalLayout_11->addWidget(listWidget_2);


        verticalLayout_8->addWidget(groupOutputs);

        groupInputs = new QGroupBox(frame_2);
        groupInputs->setObjectName("groupInputs");
        verticalLayout_10 = new QVBoxLayout(groupInputs);
        verticalLayout_10->setObjectName("verticalLayout_10");
        listWidget = new QListWidget(groupInputs);
        listWidget->setObjectName("listWidget");
        listWidget->setMinimumSize(QSize(0, 0));

        verticalLayout_10->addWidget(listWidget);


        verticalLayout_8->addWidget(groupInputs);

        groupConfig = new QGroupBox(frame_2);
        groupConfig->setObjectName("groupConfig");
        verticalLayout_9 = new QVBoxLayout(groupConfig);
        verticalLayout_9->setObjectName("verticalLayout_9");
        listWidget_3 = new QListWidget(groupConfig);
        listWidget_3->setObjectName("listWidget_3");
        listWidget_3->setMinimumSize(QSize(0, 0));

        verticalLayout_9->addWidget(listWidget_3);


        verticalLayout_8->addWidget(groupConfig);


        verticalLayout_7->addWidget(frame_2);


        verticalLayout_5->addWidget(groupContainerInspector);

        groupConnectionInspector = new QGroupBox(soundEngineTab);
        groupConnectionInspector->setObjectName("groupConnectionInspector");
        formLayout_3 = new QFormLayout(groupConnectionInspector);
        formLayout_3->setObjectName("formLayout_3");
        label_6 = new QLabel(groupConnectionInspector);
        label_6->setObjectName("label_6");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::LabelRole, label_6);

        label_7 = new QLabel(groupConnectionInspector);
        label_7->setObjectName("label_7");

        formLayout_3->setWidget(2, QFormLayout::ItemRole::LabelRole, label_7);

        labelConnectionTo = new QLabel(groupConnectionInspector);
        labelConnectionTo->setObjectName("labelConnectionTo");

        formLayout_3->setWidget(2, QFormLayout::ItemRole::FieldRole, labelConnectionTo);

        label_8 = new QLabel(groupConnectionInspector);
        label_8->setObjectName("label_8");

        formLayout_3->setWidget(3, QFormLayout::ItemRole::LabelRole, label_8);

        comboConnectionFunction = new QComboBox(groupConnectionInspector);
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->addItem(QString());
        comboConnectionFunction->setObjectName("comboConnectionFunction");

        formLayout_3->setWidget(3, QFormLayout::ItemRole::FieldRole, comboConnectionFunction);

        label_9 = new QLabel(groupConnectionInspector);
        label_9->setObjectName("label_9");

        formLayout_3->setWidget(4, QFormLayout::ItemRole::LabelRole, label_9);

        spinConnectionWeight = new QDoubleSpinBox(groupConnectionInspector);
        spinConnectionWeight->setObjectName("spinConnectionWeight");

        formLayout_3->setWidget(4, QFormLayout::ItemRole::FieldRole, spinConnectionWeight);

        buttonDisconnect = new QPushButton(groupConnectionInspector);
        buttonDisconnect->setObjectName("buttonDisconnect");

        formLayout_3->setWidget(5, QFormLayout::ItemRole::LabelRole, buttonDisconnect);

        labelConnectionFrom = new QLabel(groupConnectionInspector);
        labelConnectionFrom->setObjectName("labelConnectionFrom");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::FieldRole, labelConnectionFrom);


        verticalLayout_5->addWidget(groupConnectionInspector);

        MainTab->addTab(soundEngineTab, QString());
        compositionTab = new QWidget();
        compositionTab->setObjectName("compositionTab");
        verticalLayout_14 = new QVBoxLayout(compositionTab);
        verticalLayout_14->setObjectName("verticalLayout_14");
        groupPhraseSelector = new QGroupBox(compositionTab);
        groupPhraseSelector->setObjectName("groupPhraseSelector");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupPhraseSelector->sizePolicy().hasHeightForWidth());
        groupPhraseSelector->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(groupPhraseSelector);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        comboPhraseSelector = new QComboBox(groupPhraseSelector);
        comboPhraseSelector->setObjectName("comboPhraseSelector");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboPhraseSelector->sizePolicy().hasHeightForWidth());
        comboPhraseSelector->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(comboPhraseSelector);

        btnPhraseLoad = new QPushButton(groupPhraseSelector);
        btnPhraseLoad->setObjectName("btnPhraseLoad");

        horizontalLayout_2->addWidget(btnPhraseLoad);

        btnPhraseSave = new QPushButton(groupPhraseSelector);
        btnPhraseSave->setObjectName("btnPhraseSave");

        horizontalLayout_2->addWidget(btnPhraseSave);


        verticalLayout_14->addWidget(groupPhraseSelector);

        groupInspector = new QGroupBox(compositionTab);
        groupInspector->setObjectName("groupInspector");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupInspector->sizePolicy().hasHeightForWidth());
        groupInspector->setSizePolicy(sizePolicy2);
        verticalLayout_6 = new QVBoxLayout(groupInspector);
        verticalLayout_6->setObjectName("verticalLayout_6");
        tabInspector = new QTabWidget(groupInspector);
        tabInspector->setObjectName("tabInspector");
        tabNoteInspector = new QWidget();
        tabNoteInspector->setObjectName("tabNoteInspector");
        gridLayout = new QGridLayout(tabNoteInspector);
        gridLayout->setObjectName("gridLayout");
        positiongroupBox = new QGroupBox(tabNoteInspector);
        positiongroupBox->setObjectName("positiongroupBox");
        verticalLayout = new QVBoxLayout(positiongroupBox);
        verticalLayout->setObjectName("verticalLayout");
        label_11 = new QLabel(positiongroupBox);
        label_11->setObjectName("label_11");

        verticalLayout->addWidget(label_11);

        spinNoteStart = new QDoubleSpinBox(positiongroupBox);
        spinNoteStart->setObjectName("spinNoteStart");

        verticalLayout->addWidget(spinNoteStart);

        label_14 = new QLabel(positiongroupBox);
        label_14->setObjectName("label_14");

        verticalLayout->addWidget(label_14);

        spinNoteDuration = new QDoubleSpinBox(positiongroupBox);
        spinNoteDuration->setObjectName("spinNoteDuration");

        verticalLayout->addWidget(spinNoteDuration);

        label_16 = new QLabel(positiongroupBox);
        label_16->setObjectName("label_16");

        verticalLayout->addWidget(label_16);

        spinNotePitch = new QSpinBox(positiongroupBox);
        spinNotePitch->setObjectName("spinNotePitch");

        verticalLayout->addWidget(spinNotePitch);


        gridLayout->addWidget(positiongroupBox, 0, 0, 1, 1);

        vibratoGroupBox = new QGroupBox(tabNoteInspector);
        vibratoGroupBox->setObjectName("vibratoGroupBox");
        formLayout = new QFormLayout(vibratoGroupBox);
        formLayout->setObjectName("formLayout");
        checkNoteVibrato = new QCheckBox(vibratoGroupBox);
        checkNoteVibrato->setObjectName("checkNoteVibrato");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, checkNoteVibrato);

        comboNoteVibratoPreset = new QComboBox(vibratoGroupBox);
        comboNoteVibratoPreset->setObjectName("comboNoteVibratoPreset");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, comboNoteVibratoPreset);

        btnNoteVibratoEdit = new QPushButton(vibratoGroupBox);
        btnNoteVibratoEdit->setObjectName("btnNoteVibratoEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::SpanningRole, btnNoteVibratoEdit);


        gridLayout->addWidget(vibratoGroupBox, 0, 1, 1, 1);

        physicsGroupBox = new QGroupBox(tabNoteInspector);
        physicsGroupBox->setObjectName("physicsGroupBox");
        horizontalLayout_12 = new QHBoxLayout(physicsGroupBox);
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        label_20 = new QLabel(physicsGroupBox);
        label_20->setObjectName("label_20");

        horizontalLayout_12->addWidget(label_20);

        spinNoteMass = new QDoubleSpinBox(physicsGroupBox);
        spinNoteMass->setObjectName("spinNoteMass");

        horizontalLayout_12->addWidget(spinNoteMass);


        gridLayout->addWidget(physicsGroupBox, 0, 2, 1, 1);

        commentGroupBox = new QGroupBox(tabNoteInspector);
        commentGroupBox->setObjectName("commentGroupBox");
        horizontalLayout_11 = new QHBoxLayout(commentGroupBox);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        editNoteComment = new QLineEdit(commentGroupBox);
        editNoteComment->setObjectName("editNoteComment");

        horizontalLayout_11->addWidget(editNoteComment);


        gridLayout->addWidget(commentGroupBox, 0, 3, 1, 1);

        tabInspector->addTab(tabNoteInspector, QString());
        tabPhraseInspector = new QWidget();
        tabPhraseInspector->setObjectName("tabPhraseInspector");
        horizontalLayout_10 = new QHBoxLayout(tabPhraseInspector);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        groupPhraseIdentity = new QGroupBox(tabPhraseInspector);
        groupPhraseIdentity->setObjectName("groupPhraseIdentity");
        horizontalLayout_8 = new QHBoxLayout(groupPhraseIdentity);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        editPhraseName = new QLineEdit(groupPhraseIdentity);
        editPhraseName->setObjectName("editPhraseName");

        horizontalLayout_8->addWidget(editPhraseName);


        horizontalLayout_10->addWidget(groupPhraseIdentity);

        groupPhraseTranspose = new QGroupBox(tabPhraseInspector);
        groupPhraseTranspose->setObjectName("groupPhraseTranspose");
        formLayout_4 = new QFormLayout(groupPhraseTranspose);
        formLayout_4->setObjectName("formLayout_4");
        label_12 = new QLabel(groupPhraseTranspose);
        label_12->setObjectName("label_12");

        formLayout_4->setWidget(0, QFormLayout::ItemRole::LabelRole, label_12);

        comboPhraseTransposeMode = new QComboBox(groupPhraseTranspose);
        comboPhraseTransposeMode->setObjectName("comboPhraseTransposeMode");

        formLayout_4->setWidget(0, QFormLayout::ItemRole::FieldRole, comboPhraseTransposeMode);

        label_21 = new QLabel(groupPhraseTranspose);
        label_21->setObjectName("label_21");

        formLayout_4->setWidget(1, QFormLayout::ItemRole::LabelRole, label_21);

        spinPhraseTransposeValue = new QSpinBox(groupPhraseTranspose);
        spinPhraseTransposeValue->setObjectName("spinPhraseTransposeValue");

        formLayout_4->setWidget(1, QFormLayout::ItemRole::FieldRole, spinPhraseTransposeValue);


        horizontalLayout_10->addWidget(groupPhraseTranspose);

        groupPhraseVibrato = new QGroupBox(tabPhraseInspector);
        groupPhraseVibrato->setObjectName("groupPhraseVibrato");
        formLayout_5 = new QFormLayout(groupPhraseVibrato);
        formLayout_5->setObjectName("formLayout_5");
        label_22 = new QLabel(groupPhraseVibrato);
        label_22->setObjectName("label_22");

        formLayout_5->setWidget(0, QFormLayout::ItemRole::LabelRole, label_22);

        comboPhraseVibratoPreset = new QComboBox(groupPhraseVibrato);
        comboPhraseVibratoPreset->setObjectName("comboPhraseVibratoPreset");

        formLayout_5->setWidget(0, QFormLayout::ItemRole::FieldRole, comboPhraseVibratoPreset);


        horizontalLayout_10->addWidget(groupPhraseVibrato);

        groupPhraseComment = new QGroupBox(tabPhraseInspector);
        groupPhraseComment->setObjectName("groupPhraseComment");
        horizontalLayout_9 = new QHBoxLayout(groupPhraseComment);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        editPhraseComment = new QLineEdit(groupPhraseComment);
        editPhraseComment->setObjectName("editPhraseComment");

        horizontalLayout_9->addWidget(editPhraseComment);


        horizontalLayout_10->addWidget(groupPhraseComment);

        tabInspector->addTab(tabPhraseInspector, QString());
        tabGestureInspector = new QWidget();
        tabGestureInspector->setObjectName("tabGestureInspector");
        horizontalLayout_15 = new QHBoxLayout(tabGestureInspector);
        horizontalLayout_15->setObjectName("horizontalLayout_15");
        groupGestureIdentity = new QGroupBox(tabGestureInspector);
        groupGestureIdentity->setObjectName("groupGestureIdentity");
        verticalLayout_16 = new QVBoxLayout(groupGestureIdentity);
        verticalLayout_16->setObjectName("verticalLayout_16");
        groupGestureNav = new QGroupBox(groupGestureIdentity);
        groupGestureNav->setObjectName("groupGestureNav");
        groupGestureNav->setMinimumSize(QSize(222, 0));
        groupGestureNav->setMaximumSize(QSize(222, 1000));
        horizontalLayout_7 = new QHBoxLayout(groupGestureNav);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        btnGesturePrev = new QPushButton(groupGestureNav);
        btnGesturePrev->setObjectName("btnGesturePrev");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnGesturePrev->sizePolicy().hasHeightForWidth());
        btnGesturePrev->setSizePolicy(sizePolicy3);

        horizontalLayout_7->addWidget(btnGesturePrev);

        labelGestureIndex = new QLabel(groupGestureNav);
        labelGestureIndex->setObjectName("labelGestureIndex");

        horizontalLayout_7->addWidget(labelGestureIndex);

        btnGestureNext = new QPushButton(groupGestureNav);
        btnGestureNext->setObjectName("btnGestureNext");
        sizePolicy3.setHeightForWidth(btnGestureNext->sizePolicy().hasHeightForWidth());
        btnGestureNext->setSizePolicy(sizePolicy3);

        horizontalLayout_7->addWidget(btnGestureNext);


        verticalLayout_16->addWidget(groupGestureNav);

        label_28 = new QLabel(groupGestureIdentity);
        label_28->setObjectName("label_28");
        label_28->setMinimumSize(QSize(0, 40));
        label_28->setMaximumSize(QSize(16777215, 60));

        verticalLayout_16->addWidget(label_28);

        lineEdit = new QLineEdit(groupGestureIdentity);
        lineEdit->setObjectName("lineEdit");

        verticalLayout_16->addWidget(lineEdit);


        horizontalLayout_15->addWidget(groupGestureIdentity);

        groupGesturePreview = new QGroupBox(tabGestureInspector);
        groupGesturePreview->setObjectName("groupGesturePreview");
        groupGesturePreview->setMinimumSize(QSize(222, 80));
        horizontalLayout_14 = new QHBoxLayout(groupGesturePreview);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        frameGesturePreview = new QFrame(groupGesturePreview);
        frameGesturePreview->setObjectName("frameGesturePreview");
        frameGesturePreview->setMinimumSize(QSize(222, 80));
        frameGesturePreview->setFrameShape(QFrame::Shape::StyledPanel);
        frameGesturePreview->setFrameShadow(QFrame::Shadow::Raised);

        horizontalLayout_14->addWidget(frameGesturePreview);


        horizontalLayout_15->addWidget(groupGesturePreview);

        groupGestureMappings = new QGroupBox(tabGestureInspector);
        groupGestureMappings->setObjectName("groupGestureMappings");
        gridLayout_2 = new QGridLayout(groupGestureMappings);
        gridLayout_2->setObjectName("gridLayout_2");
        label_27 = new QLabel(groupGestureMappings);
        label_27->setObjectName("label_27");

        gridLayout_2->addWidget(label_27, 0, 0, 1, 1);

        comboBox = new QComboBox(groupGestureMappings);
        comboBox->setObjectName("comboBox");

        gridLayout_2->addWidget(comboBox, 0, 1, 1, 1);

        label_13 = new QLabel(groupGestureMappings);
        label_13->setObjectName("label_13");

        gridLayout_2->addWidget(label_13, 0, 2, 1, 1);

        comboBox_6 = new QComboBox(groupGestureMappings);
        comboBox_6->setObjectName("comboBox_6");

        gridLayout_2->addWidget(comboBox_6, 0, 3, 1, 2);

        label_25 = new QLabel(groupGestureMappings);
        label_25->setObjectName("label_25");

        gridLayout_2->addWidget(label_25, 1, 0, 1, 1);

        comboBox_4 = new QComboBox(groupGestureMappings);
        comboBox_4->setObjectName("comboBox_4");

        gridLayout_2->addWidget(comboBox_4, 1, 1, 1, 1);

        label_23 = new QLabel(groupGestureMappings);
        label_23->setObjectName("label_23");

        gridLayout_2->addWidget(label_23, 1, 2, 1, 2);

        comboBox_2 = new QComboBox(groupGestureMappings);
        comboBox_2->setObjectName("comboBox_2");

        gridLayout_2->addWidget(comboBox_2, 1, 4, 1, 1);

        label_26 = new QLabel(groupGestureMappings);
        label_26->setObjectName("label_26");

        gridLayout_2->addWidget(label_26, 2, 0, 1, 1);

        comboBox_5 = new QComboBox(groupGestureMappings);
        comboBox_5->setObjectName("comboBox_5");

        gridLayout_2->addWidget(comboBox_5, 2, 1, 1, 1);

        label_24 = new QLabel(groupGestureMappings);
        label_24->setObjectName("label_24");

        gridLayout_2->addWidget(label_24, 2, 2, 1, 2);

        comboBox_3 = new QComboBox(groupGestureMappings);
        comboBox_3->setObjectName("comboBox_3");

        gridLayout_2->addWidget(comboBox_3, 2, 4, 1, 1);


        horizontalLayout_15->addWidget(groupGestureMappings);

        groupGestureActions = new QGroupBox(tabGestureInspector);
        groupGestureActions->setObjectName("groupGestureActions");
        verticalLayout_12 = new QVBoxLayout(groupGestureActions);
        verticalLayout_12->setObjectName("verticalLayout_12");
        btnGestureSaveToLibrary = new QPushButton(groupGestureActions);
        btnGestureSaveToLibrary->setObjectName("btnGestureSaveToLibrary");

        verticalLayout_12->addWidget(btnGestureSaveToLibrary);

        btnGestureRemove = new QPushButton(groupGestureActions);
        btnGestureRemove->setObjectName("btnGestureRemove");

        verticalLayout_12->addWidget(btnGestureRemove);


        horizontalLayout_15->addWidget(groupGestureActions);

        tabInspector->addTab(tabGestureInspector, QString());

        verticalLayout_6->addWidget(tabInspector);


        verticalLayout_14->addWidget(groupInspector);

        groupTracks = new QGroupBox(compositionTab);
        groupTracks->setObjectName("groupTracks");
        sizePolicy.setHeightForWidth(groupTracks->sizePolicy().hasHeightForWidth());
        groupTracks->setSizePolicy(sizePolicy);
        horizontalLayout_6 = new QHBoxLayout(groupTracks);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        frameTracks = new QFrame(groupTracks);
        frameTracks->setObjectName("frameTracks");
        sizePolicy2.setHeightForWidth(frameTracks->sizePolicy().hasHeightForWidth());
        frameTracks->setSizePolicy(sizePolicy2);
        frameTracks->setMinimumSize(QSize(0, 60));
        frameTracks->setFrameShape(QFrame::Shape::Box);
        frameTracks->setFrameShadow(QFrame::Shadow::Sunken);
        horizontalLayout = new QHBoxLayout(frameTracks);
        horizontalLayout->setObjectName("horizontalLayout");

        horizontalLayout_6->addWidget(frameTracks);


        verticalLayout_14->addWidget(groupTracks);

        groupPenSettings = new QGroupBox(compositionTab);
        groupPenSettings->setObjectName("groupPenSettings");
        verticalLayout_15 = new QVBoxLayout(groupPenSettings);
        verticalLayout_15->setObjectName("verticalLayout_15");
        frameModeLine = new QFrame(groupPenSettings);
        frameModeLine->setObjectName("frameModeLine");
        frameModeLine->setFrameShape(QFrame::Shape::StyledPanel);
        frameModeLine->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(frameModeLine);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        labelMode = new QLabel(frameModeLine);
        labelMode->setObjectName("labelMode");

        horizontalLayout_3->addWidget(labelMode);

        radioDraw = new QRadioButton(frameModeLine);
        radioDraw->setObjectName("radioDraw");
        radioDraw->setChecked(true);

        horizontalLayout_3->addWidget(radioDraw);

        radioSelect = new QRadioButton(frameModeLine);
        radioSelect->setObjectName("radioSelect");

        horizontalLayout_3->addWidget(radioSelect);

        radioEdit = new QRadioButton(frameModeLine);
        radioEdit->setObjectName("radioEdit");

        horizontalLayout_3->addWidget(radioEdit);

        labelDrawType = new QLabel(frameModeLine);
        labelDrawType->setObjectName("labelDrawType");

        horizontalLayout_3->addWidget(labelDrawType);

        radioDrawNotes = new QRadioButton(frameModeLine);
        radioDrawNotes->setObjectName("radioDrawNotes");
        radioDrawNotes->setChecked(false);

        horizontalLayout_3->addWidget(radioDrawNotes);

        radioDrawGesture = new QRadioButton(frameModeLine);
        radioDrawGesture->setObjectName("radioDrawGesture");

        horizontalLayout_3->addWidget(radioDrawGesture);


        verticalLayout_15->addWidget(frameModeLine);

        frameMappingLine = new QFrame(groupPenSettings);
        frameMappingLine->setObjectName("frameMappingLine");
        frameMappingLine->setFrameShape(QFrame::Shape::StyledPanel);
        frameMappingLine->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_4 = new QHBoxLayout(frameMappingLine);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_10 = new QLabel(frameMappingLine);
        label_10->setObjectName("label_10");

        horizontalLayout_4->addWidget(label_10);

        comboMapPressure = new QComboBox(frameMappingLine);
        comboMapPressure->setObjectName("comboMapPressure");

        horizontalLayout_4->addWidget(comboMapPressure);

        label_17 = new QLabel(frameMappingLine);
        label_17->setObjectName("label_17");

        horizontalLayout_4->addWidget(label_17);

        comboMapTiltX = new QComboBox(frameMappingLine);
        comboMapTiltX->setObjectName("comboMapTiltX");

        horizontalLayout_4->addWidget(comboMapTiltX);

        label_18 = new QLabel(frameMappingLine);
        label_18->setObjectName("label_18");

        horizontalLayout_4->addWidget(label_18);

        comboMapTiltY = new QComboBox(frameMappingLine);
        comboMapTiltY->setObjectName("comboMapTiltY");

        horizontalLayout_4->addWidget(comboMapTiltY);

        label_19 = new QLabel(frameMappingLine);
        label_19->setObjectName("label_19");

        horizontalLayout_4->addWidget(label_19);

        comboMapRotation = new QComboBox(frameMappingLine);
        comboMapRotation->setObjectName("comboMapRotation");

        horizontalLayout_4->addWidget(comboMapRotation);


        verticalLayout_15->addWidget(frameMappingLine);


        verticalLayout_14->addWidget(groupPenSettings);

        groupScale = new QGroupBox(compositionTab);
        groupScale->setObjectName("groupScale");
        horizontalLayout_5 = new QHBoxLayout(groupScale);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        labelScaleDisplay = new QLabel(groupScale);
        labelScaleDisplay->setObjectName("labelScaleDisplay");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(labelScaleDisplay->sizePolicy().hasHeightForWidth());
        labelScaleDisplay->setSizePolicy(sizePolicy4);

        horizontalLayout_5->addWidget(labelScaleDisplay);

        btnScaleEdit = new QPushButton(groupScale);
        btnScaleEdit->setObjectName("btnScaleEdit");
        sizePolicy3.setHeightForWidth(btnScaleEdit->sizePolicy().hasHeightForWidth());
        btnScaleEdit->setSizePolicy(sizePolicy3);

        horizontalLayout_5->addWidget(btnScaleEdit);


        verticalLayout_14->addWidget(groupScale);

        MainTab->addTab(compositionTab, QString());
        preferencesTab = new QWidget();
        preferencesTab->setObjectName("preferencesTab");
        verticalLayout_13 = new QVBoxLayout(preferencesTab);
        verticalLayout_13->setObjectName("verticalLayout_13");
        label_15 = new QLabel(preferencesTab);
        label_15->setObjectName("label_15");

        verticalLayout_13->addWidget(label_15);

        MainTab->addTab(preferencesTab, QString());

        verticalLayout_4->addWidget(MainTab);

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

        MainTab->setCurrentIndex(1);
        tabInspector->setCurrentIndex(2);


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
        groupContainerInspector->setTitle(QCoreApplication::translate("CalamusMain", "Container inspector", nullptr));
        labelContainerType->setText(QCoreApplication::translate("CalamusMain", "TextLabel", nullptr));
        label_4->setText(QCoreApplication::translate("CalamusMain", "Type: ", nullptr));
        label_5->setText(QCoreApplication::translate("CalamusMain", "Instance: ", nullptr));
        groupOutputs->setTitle(QCoreApplication::translate("CalamusMain", "Outputs", nullptr));
        groupInputs->setTitle(QCoreApplication::translate("CalamusMain", "Inputs", nullptr));
        groupConfig->setTitle(QCoreApplication::translate("CalamusMain", "Config", nullptr));
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
        MainTab->setTabText(MainTab->indexOf(soundEngineTab), QCoreApplication::translate("CalamusMain", "Sound Engine", nullptr));
        groupPhraseSelector->setTitle(QCoreApplication::translate("CalamusMain", "Phrase", nullptr));
        btnPhraseLoad->setText(QCoreApplication::translate("CalamusMain", "Load", nullptr));
        btnPhraseSave->setText(QCoreApplication::translate("CalamusMain", "Save", nullptr));
        groupInspector->setTitle(QCoreApplication::translate("CalamusMain", "Inspector", nullptr));
        positiongroupBox->setTitle(QCoreApplication::translate("CalamusMain", "Position", nullptr));
        label_11->setText(QCoreApplication::translate("CalamusMain", "Start:", nullptr));
        label_14->setText(QCoreApplication::translate("CalamusMain", "Duration:", nullptr));
        label_16->setText(QCoreApplication::translate("CalamusMain", "Pitch:", nullptr));
        vibratoGroupBox->setTitle(QCoreApplication::translate("CalamusMain", "Vibrato", nullptr));
        checkNoteVibrato->setText(QCoreApplication::translate("CalamusMain", "Vibrato", nullptr));
        btnNoteVibratoEdit->setText(QCoreApplication::translate("CalamusMain", "Edit", nullptr));
        physicsGroupBox->setTitle(QCoreApplication::translate("CalamusMain", "Physics", nullptr));
        label_20->setText(QCoreApplication::translate("CalamusMain", "Mass:", nullptr));
        commentGroupBox->setTitle(QCoreApplication::translate("CalamusMain", "Comment:", nullptr));
        tabInspector->setTabText(tabInspector->indexOf(tabNoteInspector), QCoreApplication::translate("CalamusMain", "Note", nullptr));
        groupPhraseIdentity->setTitle(QCoreApplication::translate("CalamusMain", "Name:", nullptr));
        groupPhraseTranspose->setTitle(QCoreApplication::translate("CalamusMain", "Transpose", nullptr));
        label_12->setText(QCoreApplication::translate("CalamusMain", "Mode:", nullptr));
        label_21->setText(QCoreApplication::translate("CalamusMain", "Value", nullptr));
        groupPhraseVibrato->setTitle(QCoreApplication::translate("CalamusMain", "Vibrato", nullptr));
        label_22->setText(QCoreApplication::translate("CalamusMain", "Preset:", nullptr));
        groupPhraseComment->setTitle(QCoreApplication::translate("CalamusMain", "Comment:", nullptr));
        tabInspector->setTabText(tabInspector->indexOf(tabPhraseInspector), QCoreApplication::translate("CalamusMain", "Phrase", nullptr));
        groupGestureIdentity->setTitle(QCoreApplication::translate("CalamusMain", "name", nullptr));
        groupGestureNav->setTitle(QString());
        btnGesturePrev->setText(QCoreApplication::translate("CalamusMain", "\342\227\204", nullptr));
        labelGestureIndex->setText(QCoreApplication::translate("CalamusMain", "1 of 1", nullptr));
        btnGestureNext->setText(QCoreApplication::translate("CalamusMain", "\342\226\272", nullptr));
        label_28->setText(QCoreApplication::translate("CalamusMain", "Name:", nullptr));
        groupGesturePreview->setTitle(QCoreApplication::translate("CalamusMain", "Preview", nullptr));
        groupGestureMappings->setTitle(QCoreApplication::translate("CalamusMain", "Mappings", nullptr));
        label_27->setText(QCoreApplication::translate("CalamusMain", "X->", nullptr));
        label_13->setText(QCoreApplication::translate("CalamusMain", "Y->", nullptr));
        label_25->setText(QCoreApplication::translate("CalamusMain", "Pressure ->", nullptr));
        label_23->setText(QCoreApplication::translate("CalamusMain", "Tilt X ->", nullptr));
        label_26->setText(QCoreApplication::translate("CalamusMain", "Rotation ->", nullptr));
        label_24->setText(QCoreApplication::translate("CalamusMain", "Tilt Y ->", nullptr));
        groupGestureActions->setTitle(QString());
        btnGestureSaveToLibrary->setText(QCoreApplication::translate("CalamusMain", "Save to Library", nullptr));
        btnGestureRemove->setText(QCoreApplication::translate("CalamusMain", "Remove", nullptr));
        tabInspector->setTabText(tabInspector->indexOf(tabGestureInspector), QCoreApplication::translate("CalamusMain", "Gesture", nullptr));
        groupTracks->setTitle(QCoreApplication::translate("CalamusMain", "Tracks", nullptr));
        groupPenSettings->setTitle(QCoreApplication::translate("CalamusMain", "Pen Settings", nullptr));
        labelMode->setText(QCoreApplication::translate("CalamusMain", "Mode:", nullptr));
        radioDraw->setText(QCoreApplication::translate("CalamusMain", "Draw", nullptr));
        radioSelect->setText(QCoreApplication::translate("CalamusMain", "Select", nullptr));
        radioEdit->setText(QCoreApplication::translate("CalamusMain", "Edit", nullptr));
        labelDrawType->setText(QCoreApplication::translate("CalamusMain", "Draw:", nullptr));
        radioDrawNotes->setText(QCoreApplication::translate("CalamusMain", "Notes", nullptr));
        radioDrawGesture->setText(QCoreApplication::translate("CalamusMain", "Gesture", nullptr));
        label_10->setText(QCoreApplication::translate("CalamusMain", "Pressure\342\206\222", nullptr));
        label_17->setText(QCoreApplication::translate("CalamusMain", "Tilt X\342\206\222", nullptr));
        label_18->setText(QCoreApplication::translate("CalamusMain", "Tilt Y\342\206\222", nullptr));
        label_19->setText(QCoreApplication::translate("CalamusMain", "Rotation\342\206\222", nullptr));
        groupScale->setTitle(QCoreApplication::translate("CalamusMain", "Scale", nullptr));
        labelScaleDisplay->setText(QCoreApplication::translate("CalamusMain", "1 \302\267 Just Intonation \302\267 220 Hz", nullptr));
        btnScaleEdit->setText(QCoreApplication::translate("CalamusMain", "Edit", nullptr));
        MainTab->setTabText(MainTab->indexOf(compositionTab), QCoreApplication::translate("CalamusMain", "Composition", nullptr));
        label_15->setText(QCoreApplication::translate("CalamusMain", "TextLabel", nullptr));
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
