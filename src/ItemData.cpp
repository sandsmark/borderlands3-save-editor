#include "ItemData.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

const QVector<ItemPart> ItemData::nullWeaponParts;

ItemData::ItemData()
{
    QFile dbFile(":/data/inventory-serials.json");
    dbFile.open(QIODevice::ReadOnly);
    m_inventoryDb =  QJsonDocument::fromJson(dbFile.readAll()).object();

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
        for (const QString &dep : line[9].split(',')) {
            part.dependencies.append(dep.trimmed());
        }
        for (const QString &exc : line[10].split(',')) {
            part.excluders.append(exc.trimmed());
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
}

bool ItemData::isValid() const
{
    return (!m_englishNames.isEmpty() && !m_inventoryDb.isEmpty() && !m_itemPartCategories.isEmpty() && !m_weaponParts.isEmpty());
}

QString ItemData::getItemAsset(const QString &category, const int index) const
{
    if (index < 1) {
        qWarning() << "Invalid index" << index;
        return {};
    }
    if (!m_inventoryDb.contains(category)) {
        qWarning() << "Invalid category" << category;
        return {};
    }

    const QJsonObject categoryObject = m_inventoryDb[category].toObject();
    const QJsonArray assets = categoryObject["assets"].toArray();
    if (index >= assets.count()) {
        qWarning() << "Asset index" << index << "out of range, max:" << assets.count();
        return {};
    }
    return assets[index].toString();

}

int ItemData::requiredBits(const QString &category, const int requiredVersion) const
{
    if (!m_inventoryDb.contains(category)) {
        qWarning() << "Invalid category" << category;
        return -1;
    }

    const QJsonObject categoryObject = m_inventoryDb[category].toObject();
    const QJsonArray versions = categoryObject["versions"].toArray();
    if (versions.isEmpty()) {
        qWarning() << "No versions for" << category;
        return -1;
    }
    QJsonObject version = versions.first().toObject();

    int bits = version["bits"].toInt();
    for (const QJsonValue &val : versions) {
        version = val.toObject();
        if (!version.contains("bits") || !version.contains("version")) {
            qWarning() << "Invalid version in" << category;
            continue;
        }
        const int currentBits = version["bits"].toInt();
        const int versionNumber = version["version"].toInt();
        if (versionNumber > requiredVersion) {
            return currentBits;
        }

        // I don't understand this, but CJ does so I just follow him blindly
        // The logic seems either flipped or unnecessary
        bits = currentBits;
    }

    return bits;

}

QString ItemData::englishName(const QString &itemName) const
{
    const QString lowerCase = itemName.toLower();
    if (!m_englishNames.contains(lowerCase)) {
        return itemName;
    }
    return m_englishNames[lowerCase].toString();
}

QString ItemData::partCategory(const QString &objectName) const
{
    return m_itemPartCategories[objectName.toLower()].toString();
}

const QVector<ItemPart> &ItemData::weaponParts(const QString &balance)
{
    if (!m_weaponParts.contains(balance)) {
        return nullWeaponParts;
    }

    return m_weaponParts[balance];
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
        for (const QString &dep : line[8].split(',')) {
            part.dependencies.append(dep.trimmed());
        }
        for (const QString &exc : line[9].split(',')) {
            part.excluders.append(exc.trimmed());
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
        if (!m_weaponPartTypes.contains(id)) {
            qWarning() << "Unknown part" << id;
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
        const QList<QByteArray> values = line.split('\t');
        if (values.count() != 3) {
            qWarning() << "Invalid number of values in" + characterClass + "com desc file";
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
