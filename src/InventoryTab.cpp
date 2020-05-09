#include "InventoryTab.h"
#include "Savegame.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QLabel>

InventoryTab::InventoryTab(Savegame *savegame, QWidget *parent) : QWidget(parent),
  m_savegame(savegame)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    setLayout(mainLayout);

    m_list = new QListWidget;
    layout()->addWidget(m_list);

    m_partsList = new QListWidget;
    layout()->addWidget(m_partsList);

    m_partName = new QLabel;
    m_partName->setWordWrap(true);
    m_partEffects = new QLabel;
    m_partEffects->setWordWrap(true);
    m_partNegatives = new QLabel;
    m_partNegatives->setWordWrap(true);
    m_partPositives = new QLabel;
    m_partPositives->setWordWrap(true);

    QVBoxLayout *partInfoLayout = new QVBoxLayout;
    partInfoLayout->addWidget(new QLabel(tr("<h3>Item part details</h3>")));
    partInfoLayout->addWidget(m_partName);
    partInfoLayout->addWidget(new QLabel(tr("<b>Description</b>")));
    partInfoLayout->addWidget(m_partEffects);
    partInfoLayout->addWidget(new QLabel(tr("<b>Negatives</b>")));
    partInfoLayout->addWidget(m_partNegatives);
    partInfoLayout->addWidget(new QLabel(tr("<b>Positives</b>")));
    partInfoLayout->addWidget(m_partPositives);
    partInfoLayout->addStretch();

    QLabel *docsLink = new QLabel("<a href=https://docs.google.com/spreadsheets/d/16b7bGPFKIrNg_cJm_WCMO6cKahexBs7BiJ6ja0RlD04/edit>Data source</a>");
    docsLink->setOpenExternalLinks(true);
    partInfoLayout->addWidget(docsLink);

    mainLayout->addLayout(partInfoLayout);

    connect(savegame, &Savegame::itemsChanged, this, &InventoryTab::load);
    connect(m_list, &QListWidget::itemSelectionChanged, this, &InventoryTab::onItemSelected);
    connect(m_partsList, &QListWidget::itemSelectionChanged, this, &InventoryTab::onPartSelected);
}

void InventoryTab::onItemSelected()
{
    m_partName->setText({});
    m_partEffects->setText({});
    m_partNegatives->setText({});
    m_partPositives->setText({});

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
    for (const ItemPart &part : m_savegame->itemData().weaponParts(item.objectShortName)) {
        partCategories[part.partId] = part.category;
    }

    QStringList nameText, effectsText, negativesText, positivesText;

    const QString assetId = item.data.val.split('.').last();
    if (m_savegame->itemData().hasItemInfo(assetId)) {
        const ItemInfo &info = m_savegame->itemData().itemInfo(assetId);
        if (!info.inventoryName.isEmpty()) {
            nameText.append(info.inventoryName);
            qDebug() << "Inventory name" << info.inventoryName;
        }
        if (!info.canDropOrSell) {
            effectsText.append(" • Can't be dropped or sold");
        }
        if (info.inventorySize > 1) {
            effectsText.append(QString::fromUtf8(" • Inventory size %1").arg(info.inventorySize));
        } else if (info.inventorySize == 0) {
            effectsText.append(" • Takes no space in inventory");
        }
        if (info.monetaryValue > 1) {
            effectsText.append(QString::fromUtf8(" • Monetary value %1").arg(info.monetaryValue));
        }
    } else {
        qWarning() << "Missing info for asset" << assetId;
    }


    for (const Savegame::Item::Aspect &part : item.parts) {
        QString name = part.val.split('.').last();
        if (partCategories.contains(name)) {
            name = partCategories[name] + " " + name;
        } else {
            qWarning() << item.name << item.objectShortName << "has part" << name << "which is not in the usual list";
            name = m_savegame->itemData().weaponPartType(name) + " " + name;
        }
        QListWidgetItem *listItem = new QListWidgetItem(name);
        listItem->setData(Qt::UserRole, part.val.split('.').last());
//        qDebug() << part.val;
        m_partsList->addItem(listItem);

        const ItemDescription description = m_savegame->itemData().itemDescription(part.val.split('.').last());
        if (!description.naming.isEmpty()) {
            nameText.append(" • " + description.naming);
        }
        if (!description.effects.isEmpty()) {
            effectsText.append(" • " + description.effects);
        }
        if (!description.negatives.isEmpty()) {
            negativesText.append("• " + description.negatives);
        }
        if (!description.positives.isEmpty()) {
            positivesText.append("• " + description.positives);
        }
    }
    positivesText.removeAll("• DO NOT REMOVE"); // I'm very, very lazy
    positivesText.removeAll("• -");

    m_partName->setText(nameText.join('\n'));
    m_partEffects->setText(effectsText.join('\n'));
    m_partNegatives->setText(negativesText.join('\n'));
    m_partPositives->setText(positivesText.join('\n'));

//    qDebug() << "Part count" << item.numberOfParts;
//    qDebug() << "Version" << item.version << "level" << item.level;
//    qDebug() << item.balance.val;
//    qDebug() << item.data.val;
//    qDebug() << item.manufacturer.val;
//    qDebug() << item.data.val;
//    qDebug() << item.balance.val;
}

void InventoryTab::onPartSelected()
{
    QList<QListWidgetItem*> selected = m_partsList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    QString itemId = selected.first()->data(Qt::UserRole).toString();
    if (itemId.isEmpty()) {
        return;
    }
    const ItemDescription description = m_savegame->itemData().itemDescription(itemId);
    m_partName->setText(description.naming);
    m_partEffects->setText(description.effects);
    m_partNegatives->setText(description.negatives);
    m_partPositives->setText(description.positives);

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
