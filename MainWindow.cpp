#include "MainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpacerItem>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::Dialog);


    QHBoxLayout *basicsLayout = new QHBoxLayout;

    m_nameEdit = new QLineEdit;
    m_nameEdit->setEnabled(false);
    basicsLayout->addWidget(new QLabel("Name:"));
    basicsLayout->addWidget(m_nameEdit);

    basicsLayout->addStretch();

    m_levelEdit = new QSpinBox;
    basicsLayout->addWidget(new QLabel("Level:"));
    basicsLayout->addWidget(m_levelEdit);

    QHBoxLayout *saveLoadLayout = new QHBoxLayout;
    QPushButton *loadButton = new QPushButton("&Load");
    saveLoadLayout->addWidget(loadButton);
    basicsLayout->addStretch();
    m_saveButton = new QPushButton("&Save");
    m_saveButton->setEnabled(false);
    saveLoadLayout->addWidget(m_saveButton);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(basicsLayout);

    mainLayout->addStretch();
    mainLayout->addLayout(saveLoadLayout);
    setLayout(mainLayout);

    connect(loadButton, &QPushButton::clicked, this, &Widget::openFile);
    connect(m_saveButton, &QPushButton::clicked, this, &Widget::saveFile);

    resize(600, 500);

    QMetaObject::invokeMethod(this, "openFile", Qt::QueuedConnection);
}

Widget::~Widget()
{
}

void Widget::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Select a savegame", QString(), "Savefile (*.sav)");
    if (path.isEmpty()) {
        return;
    }
    if (!m_savegame.load(path)) {
        return;
    }

    m_nameEdit->setText(m_savegame.characterName());
    m_saveButton->setEnabled(true);
    m_nameEdit->setEnabled(true);
}

void Widget::saveFile()
{
    // todo

}

