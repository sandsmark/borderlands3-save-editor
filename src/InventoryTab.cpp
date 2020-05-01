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
        return;
    }
    int index = m_list->row(selected.first());
    if (index >= m_savegame->items().count()) {
        qWarning() << "Out of bounds!";
        return;
    }
    // TODO actually modify
    // Can't be arsed to find out exactly how
    // TODO: maybe list of dropdowns? idk


    m_partsList->clear();
    const Savegame::Item &item = m_savegame->items()[index];
    QStringList parts;

    QMap<QString, QString> partCategories;
    for (const WeaponPart &part : m_savegame->itemData().weaponParts(item.objectShortName)) {
        partCategories[part.partId] = part.category;
    }

    for (const Savegame::Item::Aspect &part : item.parts) {
        QString name = part.val.split('.').last();
        if (partCategories.contains(name)) {
            name = partCategories[name] + " " + name;
        } else {
            qWarning() << item.name << item.objectShortName << "has part" << name << "which is not in the usual list";
            name = m_savegame->itemData().weaponPartType(name) + " " + name;
        }
        parts.append(name);
//        m_partsList->addItem(part.val.split('.').last());
    }
    std::sort(parts.begin(), parts.end());
    m_partsList->addItems(parts);
//    qDebug() << "Part count" << item.numberOfParts;
//    qDebug() << "Version" << item.version << "level" << item.level;
//    qDebug() << item.balance.val;
//    qDebug() << item.data.val;
//    qDebug() << item.manufacturer.val;
}

void InventoryTab::load()
{
    m_list->clear();
    for (const Savegame::Item &item : m_savegame->items()) {
        QString rarity = item.objectShortName.split('_').last();
        m_list->addItem(tr("%1 (level %2)").arg(item.name, QString::number(item.level)));
//        m_list->addItem(tr("%1%2 (level %3)").arg(rarity + " ", item.name, QString::number(item.level)));
    }
}
