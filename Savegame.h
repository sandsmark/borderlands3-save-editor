#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <memory>
#include <QString>
#include <QVector>
#include <QUuid>

namespace OakSave {
class Character;
}

class QIODevice;

class Savegame
{
public:
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

        uint32_t dataLength;
    } header{};

    Savegame();
    virtual ~Savegame();

    bool load(const QString &filePath);

    QString characterName() const;
    void setCharacterName(const QString &name);

private:
    std::unique_ptr<OakSave::Character> m_character;
};

#endif // SAVEGAME_H
