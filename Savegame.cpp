#include "Savegame.h"

#include "Constants.h"

#include "OakSave.pb.h"

#include <QFile>
#include <QMessageBox>
#include <QtEndian>
#include <QDebug>

Savegame::Savegame(QObject *parent) :
    QObject(parent)
{
    m_character = std::make_unique<OakSave::Character>();
}

Savegame::~Savegame()
{ // can't be inline or default, because unique_ptr in gcc is short-bus special
}

// could be simpler and more efficient and who uses powerpc these days, but meh
template <typename T>
static bool readInt(T *output, QIODevice *input)
{
    char data[sizeof(T)];
    if (input->read(data, sizeof(data)) < qint64(sizeof(data))) {
        return false;
    }
    *output = qFromLittleEndian<T>(data);
    return true;
}

static bool readString(QString *output, QIODevice *input)
{
    int stringLength;
    if (!readInt(&stringLength, input) || stringLength < 0) {
        return false;
    }

    // just an empty string, because it's supposed to be 0 terminated
    if (stringLength <= 1) {
        return true;
    }
    *output = QString::fromUtf8(input->read(stringLength), stringLength - 1); // Supposed to be \0-terminated, but just in case
    return true;
}

bool Savegame::load(const QString &filePath)
{
    m_header = {};

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Failed to open file", file.errorString());
        return false;
    }

    const QByteArray fileMagic = file.read(4);
    if (fileMagic != "GVAS") {
        QMessageBox::warning(nullptr, "Invalid header", "Invalid file, starts with:\n" + fileMagic.toHex() + "'.");
        return false;
    }
    bool couldReadHeader =
            readInt(&m_header.savegameVersion, &file) &&
            readInt(&m_header.packageVersion, &file) &&
            readInt(&m_header.engineMajorVersion, &file) &&
            readInt(&m_header.engineMinorVersion, &file) &&
            readInt(&m_header.enginePatchVersion, &file) &&
            readInt(&m_header.engineBuild, &file) &&
            readString(&m_header.buildId, &file) &&
            readInt(&m_header.customFormatVersion, &file) &&
            readInt(&m_header.customFormatCount, &file);

    if (!couldReadHeader) {
        QMessageBox::warning(nullptr, "Invalid header", "Invalid file, failed to read header.\n" + file.errorString());
        return false;
    }
    if (m_header.customFormatCount > 1000) { // idk, just sanity
        QMessageBox::warning(nullptr, "Invalid header", "Invalid file, too many custom formats: " + QString::number(m_header.customFormatCount));
        return false;
    }
    qDebug() << "Custom formats" << m_header.customFormatCount;

    m_header.customFormats.resize(m_header.customFormatCount);

    for (Header::CustomFormat &format : m_header.customFormats) {
        format.id = QUuid::fromRfc4122(file.read(16));
        if (format.id.isNull()) {
            QMessageBox::warning(nullptr, "Invalid header", "Invalid custom format description id");
            return false;
        }
        if (!readInt(&format.entry, &file)) {
            QMessageBox::warning(nullptr, "Invalid header", "Invalid file, failed to read custom format entry index.\n" + file.errorString());
            return false;
        }
        qDebug() << "Format" << format.id << format.entry;
    }
    if (!readString(&m_header.savegameType, &file)) {
        QMessageBox::warning(nullptr, "Invalid header", "Invalid file, failed to read savegame type.\n" + file.errorString());
        return false;
    }
    if (!readInt(&m_header.dataLength, &file)) {
            QMessageBox::warning(nullptr, "Invalid header", "Failed to read data length");
            return false;
    }

    qDebug() << "Savegame version" << m_header.savegameType << m_header.savegameVersion;
    qDebug() << "Package version" << m_header.packageVersion;
    qDebug() << "Build id:" << m_header.buildId;
    qDebug() << "Engine version" << m_header.engineMajorVersion << m_header.engineMinorVersion << m_header.enginePatchVersion << m_header.engineBuild;
    qDebug() << "Custom format version" << m_header.customFormatVersion;


    QByteArray data = file.readAll();
    if (data.size() != m_header.dataLength) { // yeah yeah, padding, but it needs to be significantly larger so whatever
        QMessageBox::warning(nullptr, "Failed to read from file", "Wrong amount of data available, expected " + QString::number(m_header.dataLength) + ", but got " + QString::number(data.size()));
        return false;
    }

    const uint8_t prefixMask[] = {
        0x71, 0x34, 0x36, 0xB3, 0x56, 0x63, 0x25, 0x5F,
        0xEA, 0xE2, 0x83, 0x73, 0xF4, 0x98, 0xB8, 0x18,
        0x2E, 0xE5, 0x42, 0x2E, 0x50, 0xA2, 0x0F, 0x49,
        0x87, 0x24, 0xE6, 0x65, 0x9A, 0xF0, 0x7C, 0xD7
    };
    static_assert(sizeof(prefixMask) == ' ');

    static uint8_t xorMask[] = {
        0x7C, 0x07, 0x69, 0x83, 0x31, 0x7E, 0x0C, 0x82,
        0x5F, 0x2E, 0x36, 0x7F, 0x76, 0xB4, 0xA2, 0x71,
        0x38, 0x2B, 0x6E, 0x87, 0x39, 0x05, 0x02, 0xC6,
        0xCD, 0xD8, 0xB1, 0xCC, 0xA1, 0x33, 0xF9, 0xB6,
    };
    static_assert(sizeof(xorMask) == ' ');

    char *dataRaw = data.data();
    for (int i=data.size() - 1; i >= 0; i--) {
        // I want a prize for ugly code
        // Premature optimization^Wobfuscation (it's probably not faster than doing it the pretty way)
        dataRaw[i] ^= (i < int(sizeof(prefixMask)) ? prefixMask[i] : dataRaw[i - sizeof(prefixMask)])
            ^ xorMask[i % sizeof(xorMask)];
    }

    if (!m_character->ParseFromArray(dataRaw, data.size())) {
        // protobuf never gives us anything, but whatever
        QMessageBox::warning(nullptr, "Invalid file", "Failed to parse file contents:\n" + QString::fromStdString(m_character->InitializationErrorString()));
        return false;
    }

    if (!m_character->IsInitialized()) {
        return false;
    }
    emit nameChanged(characterName());
    emit xpChanged(xp());
    emit levelChanged(level());
    return true;
}

QString Savegame::characterName() const
{
    return QString::fromStdString(m_character->preferred_character_name());
}

void Savegame::setCharacterName(const QString &name)
{
    m_character->set_preferred_character_name(name.toStdString());
}

int Savegame::xp() const
{
    return m_character->experience_points();
}

void Savegame::setXp(const int newXp)
{
    if (newXp == xp()) {
        return;
    }
    const int oldLevel = level();

    m_character->set_experience_points(newXp);

    if (oldLevel != level()) {
        emit levelChanged(level());
    }
}

int Savegame::level() const
{
    const int currentXp = xp();
    int level = 0;
    for (const int requiredXp : Constants::requiredXp) {
        if (currentXp >= requiredXp) {
            level++;
        } else {
            return level;
        }
    }

    qWarning() << "Invalid amount of xp" << currentXp << ", can't get level";
    return level;
}

void Savegame::setLevel(const int newLevel)
{
    if (newLevel > Constants::maxLevel) {
        qWarning() << "Invalid new level" << newLevel;
        return;
    }

    if (newLevel == level()) {
        return;
    }
    const int requiredXp = Constants::requiredXp[newLevel-1];
    if (requiredXp != xp()) {
        m_character->set_experience_points(requiredXp);
        emit xpChanged(requiredXp);
    }
}
