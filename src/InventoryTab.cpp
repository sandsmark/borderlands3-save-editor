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
    QHBoxLayout *mainLayout = new QHBoxLayout;
    setLayout(mainLayout);

    m_list = new QListWidget;
    layout()->addWidget(m_list);

    m_partsList = new QListWidget;
    layout()->addWidget(m_partsList);

    connect(savegame, &Savegame::itemsChanged, this, &InventoryTab::load);
    connect(m_list, &QListWidget::itemSelectionChanged, this, &InventoryTab::onSelected);
}

void InventoryTab::onSelected()
{
    QList<QListWidgetItem*> selected = m_list->selectedItems();
    if (selected.isEmpty()) {
        qDebug() << "No selected"; // todo disable button when none selected
        return;
    }
    int index = m_list->row(selected.first());
    if (index >= m_savegame->items().count()) {
        qWarning() << "Out of bounds!";
        return;
    }

    const Savegame::Item &item = m_savegame->items()[index];
    qDebug() << "Part count" << item.numberOfParts;
    qDebug() << "Version" << item.version << "level" << item.level;
    qDebug() << item.balance.val;
    qDebug() << item.data.val;
    qDebug() << item.manufacturer.val;
}

void InventoryTab::load()
{
    m_list->clear();
    for (const Savegame::Item &item : m_savegame->items()) {
        m_list->addItem(item.name);
    }
}
