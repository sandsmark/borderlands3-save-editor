#include "InventoryTab.h"
#include "Savegame.h"
#include <QListWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>

InventoryTab::InventoryTab(Savegame *savegame, QWidget *parent) : QWidget(parent),
  m_savegame(savegame)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    setLayout(mainLayout);

    m_list = new QListWidget;
    layout()->addWidget(m_list);

    m_partsList = new QTreeWidget;
    m_partsList->setHeaderHidden(true);
    layout()->addWidget(m_partsList);

    m_partName = new QLabel;
    m_partName->setWordWrap(true);
    m_partEffects = new QLabel;
    m_partEffects->setWordWrap(true);
    m_partNegatives = new QLabel;
    m_partNegatives->setWordWrap(true);
    m_partPositives = new QLabel;
    m_partPositives->setWordWrap(true);

    QWidget *infoWidget = new QWidget;
    QVBoxLayout *partInfoLayout = new QVBoxLayout(infoWidget);
    m_warningText = new QLabel;
    partInfoLayout->addWidget(m_warningText);

    partInfoLayout->addWidget(new QLabel(tr("<h3>Item part details</h3>")));
    partInfoLayout->addWidget(m_partName);
    partInfoLayout->addWidget(new QLabel(tr("<b>Description</b>")));
    partInfoLayout->addWidget(m_partEffects);
    partInfoLayout->addWidget(new QLabel(tr("<b>Negatives</b>")));
    partInfoLayout->addWidget(m_partNegatives);
    partInfoLayout->addWidget(new QLabel(tr("<b>Positives</b>")));
    partInfoLayout->addWidget(m_partPositives);
    partInfoLayout->addStretch();
//    infoWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    infoWidget->setFixedWidth(300);

    QLabel *docsLink = new QLabel("<a href=https://docs.google.com/spreadsheets/d/16b7bGPFKIrNg_cJm_WCMO6cKahexBs7BiJ6ja0RlD04/edit>Data source</a>");
    docsLink->setOpenExternalLinks(true);
    partInfoLayout->addWidget(docsLink);

    mainLayout->addWidget(infoWidget);

    connect(savegame, &Savegame::itemsChanged, this, &InventoryTab::load);
    connect(m_list, &QListWidget::itemSelectionChanged, this, &InventoryTab::onItemSelected);
    connect(m_partsList, &QTreeWidget::itemSelectionChanged, this, &InventoryTab::onPartSelected);
    connect(m_partsList, &QTreeWidget::itemChanged, this, &InventoryTab::onPartChanged);
}

static QString makeNamePretty(const QString &name)
{
    QString displayName = name.split('.').last();
    displayName.replace("_AR_", "_Assault Rifle_");
    displayName.replace("_SR_", "_Sniper Rifle_");
    displayName.replace("_SM_", "_SMG_");
    displayName.replace("_SG_", "_Shotgun_");
    displayName.replace("_GM_", "_Grenade Mod_");
    displayName.replace("_MAL_", "_Maliwan_");
    displayName.replace("_DAL_", "_Dahl_");
    displayName.replace("_Hyp_", "_Hyperion_");
    displayName.replace("_HYP_", "_Hyperion_");
    displayName.replace("_TED_", "_Tediore_");
    displayName.replace("_VLA_", "_Vladof_");
    QStringList nameParts = displayName.split('_');
    if (nameParts.count() >= 3) {
        //            displayName = nameParts.mid(1).join(' ');
        if (nameParts.first() == "Part") {
            nameParts.takeFirst();
        }
        nameParts.replaceInStrings("SR", "Sniper Rifle");
    } else {
        qWarning() << "Weird name" << name;
    }
    displayName = nameParts.join(' ');

    return displayName;
}

void InventoryTab::onItemSelected()
{
    QSignalBlocker listSignalBlocker(m_partsList);
    m_partsList->clear();
    m_enabledParts.clear();

    m_partName->setText({});
    m_partEffects->setText({});
    m_partNegatives->setText({});
    m_partPositives->setText({});

    QList<QListWidgetItem*> selected = m_list->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    m_selectedInventoryItem = m_list->row(selected.first());
    if (m_selectedInventoryItem >= m_savegame->inventoryItemsCount()) {
        qWarning() << "Out of bounds!";
        return;
    }
    // TODO actually modify
    // Can't be arsed to find out exactly how
    // TODO: maybe list of dropdowns? idk


    const InventoryItem &currentInventoryItem = m_savegame->inventoryItem(m_selectedInventoryItem);
    QStringList parts;

    QMap<QString, QString> partCategories;
    QSet<QString> categories;
    for (const ItemPart &part : ItemData::weaponParts(currentInventoryItem.objectShortName)) {
        partCategories[part.partId] = part.category;
        categories.insert(part.category);
    }


    QHash<QString, QTreeWidgetItem*> categoryItems;
    for (const QString &category : categories) {
        QTreeWidgetItem *item = new QTreeWidgetItem({category});
        m_partsList->addTopLevelItem(item);
        categoryItems[category] = item;
    }

    QStringList nameText, effectsText, negativesText, positivesText;

    const QString assetId = currentInventoryItem.data.val.split('.').last();
    if (ItemData::hasItemInfo(assetId)) {
        const ItemInfo &info = ItemData::itemInfo(assetId);
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


    for (int partIndex = 0; partIndex < currentInventoryItem.parts.count(); partIndex++) {
        const InventoryItem::Aspect &part = currentInventoryItem.parts[partIndex];

        const QString name = part.val.split('.').last();
        m_enabledParts.insert(name, partIndex);


        QString category;

        if (partCategories.contains(name)) {
            category = partCategories[name];
        } else {
            category = ItemData::weaponPartType(name);
            if (category.isEmpty()) {
                qWarning() << "Unknown category for" << name;
                category = "Unknown type";
            }
            if (!categoryItems.contains(category)) {
                categoryItems[category] = new QTreeWidgetItem({category});
                m_partsList->addTopLevelItem(categoryItems[category]);
            }

            qWarning() << currentInventoryItem.name << currentInventoryItem.objectShortName << "has part" << name << "which is not in the list of parts for" << currentInventoryItem.name;
        }

        const ItemDescription description = ItemData::itemDescription(name);
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

    for (const QString &partId : partCategories.keys()) {
//        if (enabledParts.contains(partId)) {
//            continue;
//        }


        QTreeWidgetItem *listItem = new QTreeWidgetItem(categoryItems[partCategories[partId]], {makeNamePretty(partId)});
        listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        listItem->setData(0, Qt::UserRole, partId);
        if (m_enabledParts.contains(partId)) {
            listItem->setCheckState(0, Qt::Checked);
            listItem->setData(0, Qt::UserRole + 1, m_enabledParts[partId]);
        } else {
            listItem->setCheckState(0, Qt::Unchecked);
            listItem->setData(0, Qt::UserRole + 1, -1);
        }
    }
    for (QTreeWidgetItem *categoryItem : categoryItems.values()) {
        categoryItem->setExpanded(true);
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
    QList<QTreeWidgetItem*> selected = m_partsList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    QString itemId = selected.first()->data(0, Qt::UserRole).toString();
    if (itemId.isEmpty()) {
        return;
    }
    const ItemDescription description = ItemData::itemDescription(itemId);
    m_partName->setText(description.naming);
    m_partEffects->setText(description.effects);
    m_partNegatives->setText(description.negatives);
    m_partPositives->setText(description.positives);

}

void InventoryTab::onPartChanged(QTreeWidgetItem *item, int column)
{
    QSignalBlocker listSignalBlocker(m_partsList);

    if (column != 0) {
        qWarning() << "Unexpected column" << column;
    }
    const QString itemId = ItemData::objectForShortName(item->data(0, Qt::UserRole).toString());
    if (itemId.isEmpty()) {
        qWarning() << "Empty part id" << item->text(0) << item;
        return;
    }
    const bool enabled = item->checkState(0) == Qt::Checked;

    const QString itemPartCategory = ItemData::partCategory(itemId);
    if (itemPartCategory.isEmpty()) {
        QMessageBox::warning(nullptr, "Invalid item", tr("Failed to find %1\nin list of items with parts.").arg(itemId));
        item->setCheckState(0, enabled ? Qt::Unchecked : Qt::Checked); // reverse
        return;
    }

    const int existingPartPosition = item->data(0, Qt::UserRole + 1).toInt();
    if (!enabled) {
        if (existingPartPosition < 0) {
            qWarning() << "Failed to find" << itemId;
            return;
        }
        m_savegame->removeInventoryItemPart(m_selectedInventoryItem, existingPartPosition);
        item->setData(0, Qt::UserRole + 1, -1);
        m_enabledParts.remove(item->data(0, Qt::UserRole).toString());
        return;
    }

    const InventoryItem &currentInventoryItem = m_savegame->items()[m_selectedInventoryItem];
    qDebug() << itemId << "Part category" << itemPartCategory;
    InventoryItem::Aspect part = ItemData::createInventoryItemPart(currentInventoryItem, itemId);
    if (part.index <= 0) {
        QMessageBox::warning(nullptr, "Invalid item", tr("Failed to find %1\nin list of parts for item.").arg(itemId));
        item->setCheckState(0, enabled ? Qt::Unchecked : Qt::Checked); // reverse
        return;
    }
    if (existingPartPosition < 0 || existingPartPosition >= currentInventoryItem.parts.count()) {
        item->setData(0, Qt::UserRole + 1, m_savegame->inventoryItemsCount());
        m_savegame->addInventoryItemPart(m_selectedInventoryItem, part);
//        currentInventoryItem.parts.append(part);
    } else {
        m_savegame->replaceInventoryItemPart(m_selectedInventoryItem, existingPartPosition, part);
//        currentInventoryItem.parts[existingPartPosition] = part;
    }
    m_enabledParts.insert(item->data(0, Qt::UserRole).toString(), item->data(0, Qt::UserRole + 1).toInt());
    qDebug() << "existing index" << existingPartPosition << "new part name" << part.val;
}

void InventoryTab::load()
{
    m_list->clear();
    for (const InventoryItem &item : m_savegame->items()) {
        QString rarity = item.objectShortName.split('_').last();
        m_list->addItem(tr("%1 (level %2)").arg(item.name, QString::number(item.level)));
//        m_list->addItem(tr("%1%2 (level %3)").arg(rarity + " ", item.name, QString::number(item.level)));
    }
}

void InventoryTab::checkBounds()
{
    QString warningText;

    QHash<QString, int> maxInCategories;
    QHash<QString, int> minInCategories;
    QHash<QString, int> enabledInCategories;
    for (const QString &partId : m_enabledParts.keys()) {
        if (!ItemData::hasItemInfo(partId)) {
            warningText += tr("Unknown item %1\n").arg(partId);
            continue;
        }
        QString category = ItemData::weaponPartType(partId);
        if (category.isEmpty()) {
            warningText += tr("Unknown category for %1\n").arg(partId);
            continue;
        }
//        const ItemPart &itemInfo = ItemData::itemp(partId);
//        if (!maxInCategories.contains(category)) {
//            maxInCategories[category] = itemInfo.
//        }
    }
    m_warningText->setText(warningText);
}
