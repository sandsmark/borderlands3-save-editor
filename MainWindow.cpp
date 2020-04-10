#include "MainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpacerItem>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::Dialog);


    QHBoxLayout *basicsLayout = new QHBoxLayout;

    m_nameEdit = new QLineEdit;
    basicsLayout->addWidget(new QLabel("Name:"));
    basicsLayout->addWidget(m_nameEdit);
    basicsLayout->addSpacerItem(new QSpacerItem(0, 0));


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(basicsLayout);
    setLayout(mainLayout);

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
}

