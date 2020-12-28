/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
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
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(655, 681);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionLoad_Scene = new QAction(MainWindow);
        actionLoad_Scene->setObjectName(QStringLiteral("actionLoad_Scene"));
        actionRender = new QAction(MainWindow);
        actionRender->setObjectName(QStringLiteral("actionRender"));
        actionCamera_Controls = new QAction(MainWindow);
        actionCamera_Controls->setObjectName(QStringLiteral("actionCamera_Controls"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mygl = new MyGL(centralWidget);
        mygl->setObjectName(QStringLiteral("mygl"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(mygl->sizePolicy().hasHeightForWidth());
        mygl->setSizePolicy(sizePolicy);
        mygl->setMinimumSize(QSize(618, 432));

        verticalLayout->addWidget(mygl);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setMinimumSize(QSize(631, 121));
        groupBox->setBaseSize(QSize(631, 121));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 20, 611, 91));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 1, 3, 1, 1);

        spinBox_maxPrims = new QSpinBox(layoutWidget);
        spinBox_maxPrims->setObjectName(QStringLiteral("spinBox_maxPrims"));
        spinBox_maxPrims->setMinimum(1);
        spinBox_maxPrims->setMaximum(255);

        gridLayout->addWidget(spinBox_maxPrims, 1, 4, 1, 1);

        integratorSpinBox = new QComboBox(layoutWidget);
        integratorSpinBox->setObjectName(QStringLiteral("integratorSpinBox"));

        gridLayout->addWidget(integratorSpinBox, 1, 1, 1, 2);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setWordWrap(true);

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setWordWrap(true);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        samplesSpinBox = new QSpinBox(layoutWidget);
        samplesSpinBox->setObjectName(QStringLiteral("samplesSpinBox"));
        samplesSpinBox->setMinimum(1);
        samplesSpinBox->setMaximum(100);
        samplesSpinBox->setValue(10);

        gridLayout->addWidget(samplesSpinBox, 0, 2, 1, 1);

        checkBox_BVH = new QCheckBox(layoutWidget);
        checkBox_BVH->setObjectName(QStringLiteral("checkBox_BVH"));
        checkBox_BVH->setChecked(true);

        gridLayout->addWidget(checkBox_BVH, 1, 5, 1, 1);

        checkBox_Progressive = new QCheckBox(layoutWidget);
        checkBox_Progressive->setObjectName(QStringLiteral("checkBox_Progressive"));
        checkBox_Progressive->setChecked(true);

        gridLayout->addWidget(checkBox_Progressive, 0, 5, 1, 1);

        recursionSpinBox = new QSpinBox(layoutWidget);
        recursionSpinBox->setObjectName(QStringLiteral("recursionSpinBox"));
        recursionSpinBox->setMinimum(1);
        recursionSpinBox->setMaximum(100);
        recursionSpinBox->setValue(5);

        gridLayout->addWidget(recursionSpinBox, 0, 4, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setWordWrap(true);

        gridLayout->addWidget(label_2, 0, 3, 1, 1);

        checkBox_KDT = new QCheckBox(layoutWidget);
        checkBox_KDT->setObjectName(QStringLiteral("checkBox_KDT"));
        checkBox_KDT->setChecked(false);

        gridLayout->addWidget(checkBox_KDT, 2, 5, 1, 1);


        verticalLayout->addWidget(groupBox);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 655, 20));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Path Tracer", Q_NULLPTR));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionLoad_Scene->setText(QApplication::translate("MainWindow", "Load Scene", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        actionLoad_Scene->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionRender->setText(QApplication::translate("MainWindow", "Render", Q_NULLPTR));
#ifndef QT_NO_SHORTCUT
        actionRender->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionCamera_Controls->setText(QApplication::translate("MainWindow", "Camera Controls", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "Controls", Q_NULLPTR));
        label_4->setText(QApplication::translate("MainWindow", "Max primitives in node:", Q_NULLPTR));
        integratorSpinBox->clear();
        integratorSpinBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Naive", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Direct Lighting", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Indirect Lighting", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Full Lighting", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Volumetric", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Photon Mapping", Q_NULLPTR)
        );
        label_3->setText(QApplication::translate("MainWindow", "Integrator type:", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Square root of pixel samples count:", Q_NULLPTR));
        checkBox_BVH->setText(QApplication::translate("MainWindow", "Make BVH", Q_NULLPTR));
        checkBox_Progressive->setText(QApplication::translate("MainWindow", "Progressive Preview", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Recursion limit:", Q_NULLPTR));
        checkBox_KDT->setText(QApplication::translate("MainWindow", "Make kd-Tree", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
