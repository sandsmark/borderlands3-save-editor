#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QJsonObject>
#include <QStringList>
#include <QMap>
#include <QHash>

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

private:
    void loadPartsForOther(const QString &type);

    static const QVector<ItemPart> nullWeaponParts; // so we always can return references

    QJsonObject m_inventoryDb; // TODO: parse to in-memory struct, currently very inefficient
    QJsonObject m_englishNames;
    QJsonObject m_itemPartCategories;
    QHash<QString, QVector<ItemPart>> m_weaponParts;
    QHash<QString, QString> m_weaponPartTypes;
    QMultiMap<QString, QString> m_weaponPartCategories;
};

#endif // ITEMDATA_H
