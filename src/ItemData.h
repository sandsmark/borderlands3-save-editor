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
    static ItemData *instance();

    static bool isValid();

    static QString getItemAsset(const QString &category, const int index);
    static int requiredBits(const QString &category, const int requiredVersion);

    static QString englishName(const QString &itemName);
    static QString partCategory(const QString &objectName);

    static const QVector<ItemPart> &weaponParts(const QString &balance);
    static QStringList categoriesForWeapon(const QString &balance) { return instance()->m_weaponPartCategories.values(balance); }
    static QString weaponPartType(const QString &id) { return instance()->m_weaponPartTypes[id]; }

    static int partIndex(const QString &category, const QString &id);

    static const ItemDescription &itemDescription(const QString &id) { return instance()->m_itemDescriptions[id]; }
    static const ItemInfo &itemInfo(const QString &id) { return instance()->m_itemInfos[id]; }
    static bool hasItemInfo(const QString &id) { return instance()->m_itemInfos.contains(id); } // inefficient lol

    static const QString &objectForShortName(const QString &shortName) { return instance()->m_shortNameToObject[shortName]; }

    static InventoryItem::Aspect createInventoryItemPart(const InventoryItem &inventoryItem, const QString &objectName);

//    static bool hasItemPart(const QString &id) { return instance()->m_}

private:
    ItemData();

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
