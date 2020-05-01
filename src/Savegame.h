#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "Constants.h"
#include "ItemData.h"

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
    struct Item {
        enum Flag {
            Seen = 1,
            Favorite = 2,
            Trash = 4
        };

        int version = -1;

        struct Aspect {
            int bits = -1;
            int index = -1;
            QString val;

            bool isValid() const {
                return bits > 0 && index > 0 && !val.isEmpty();
            }
        };

        QString name;
        QString objectShortName;

        Aspect balance;
        Aspect data;
        Aspect manufacturer;

        int level = -1;

        int numberOfParts = -1;
        QVector<Aspect> parts;
    };

    Savegame(QObject *parent);
    virtual ~Savegame();

    bool load(const QString &filePath);
    bool save(const QString filePath) const;

    const QVector<Item> &items() const { return m_items; }

    int ammoAmount(const QString &name) const;
    void setAmmoAmount(const QString &name, const int amount);

    int sduAmount(const QString &name) const;
    void setSduAmount(const QString &name, const int amount);

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

signals:
    void nameChanged(const QString &newName);
    void xpChanged(const int xp);
    void levelChanged(const int level);
    void moneyChanged(const int amount);
    void eridiumChanged(const int amount);

    void itemsChanged();
    void fileLoaded();


private:
    Item::Aspect getAspect(const QString &category, const int requiredVersion, BitParser *bits);

    int currencyAmount(const Constants::Currency currenct) const;
    void setCurrency(const Constants::Currency currency, const int amount);
    std::unique_ptr<OakSave::Character> m_character;

    ItemData m_data;

    QVector<Item> m_items;
    int m_maxItemVersion = 1000; // todo
};

#endif // SAVEGAME_H
