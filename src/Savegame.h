#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "Constants.h"
#include "ItemData.h"
#include "InventoryItem.h"

#include <memory>
#include <QString>
#include <QVector>
#include <QUuid>
#include <QObject>
#include <QJsonObject>

namespace OakSave {
class Character;
}

class QIODevice;
struct BitParser;

class Savegame : public QObject
{
    Q_OBJECT

    struct Header {
        uint32_t savegameVersion;
        uint32_t packageVersion;

        uint16_t engineMajorVersion;
        uint16_t engineMinorVersion;
        uint16_t enginePatchVersion;

        uint32_t engineBuild;
        QString buildId;

        uint32_t customFormatVersion;
        uint32_t customFormatCount;

        struct CustomFormat {
            QUuid id;
            int entry = 0;
        };
        QVector<CustomFormat> customFormats;

        QString savegameType;

        int32_t dataLength;
    } m_header{};

public:
    Savegame(QObject *parent);
    virtual ~Savegame();

    bool load(const QString &filePath);
    bool save(const QString filePath) const;

    const QVector<InventoryItem> &items() { return m_items; }
    int inventoryItemsCount() const { return m_items.count(); }
    const InventoryItem &inventoryItem(const int index) { return m_items[index]; }
    void addInventoryItemPart(const int index, const InventoryItem::Aspect &part);
    void removeInventoryItemPart(const int index, const QString partId);
    void setItemLevel(const int index, const int newLevel);

    int ammoAmount(const QString &name) const;
    void setAmmoAmount(const QString &name, const int amount);

    int sduAmount(const QString &name) const;
    void setSduAmount(const QString &name, const int amount);

    QStringList activeMissions() const;
    QVector<bool> objectivesCompleted(const QString &missionName);

public slots:
    //////////////////////////
    // Character stuff
    QString characterName() const;
    void setCharacterName(const QString &name);

    int xp() const;
    void setXp(const int newXp);

    int level() const;
    void setLevel(const int newLevel);

    int money() const;
    void setMoney(const int amount);

    int eridium() const;
    void setEridium(const int amount);

    void regenerateUuid();

    void setSaveSlot(const int slotId);

signals:
    void nameChanged(const QString &newName);
    void xpChanged(const int xp);
    void levelChanged(const int level);
    void moneyChanged(const int amount);
    void eridiumChanged(const int amount);

    void itemsChanged();
    void fileLoaded();

    void uuidChanged(const QString &uuid);

    void saveSlotChanged(const int slotId);


private:
    InventoryItem parseItem(const std::string &obfuscatedSerial);
    std::string serializeItem(const InventoryItem &item);

    InventoryItem::Aspect getAspect(const QString &category, const int requiredVersion, BitParser *bits);
    void putAspect(const InventoryItem::Aspect &aspect, const QString &category, const int requiredVersion, BitParser *bits);

    int currencyAmount(const Constants::Currency currenct) const;
    void setCurrency(const Constants::Currency currency, const int amount);
    std::unique_ptr<OakSave::Character> m_character;

    QVector<InventoryItem> m_items;
    int m_maxItemVersion = 1000; // todo
};


#endif // SAVEGAME_H
