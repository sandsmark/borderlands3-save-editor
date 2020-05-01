#include "ItemData.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

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

}

bool ItemData::isValid() const
{
    return (!m_englishNames.isEmpty() && !m_inventoryDb.isEmpty() && !m_itemPartCategories.isEmpty());
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
