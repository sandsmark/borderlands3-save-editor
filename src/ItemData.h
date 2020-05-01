#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QJsonObject>

class ItemData
{
public:
    ItemData();

    bool isValid() const;

    QString getItemAsset(const QString &category, const int index) const;
    int requiredBits(const QString &category, const int requiredVersion) const;

    QString englishName(const QString &itemName) const;
    QString partCategory(const QString &objectName) const;

private:
    QJsonObject m_inventoryDb; // TODO: parse to in-memory struct, currently very inefficient
    QJsonObject m_englishNames;
    QJsonObject m_itemPartCategories;
};

#endif // ITEMDATA_H
