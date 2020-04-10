#include "Constants.h"

#include <unordered_map>
#include <string>

template<typename T>
static std::unordered_map<T, std::string> reverseMapping(const std::unordered_map<std::string, T> &mapping)
{
    std::unordered_map<T, std::string> ret;
    for (const std::pair<std::string, T> &entry : mapping) {
        ret[entry.second] = entry.first;
    }

    return ret;
}

// Don't need to account for invalid enums
template<typename T>
static std::string lookupString(const T val, const std::unordered_map<T, std::string> &map)
{
    typename std::unordered_map<T, std::string>::const_iterator it = map.find(val);
    if (it == map.end()) {
        return "";
    }
    return it->second;
}



////////////////////////////////////
// Character classes
////////////////////////////////////

const std::unordered_map<std::string, Constants::Class> Constants::classObjectNames = {
    { "Game/PlayerCharacters/Beastmaster/PlayerClassId_Beastmaster.PlayerClassId_Beastmaster", Class::Beastmaster} ,
    { "Game/PlayerCharacters/Gunner/PlayerClassId_Gunner.PlayerClassId_Gunner", Class::Gunner} ,
    { "Game/PlayerCharacters/Operative/PlayerClassId_Operative.PlayerClassId_Operative", Class::Operative} ,
    { "Game/PlayerCharacters/SirenBrawler/PlayerClassId_Siren.PlayerClassId_Siren", Class::Siren} ,
};

Constants::Class Constants::classFromObjectName(const std::string &key)
{
    std::unordered_map<std::string, Class>::const_iterator it = classObjectNames.find(key);
    if (it == classObjectNames.end()) {
        return Class::Invalid;
    }
    return it->second;
}

std::string Constants::objectNameFromClass(const Constants::Class characterClass)
{
    return lookupString(characterClass, reverseMapping(classObjectNames));
}


////////////////////////////////////
// Pets
////////////////////////////////////

const std::unordered_map<std::string, Constants::Pet> Constants::petKeys = {
        { "petmonkey", Pet::Jabber },
        { "petspiderant", Pet::Spiderant },
        { "petskag", Pet::Skag },
};

Constants::Pet Constants::petFromKey(const std::string &key)
{
    std::unordered_map<std::string, Pet>::const_iterator it = petKeys.find(key);
    if (it == petKeys.end()) {
        return Pet::Invalid;
    }
    return it->second;
}

std::string Constants::keyFromPet(const Pet pet)
{
    return lookupString(pet, reverseMapping(petKeys));
}

////////////////////////////////////
// Inventory slots
////////////////////////////////////

const std::unordered_map<std::string, Constants::Slot> Constants::slotNames = {
        {"/Game/Gear/Weapons/_Shared/_Design/InventorySlots/BPInvSlot_Weapon1.BPInvSlot_Weapon1", Slot::Weapon1 },
        {"/Game/Gear/Weapons/_Shared/_Design/InventorySlots/BPInvSlot_Weapon2.BPInvSlot_Weapon2", Slot::Weapon2 },
        { "/Game/Gear/Weapons/_Shared/_Design/InventorySlots/BPInvSlot_Weapon3.BPInvSlot_Weapon3", Slot::Weapon3},
        { "/Game/Gear/Weapons/_Shared/_Design/InventorySlots/BPInvSlot_Weapon4.BPInvSlot_Weapon4", Slot::Weapon4},
        { "/Game/Gear/Shields/_Design/A_Data/BPInvSlot_Shield.BPInvSlot_Shield", Slot::Shield},
        { "/Game/Gear/GrenadeMods/_Design/A_Data/BPInvSlot_GrenadeMod.BPInvSlot_GrenadeMod", Slot::Grenade},
        { "/Game/Gear/ClassMods/_Design/_Data/BPInvSlot_ClassMod.BPInvSlot_ClassMod", Slot::COM},
        { "/Game/Gear/Artifacts/_Design/_Data/BPInvSlot_Artifact.BPInvSlot_Artifact", Slot::Artifact},
    };

Constants::Slot Constants::slotFromObjectName(const std::string &objectName)
{
    std::unordered_map<std::string, Slot>::const_iterator it = slotNames.find(objectName);
    if (it == slotNames.end()) {
        return Slot::Invalid;
    }
    return it->second;
}

std::string Constants::objectNameFromSlot(const Constants::Slot slot)
{
    return lookupString(slot, reverseMapping(slotNames));
}
