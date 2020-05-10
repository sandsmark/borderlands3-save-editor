#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "InventoryItem.h"

#include <QJsonObject>
#include <QStringList>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QPair>

// Could use an enum, but memory is cheap and I'm lazy
struct ItemPart {
    QString manufacturer;
    QString itemType;
    QString rarity;
    QString balance;
    QString category;
    int minParts = 0;
    int maxParts = 0;
    float weight = 0.f;
    QString partId;
    QStringList dependencies;
    QStringList excluders;
};

struct ItemDescription {
    QString positives;
    QString negatives;
    QString effects;
    QString naming;
};

struct ItemInfo {
    QString inventoryName;
    QString inventoryNameLocationKey;

    int inventoryCategoryHash;

    float inventorySize = 0.f;
    bool usesInventoryScore = false;

    int monetaryValue = 0.f;
    float baseMonetaryValueModifier = 0.f;

    bool canDropOrSell = false;
};

class ItemData
{
public:
    ItemData();

    bool isValid() const;

    QString getItemAsset(const QString &category, const int index) const;
    int requiredBits(const QString &category, const int requiredVersion) const;

    QString englishName(const QString &itemName) const;
    QString partCategory(const QString &objectName) const;

    const QVector<ItemPart> &weaponParts(const QString &balance);
    QStringList categoriesForWeapon(const QString &balance) const { return m_weaponPartCategories.values(balance); }
    QString weaponPartType(const QString &id) const { return m_weaponPartTypes[id]; }

    int partIndex(const QString &category, const QString &id);

    const ItemDescription &itemDescription(const QString &id) { return m_itemDescriptions[id]; }
    const ItemInfo &itemInfo(const QString &id) { return m_itemInfos[id]; }
    bool hasItemInfo(const QString &id) { return m_itemInfos.contains(id); } // inefficient lol

    const QString &objectForShortName(const QString &shortName) { return m_shortNameToObject[shortName]; }

    InventoryItem::Aspect createInventoryItemPart(const InventoryItem &inventoryItem, const QString &objectName);

private:
    void loadPartsForOther(const QString &type);
    void loadWeaponPartDescriptions(const QString &filename);
    void loadShieldPartDescriptions();
    void loadGrenadePartDescriptions();
    void loadClassModDescriptions(const QString &characterClass);
    void loadItemInfos();
    void loadInventorySerials();

    static const QVector<ItemPart> nullWeaponParts; // so we always can return references

    QJsonObject m_englishNames;
    QJsonObject m_itemPartCategories;
    QHash<QString, QVector<ItemPart>> m_weaponParts;
    QHash<QString, QString> m_weaponPartTypes;
    QMultiMap<QString, QString> m_weaponPartCategories;
    QHash<QString, ItemDescription> m_itemDescriptions;
    QHash<QString, ItemInfo> m_itemInfos;

    QHash<QString, QStringList> m_categoryObjects;
    QHash<QString, QVector<QPair<int, int>>> m_categoryRequiredBits;
    QHash<QString, QString> m_shortNameToObject;
};

#endif // ITEMDATA_H
