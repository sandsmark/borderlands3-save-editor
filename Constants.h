#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QMetaEnum>

struct Constants : public QObject
{
    Q_OBJECT

public:
     // This lets us automagically get the name with Q_ENUM
    template <typename ENUM> static QString enumToString(const ENUM val) {
        static_assert(std::is_enum<ENUM>());
        return QString::fromUtf8(QMetaEnum::fromType<ENUM>().valueToKey(val));
    }

    enum class Class {
        Invalid,
        Beastmaster,
        Gunner,
        Operative,
        Siren
    };
    Q_ENUM(Class)
    static const std::unordered_map<std::string, Class> classObjectNames;
    static Class classFromObjectName(const std::string &key);
    static std::string objectNameFromClass(const Class characterClass);

    enum class Pet {
        Invalid = -1,
        Jabber,
        Spiderant,
        Skag
    };
    Q_ENUM(Pet)
    static const std::unordered_map<std::string, Pet> petKeys;
    static Pet petFromKey(const std::string &key);
    static std::string keyFromPet(const Pet pet);

    enum class Slot {
        Invalid = -1,
        Weapon1,
        Weapon2,
        Weapon3,
        Weapon4,
        Shield,
        Grenade,
        COM,
        Artifact
    };
    Q_ENUM(Slot)
    static const std::unordered_map<std::string, Slot> slotNames;

    static Slot slotFromObjectName(const std::string &objectName);
    static std::string objectNameFromSlot(const Slot slot);

    static constexpr int minLevel = 1;
    static constexpr int maxLevel = 57;
    static const QList<int> requiredXp;
};

#endif // CONSTANTS_H
