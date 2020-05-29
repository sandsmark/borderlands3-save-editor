#include "ItemData.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

const QVector<ItemPart> ItemData::nullWeaponParts;

ItemData::ItemData()
{
    loadInventorySerials();

    QFile namesFile(":/data/english-names.json");
    namesFile.open(QIODevice::ReadOnly);
    m_englishNames =  QJsonDocument::fromJson(namesFile.readAll()).object();

    // From cfi2017
    QFile itemPartCategoriesFile(":/data/balance_to_inv_key.json");
    itemPartCategoriesFile.open(QIODevice::ReadOnly);
    m_itemPartCategories = QJsonDocument::fromJson(itemPartCategoriesFile.readAll()).object();

    QFile weaponPartsFile(":/data/weapon-parts.tsv");
    weaponPartsFile.open(QIODevice::ReadOnly);
    weaponPartsFile.readLine(); // Skip header
    while (!weaponPartsFile.atEnd()) {
        QStringList line = QString::fromUtf8(weaponPartsFile.readLine()).split('\t');
        if (line.length() != 11) {
            qWarning() << "Invalid line in weapon parts file" << line;
            return;
        }
        ItemPart part;
        part.manufacturer = line[0];
        part.itemType = line[1];
        part.rarity = line[2];
        part.balance = line[3];
        part.category = line[4];
        part.minParts = line[5].toInt();
        part.maxParts = line[6].toInt();
        part.weight = line[7].toFloat();
        part.partId = line[8];
        for (QString dep : line[9].split(',', QString::SkipEmptyParts)) {
            dep = dep.trimmed();
            if (dep.isEmpty()) {
                continue;
            }
            part.dependencies.append(dep);
        }
        for (QString exc : line[10].split(',', QString::SkipEmptyParts)) {
            exc = exc.trimmed();
            if (exc.isEmpty()) {
                continue;
            }
            part.excluders.append(exc);
        }
        m_weaponPartTypes[part.partId] = part.category;
        m_weaponPartCategories.insert(part.balance, part.category);
        m_weaponParts[part.balance].append(std::move(part));
    }

    loadPartsForOther("Grenade");
    loadPartsForOther("Shield");
    loadPartsForOther("ClassMod");
    loadPartsForOther("Artifact");

    for (const QFileInfo &file : QDir(":/data/descriptions/weapons/").entryInfoList({"*.tsv"})) {
        loadWeaponPartDescriptions(file.filePath());
    }

    loadShieldPartDescriptions();
    loadGrenadePartDescriptions();
    loadClassModDescriptions("fl4k");
    loadClassModDescriptions("amara");
    loadClassModDescriptions("zane");
    loadItemInfos();
}

ItemData *ItemData::instance()
{
    static ItemData inst;
    return &inst;
}

bool ItemData::isValid()
{
    ItemData *me = instance();
    return (!me->m_englishNames.isEmpty() &&
            !me->m_categoryObjects.isEmpty() &&
            !me->m_categoryRequiredBits.isEmpty() &&
            !me->m_itemPartCategories.isEmpty() &&
            !me->m_weaponParts.isEmpty());
}

QString ItemData::getItemAsset(const QString &category, const int index)
{
    ItemData *me = instance();
    if (index < 0) {
        qWarning() << "Invalid item index" << index;
        return {};
    }
    if (!me->m_categoryObjects.contains(category)) {
        qWarning() << "Invalid category" << category;
        return {};
    }

    if (index >= me->m_categoryObjects[category].count()) {
        qWarning() << "Asset index" << index << "out of range, max:" << me->m_categoryObjects[category].count();
        return {};
    }
    return me->m_categoryObjects[category][index];

}

int ItemData::requiredBits(const QString &category, const int requiredVersion)
{
    ItemData *me = instance();
    if (!me->m_categoryRequiredBits.contains(category)) {
        qWarning() << "Invalid category" << category;
        return -1;
    }

    const QVector<QPair<int, int>> &versions = me->m_categoryRequiredBits[category];

    int bits = versions.first().second;
    for (const QPair<int, int> &version : versions) {
        const int versionNumber = version.first;
        const int currentBits = version.second;
        if (versionNumber > requiredVersion) {
            return currentBits;
        }

        // I don't understand this, but CJ does so I just follow him blindly
        // The logic seems either flipped or unnecessary
        bits = currentBits;
    }

    return bits;

}

QString ItemData::englishName(const QString &itemName)
{
    ItemData *me = instance();
    const QString lowerCase = itemName.toLower();
    if (!me->m_englishNames.contains(lowerCase)) {
        return itemName;
    }
    return me->m_englishNames[lowerCase].toString();
}

QString ItemData::partCategory(const QString &objectName)
{
    ItemData *me = instance();
    const QString lowerCase = objectName.toLower();
    if (!me->m_itemPartCategories.contains(lowerCase)) {
        qWarning() << objectName << "not in part category db";
    }
    return me->m_itemPartCategories[lowerCase].toString();
}

const QVector<ItemPart> &ItemData::weaponParts(const QString &balance)
{
    ItemData *me = instance();
    if (!me->m_weaponParts.contains(balance)) {
        return nullWeaponParts;
    }

    return me->m_weaponParts[balance];
}

int ItemData::partIndex(const QString &category, const QString &id)
{
    ItemData *me = instance();
    if (!me->m_categoryObjects.contains(category)) {
        qWarning() << "Invalid category requested" << category << "for" << id;
        return -1;
    }

    return me->m_categoryObjects[category].indexOf(id);
}

InventoryItem::Aspect ItemData::createInventoryItemPart(const InventoryItem &inventoryItem, const QString &objectName)
{
    ItemData *me = instance();
    InventoryItem::Aspect part;
    part.index = me->partIndex(inventoryItem.partsCategory, objectName);
    if (part.index <= 0) {
        qWarning() << "Invalid object name" << objectName;
        return {};
    }
    part.index++; // it is 1-indexed
    part.bits = requiredBits(inventoryItem.partsCategory, inventoryItem.version);
    part.val = getItemAsset(inventoryItem.partsCategory, part.index - 1);

    return part;
}

void ItemData::loadPartsForOther(const QString &type)
{
    QFile grenadeModsFile(":/data/" + type.toLower() + "-parts.tsv");
    grenadeModsFile.open(QIODevice::ReadOnly);
    grenadeModsFile.readLine(); // Skip header
    while (!grenadeModsFile.atEnd()) {
        QStringList line = QString::fromUtf8(grenadeModsFile.readLine()).split('\t');
        if (line.length() != 10) {
            qWarning() << "Invalid line in grenade mods file" << line;
            return;
        }
        ItemPart part;
        part.manufacturer = line[0];
        part.itemType = type;
        part.rarity = line[1];
        part.balance = line[2];
        part.category = line[3];
        part.minParts = line[4].toInt();
        part.maxParts = line[5].toInt();
        part.weight = line[6].toFloat();
        part.partId = line[7];
        for (QString dep : line[8].split(',')) {
            dep = dep.trimmed();
            if (dep.isEmpty()) {
                continue;
            }
            part.dependencies.append(dep);
        }
        for (QString exc : line[9].split(',')) {
            exc = exc.trimmed();
            if (exc.isEmpty()) {
                continue;
            }
            part.excluders.append(exc);
        }
        m_weaponPartTypes[part.partId] = part.category;
        m_weaponPartCategories.insert(part.balance, part.category);
        m_weaponParts[part.balance].append(std::move(part));
    }
}

void ItemData::loadWeaponPartDescriptions(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    for (const QByteArray &line : file.readAll().split('\n')) {
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        const QList<QByteArray> values = line.split('\t');
        if (values.count() != 5) {
            qWarning() << "Invalid number of values in" << filename;
            return;
        }
        const QString id = values[0].split('.').last();
        if (m_itemDescriptions.contains(id)) {
            qWarning() << "Duplicate description for" << id;
            continue;
        }
        if (id.isEmpty()) {
            qWarning() << "Empty id" << values << "in" << filename;
            continue;
        }
        if (!m_weaponPartTypes.contains(id)) {
//            qWarning() << "Unknown part" << id;
        }
        ItemDescription description;
        description.positives = values[1];
        description.negatives = values[2];
        description.effects = values[3];
        description.naming = values[4];
        m_itemDescriptions[id] = std::move(description);
    }
}

void ItemData::loadShieldPartDescriptions()
{
    QFile file(":/data/descriptions/shields.tsv");
    file.open(QIODevice::ReadOnly);
    for (const QByteArray &line : file.readAll().split('\n')) {
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        const QList<QByteArray> values = line.split('\t');
        if (values.count() != 3) {
            qWarning() << "Invalid number of values in shields desc file";
            return;
        }
        const QString id = values[0].split('.').last();
        if (m_itemDescriptions.contains(id)) {
            qWarning() << "Duplicate description for" << id;
            continue;
        }
        if (!m_weaponPartTypes.contains(id)) {
            qWarning() << "Unknown part" << id;
        }
        ItemDescription description;
        description.effects = values[1];
        description.naming = values[2]; // meh, close enough
        m_itemDescriptions[id] = std::move(description);
    }
}

void ItemData::loadGrenadePartDescriptions()
{
    QFile file(":/data/descriptions/grenades.tsv");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load grenades file";
        return;
    }
    for (const QByteArray &line : file.readAll().split('\n')) {
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        const QList<QByteArray> values = line.split('\t');
        if (values.count() != 3) {
            qWarning() << "Invalid number of values in grenades desc file";
            return;
        }
        const QString id = values[1].split('.').last();
        if (m_itemDescriptions.contains(id)) {
            qWarning() << "Duplicate description for" << id;
            continue;
        }
        if (!m_weaponPartTypes.contains(id)) {
            qWarning() << "Unknown part" << id;
        }
        ItemDescription description;
        description.effects = values[2];
        description.naming = values[0]; // meh, close enough
        m_itemDescriptions[id] = std::move(description);
    }
}

void ItemData::loadClassModDescriptions(const QString &characterClass)
{
    QFile file(":/data/descriptions/com-" + characterClass + ".tsv");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" + characterClass + "com file";
        return;
    }
    for (const QByteArray &line : file.readAll().split('\n')) {
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        QList<QByteArray> values = line.split('\t');
        if (values.count() > 2) {
            values.takeFirst(); // meh, can't be bothered to fix the files
//            qWarning() << "Invalid number of values in" << characterClass << "com desc file";
//            qDebug() << values;
//            return;
        }
        const QString id = values[0].split('.').last();
        if (m_itemDescriptions.contains(id)) {
            qWarning() << "Duplicate description for" << id;
            continue;
        }
        if (!m_weaponPartTypes.contains(id)) {
            qWarning() << "Unknown part" << id;
        }
        ItemDescription description;
        description.effects = values[1];
//        description.naming = values[0]; // meh, close enough
        m_itemDescriptions[id] = std::move(description);
    }
}

void ItemData::loadItemInfos()
{
    QFile namesFile(":/data/item-data.json");
    namesFile.open(QIODevice::ReadOnly);
    const QJsonObject rootObject =  QJsonDocument::fromJson(namesFile.readAll()).object();

    for (const QJsonValue &val : rootObject) {
        const QJsonObject obj = val.toObject();
        const QString assetName = obj["AssetName"].toString();
        if (assetName.isEmpty()) {
            qWarning() << "Invalid object" << obj;
            continue;
        }

        ItemInfo info;

        info.inventoryName = obj["InventoryName"].toString();
        info.inventoryNameLocationKey = obj["InventoryName_LocKey"].toString();

        info.inventoryCategoryHash = obj["InventoryCategoryHash"].toInt();

        info.inventorySize = obj["SizeInInventory"].toDouble();
        info.usesInventoryScore = obj["UsesInventoryScore"].toBool();


        info.monetaryValue = obj["MonetaryValue"].toInt();
        info.baseMonetaryValueModifier = obj["BaseMonetaryValueModifier"].toDouble();

        const QString droppability = obj["Droppability"].toString();
        if (droppability == QLatin1String("EPD_CanDropAndSell")) {
            info.canDropOrSell = true;
        } else if (droppability == QLatin1String("EPD_NoDropOrSell")) {
            info.canDropOrSell = true;
        } else {
            qWarning() << "Unknown droppability" << droppability;
        }


//        qDebug() << "Adding" << assetName << info.inventoryName;
        m_itemInfos[assetName] = std::move(info); // compilers probably figure it out themselves, but meh
    }
    qDebug() << "Loaded" << m_itemInfos.count() << "item infos";

}

void ItemData::loadInventorySerials()
{
    QFile dbFile(":/data/inventory-serials.json");
    dbFile.open(QIODevice::ReadOnly);
    const QJsonObject serialsDb = QJsonDocument::fromJson(dbFile.readAll()).object();
    for (const QString &category : serialsDb.keys()) {
        const QJsonObject &categoryObject = serialsDb[category].toObject();
        const QJsonArray assets = categoryObject["assets"].toArray();
        for (const QJsonValue val : assets) {
            const QString objectName = val.toString();
            m_categoryObjects[category].append(objectName);
            m_shortNameToObject[objectName.split('.').last()] = objectName;
        }

        const QJsonArray versions = categoryObject["versions"].toArray();

        QVector<QPair<int, int>> versionsVector;
        for (const QJsonValue &val : versions) {
            QJsonObject version = val.toObject();
            if (!version.contains("bits") || !version.contains("version")) {
                qWarning() << "Invalid version in" << category;
                continue;
            }
            const int bits = version["bits"].toInt();
            const int versionNumber = version["version"].toInt();
            versionsVector.append({versionNumber, bits});
        }

        m_categoryRequiredBits[category] = std::move(versionsVector);
    }
}
