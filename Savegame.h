#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <memory>
#include <QString>
#include <QVector>
#include <QUuid>
#include <QObject>

namespace OakSave {
class Character;
}

class QIODevice;

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

public slots:
    QString characterName() const;
    void setCharacterName(const QString &name);

    int xp() const;
    void setXp(const int newXp);

    int level() const;
    void setLevel(const int newLevel);

signals:
    void nameChanged(const QString &newName);
    void xpChanged(const int xp);
    void levelChanged(const int level);


private:
    std::unique_ptr<OakSave::Character> m_character;
};

#endif // SAVEGAME_H
