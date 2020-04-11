#include "Constants.h"

#include <unordered_map>
#include <string>

const QList<int> Constants::requiredXp = {
    0,          // lvl 1
    358,        // lvl 2
    1241,       // lvl 3
    2850,       // lvl 4
    5376,       // lvl 5
    8997,       // lvl 6
    13886,      // lvl 7
    20208,      // lvl 8
    28126,      // lvl 9
    37798,      // lvl 10
    49377,      // lvl 11
    63016,      // lvl 12
    78861,      // lvl 13
    97061,      // lvl 14
    117757,     // lvl 15
    141092,     // lvl 16
    167206,     // lvl 17
    196238,     // lvl 18
    228322,     // lvl 19
    263595,     // lvl 20
    302190,     // lvl 21
    344238,     // lvl 22
    389873,     // lvl 23
    439222,     // lvl 24
    492414,     // lvl 25
    549578,     // lvl 26
    610840,     // lvl 27
    676325,     // lvl 28
    746158,     // lvl 29
    820463,     // lvl 30
    899363,     // lvl 31
    982980,     // lvl 32
    1071435,    // lvl 33
    1164850,    // lvl 34
    1263343,    // lvl 35
    1367034,    // lvl 36
    1476041,    // lvl 37
    1590483,    // lvl 38
    1710476,    // lvl 39
    1836137,    // lvl 40
    1967582,    // lvl 41
    2104926,    // lvl 42
    2248285,    // lvl 43
    2397772,    // lvl 44
    2553501,    // lvl 45
    2715586,    // lvl 46
    2884139,    // lvl 47
    3059273,    // lvl 48
    3241098,    // lvl 49
    3429728,    // lvl 50
    3625271,    // lvl 51
    3827840,    // lvl 52
    4037543,    // lvl 53
    4254491,    // lvl 54
    4478792,    // lvl 55
    4710556,    // lvl 56
    4949890,    // lvl 57
    5196902,    // lvl 58
    5451701,    // lvl 59
    5714393,    // lvl 60
    5985086,    // lvl 61
    6263885,    // lvl 62
    6550897,    // lvl 63
    6846227,    // lvl 64
    7149982,    // lvl 65
    7462266,    // lvl 66
    7783184,    // lvl 67
    8112840,    // lvl 68
    8451340,    // lvl 69
    8798786,    // lvl 70
    9155282,    // lvl 71
    9520931,    // lvl 72
    9895837,    // lvl 73
    10280103,    // lvl 74
    10673830,    // lvl 75
    11077120,    // lvl 76
    11490077,    // lvl 77
    11912801,    // lvl 78
    12345393,    // lvl 79
    12787955,    // lvl 80
};

template<typename T>
static std::unordered_map<T, std::string> reverseMapping(const std::unordered_map<std::string, T> &mapping)
{
    std::unordered_map<T, std::string> ret;
    for (const std::pair<const std::string, T> &entry : mapping) {
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
