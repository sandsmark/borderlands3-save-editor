#pragma once

#include <QString>
#include <QVector>
#include <QByteArray>

struct InventoryItem {
    enum Flag {
        Seen = 1,
        Favorite = 2,
        Trash = 4
    };

    bool writable = false;

    bool isValid() const {
        return version != -1 &&
                !name.isEmpty() &&
                !objectShortName.isEmpty() &&
                balance.isValid() &&
                data.isValid() &&
                manufacturer.isValid() &&
                level != -1 &&
                numberOfParts != -1 &&
                !parts.isEmpty() &&
                numCustom == 0 // TODO
                ;
    }

    int version = -1;

    QString partsCategory;

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
    QVector<Aspect> genericParts;
    int seed = 0;

    QVector<uint8_t> itemWearMaybe;

    int numCustom = -1;

    QByteArray remainingBits; // TODO
};
