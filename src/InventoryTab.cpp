#include "InventoryTab.h"
#include "Savegame.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

InventoryTab::InventoryTab(Savegame *savegame, QWidget *parent) : QWidget(parent),
  m_savegame(savegame)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_list = new QListWidget;
    layout()->addWidget(m_list);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setMargin(0);
    mainLayout->addLayout(buttonsLayout);

    QPushButton *editButton = new QPushButton(tr("Edit"));
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addStretch();
}

void InventoryTab::onEdit()
{
    QList<QListWidgetItem*> selected = m_list->selectedItems();
    if (selected.isEmpty()) {
        qDebug() << "No selected"; // todo disable button when none selected
        return;
    }

    QString itemId = selected.first()->data(Qt::UserRole).toString();
}

void InventoryTab::load()
{
//    m_saveGame->in

}
