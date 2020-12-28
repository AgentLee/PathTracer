/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <mygl.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionLoad_Scene;
    QAction *actionRender;
    QAction *actionCamera_Controls;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    MyGL *mygl;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QSpinBox *spinBox_maxPrims;
    QComboBox *integratorSpinBox;
    QLabel *label_3;
    QLabel *label;
    QSpinBox *samplesSpinBox;
    QCheckBox *checkBox_BVH;
    QCheckBox *checkBox_Progressive;
    QSpinBox *recursionSpinBox;
    QLabel *label_2;
    QCheckBox *checkBox_KDT;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(655, 681);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionLoad_Scene = new QAction(MainWindow);
        actionLoad_Scene->setObjectName(QString::fromUtf8("actionLoad_Scene"));
        actionRender = new QAction(MainWindow);
        actionRender->setObjectName(QString::fromUtf8("actionRender"));
        actionCamera_Controls = new QAction(MainWindow);
        actionCamera_Controls->setObjectName(QString::fromUtf8("actionCamera_Controls"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mygl = new MyGL(centralWidget);
        mygl->setObjectName(QString::fromUtf8("mygl"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(mygl->sizePolicy().hasHeightForWidth());
        mygl->setSizePolicy(sizePolicy);
        mygl->setMinimumSize(QSize(618, 432));

        verticalLayout->addWidget(mygl);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setMinimumSize(QSize(631, 121));
        groupBox->setBaseSize(QSize(631, 121));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 20, 611, 91));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 3, 1, 1);

        spinBox_maxPrims = new QSpinBox(layoutWidget);
        spinBox_maxPrims->setObjectName(QString::fromUtf8("spinBox_maxPrims"));
        spinBox_maxPrims->setMinimum(1);
        spinBox_maxPrims->setMaximum(255);

        gridLayout->addWidget(spinBox_maxPrims, 1, 4, 1, 1);

        integratorSpinBox = new QComboBox(layoutWidget);
        integratorSpinBox->addItem(QString());
        integratorSpinBox->addItem(QString());
        integratorSpinBox->addItem(QString());
        integratorSpinBox->addItem(QString());
        integratorSpinBox->addItem(QString());
        integratorSpinBox->addItem(QString());
        integratorSpinBox->setObjectName(QString::fromUtf8("integratorSpinBox"));

        gridLayout->addWidget(integratorSpinBox, 1, 1, 1, 2);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setWordWrap(true);

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        samplesSpinBox = new QSpinBox(layoutWidget);
        samplesSpinBox->setObjectName(QString::fromUtf8("samplesSpinBox"));
        samplesSpinBox->setMinimum(1);
        samplesSpinBox->setMaximum(100);
        samplesSpinBox->setValue(10);

        gridLayout->addWidget(samplesSpinBox, 0, 2, 1, 1);

        checkBox_BVH = new QCheckBox(layoutWidget);
        checkBox_BVH->setObjectName(QString::fromUtf8("checkBox_BVH"));
        checkBox_BVH->setChecked(true);

        gridLayout->addWidget(checkBox_BVH, 1, 5, 1, 1);

        checkBox_Progressive = new QCheckBox(layoutWidget);
        checkBox_Progressive->setObjectName(QString::fromUtf8("checkBox_Progressive"));
        checkBox_Progressive->setChecked(true);

        gridLayout->addWidget(checkBox_Progressive, 0, 5, 1, 1);

        recursionSpinBox = new QSpinBox(layoutWidget);
        recursionSpinBox->setObjectName(QString::fromUtf8("recursionSpinBox"));
        recursionSpinBox->setMinimum(1);
        recursionSpinBox->setMaximum(100);
        recursionSpinBox->setValue(5);

        gridLayout->addWidget(recursionSpinBox, 0, 4, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setWordWrap(true);

        gridLayout->addWidget(label_2, 0, 3, 1, 1);

        checkBox_KDT = new QCheckBox(layoutWidget);
        checkBox_KDT->setObjectName(QString::fromUtf8("checkBox_KDT"));
        checkBox_KDT->setChecked(false);

        gridLayout->addWidget(checkBox_KDT, 2, 5, 1, 1);


        verticalLayout->addWidget(groupBox);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 655, 20));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionRender);
        menuFile->addAction(actionLoad_Scene);
        menuFile->addAction(actionQuit);
        menuHelp->addAction(actionCamera_Controls);

        retranslateUi(MainWindow);
        QObject::connect(mygl, SIGNAL(sig_ResizeToCamera(int,int)), MainWindow, SLOT(slot_ResizeToCamera(int,int)));
        QObject::connect(samplesSpinBox, SIGNAL(valueChanged(int)), mygl, SLOT(slot_SetNumSamplesSqrt(int)));
        QObject::connect(recursionSpinBox, SIGNAL(valueChanged(int)), mygl, SLOT(slot_SetRecursionLimit(int)));
        QObject::connect(checkBox_Progressive, SIGNAL(toggled(bool)), mygl, SLOT(slot_SetProgressiveRender(bool)));
        QObject::connect(integratorSpinBox, SIGNAL(currentIndexChanged(int)), mygl, SLOT(slot_SetIntegratorType(int)));
        QObject::connect(mygl, SIGNAL(sig_DisableGUI(bool)), MainWindow, SLOT(slot_DisableGUI(bool)));
        QObject::connect(checkBox_BVH, SIGNAL(toggled(bool)), mygl, SLOT(slot_UseBVH(bool)));
        QObject::connect(spinBox_maxPrims, SIGNAL(valueChanged(int)), mygl, SLOT(slot_SetMaxBVHPrims(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Path Tracer", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
#if QT_CONFIG(shortcut)
        actionQuit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLoad_Scene->setText(QCoreApplication::translate("MainWindow", "Load Scene", nullptr));
#if QT_CONFIG(shortcut)
        actionLoad_Scene->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRender->setText(QCoreApplication::translate("MainWindow", "Render", nullptr));
#if QT_CONFIG(shortcut)
        actionRender->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCamera_Controls->setText(QCoreApplication::translate("MainWindow", "Camera Controls", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Controls", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Max primitives in node:", nullptr));
        integratorSpinBox->setItemText(0, QCoreApplication::translate("MainWindow", "Naive", nullptr));
        integratorSpinBox->setItemText(1, QCoreApplication::translate("MainWindow", "Direct Lighting", nullptr));
        integratorSpinBox->setItemText(2, QCoreApplication::translate("MainWindow", "Indirect Lighting", nullptr));
        integratorSpinBox->setItemText(3, QCoreApplication::translate("MainWindow", "Full Lighting", nullptr));
        integratorSpinBox->setItemText(4, QCoreApplication::translate("MainWindow", "Volumetric", nullptr));
        integratorSpinBox->setItemText(5, QCoreApplication::translate("MainWindow", "Photon Mapping", nullptr));

        label_3->setText(QCoreApplication::translate("MainWindow", "Integrator type:", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Square root of pixel samples count:", nullptr));
        checkBox_BVH->setText(QCoreApplication::translate("MainWindow", "Make BVH", nullptr));
        checkBox_Progressive->setText(QCoreApplication::translate("MainWindow", "Progressive Preview", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Recursion limit:", nullptr));
        checkBox_KDT->setText(QCoreApplication::translate("MainWindow", "Make kd-Tree", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
