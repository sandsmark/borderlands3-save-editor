#include "Savegame.h"

#include "OakSave.pb.h"

#include "obfuscation.h"

#include <QBitArray>
#include <QFile>
#include <QMessageBox>
#include <QtEndian> // all the qFromLittleEndian is valid for the PC saves at least
#include <QDebug>
#include <QtMath>
#include <deque>

//#include <bitset> // More stuff that we want than QBitSet (like shifting) fuck std

// Idea stolen wholesale from CJ
// I'm not in the mood to write a long switch-case-shift-bits-thing
struct BitParser
{
    BitParser() = default;

    BitParser(const QByteArray &data)
    {
        QBitArray bits = QBitArray::fromBits(data.data(), data.size() * 8);
        for (int i=0; i < bits.size(); i++) {
            m_bits.append(bits[i] ? '1' : '0');
        }
    }

    int bitsLeft() const {
        return m_bits.count();
    }

    void put(const uint64_t number, const uint64_t count) {
        if (count >= sizeof(number) * 8) {
            qWarning() << "Trying to store invalid amount of bits" << count;
            return;
        }
        QBitArray bits = QBitArray::fromBits(reinterpret_cast<const char*>(&number), count);
        for (int i=0; i < bits.size(); i++) {
            m_bits.append(bits[i] ? '1' : '0');
        }
    }

    QByteArray toBinaryData() const {
        QBitArray bits(m_bits.count());
        for (int i=0; i<m_bits.length(); i++) {
            bits.setBit(i, m_bits[i] == '1');
        }

        return QByteArray(bits.bits(), qCeil(bits.size() / 8.));
    }
    QByteArray asOnesAndZeros();

    int eat(const int count) {
        if (count <= 0) {
            return 0;
        }
        if (count > m_bits.size()) {
            qWarning() << "Invalid amount of bits requested" << count << "only have" << m_bits.size();
            return -1;
        }

        QByteArray toEat = m_bits.mid(0, count);
        m_bits = m_bits.mid(count);
        std::reverse(toEat.begin(), toEat.end()); // idk lol I don't know computer

        bool ok; // lol as if
        return toEat.toInt(&ok, 2);
    }
    QByteArray m_bits;
private:
};

Savegame::Savegame(QObject *parent) :
    QObject(parent)
{
    m_character = std::make_unique<OakSave::Character>();

#if 0
    QFile infile("name.json");
    infile.open(QIODevice::ReadOnly);
    QFile outFile("dump.compressed");
    outFile.open(QIODevice::WriteOnly);
    outFile.write(qCompress(infile.readAll(), 9));
#endif
}

Savegame::~Savegame()
{ // can't be inline or default, because unique_ptr in gcc is short-bus special
}

static QByteArray deobfuscateItem(const QByteArray &input)
{
    if (input.size() < 6) {
        QMessageBox::warning(nullptr, "Invalid file", QObject::tr("Invalid item serial (too short: %1).").arg(input.size()));
        return {};
    }
    if (input[0] != 3) {
        QMessageBox::warning(nullptr, "Invalid file", QObject::tr("Invalid item (item serial doesn't start with 3, got %1).").arg(int(input[0])));
        return {};
    }

    const int32_t seed = qFromBigEndian<int32_t>(input.data() + 1);

    QByteArray data = input.mid(5); // 1 first byte is 3, 4 bytes is int seed

    if (seed != 0) {
        uint32_t key = (seed >> 5) & 0xFFFFFFFF;
        for (char &c : data) {
            key = (key * obfuscation::itemKey) % obfuscation::itemMask;
            c ^= key;
        }
        const int steps = (seed & 0x1f) % data.size();
        std::rotate(data.rbegin(), data.rbegin() + steps, data.rend());
    } else {
        qWarning() << "0 seed?";
    }

    // Normal crc32
    const QByteArray toChecksum = input.mid(0, 5) + "\xff\xff" + data.mid(2);
    uint32_t crc32 = 0xffffffff;
    for (const char c : toChecksum) {
        uint32_t val = (crc32 ^ c) & 0xff;
        for (int i=0; i<8; i++) {
            val = (val & 1) ? (val >>1 ) ^ 0xedb88320 : val >> 1;
        }
        crc32 = val ^ (crc32 >> 8);
    }
    crc32 ^= 0xffffffff;

    const uint16_t computedChecksum = (crc32 >> 16) ^ crc32;
    const uint16_t checksum = qFromBigEndian<uint16_t>(data.data());
    if (computedChecksum != checksum) {
        QMessageBox::warning(nullptr, "Invalid file", QObject::tr("Invalid item (checksum failed)."));
        qWarning() << "Checksum mismatch" << computedChecksum << "expected" << checksum;
        return {};
    }

    return data.mid(2);
}

// extremely inefficient, but can't be bothered to think
static QByteArray obfuscateItem(QByteArray input, const int seed)
{
    if (input.isEmpty()) {
        qWarning() << "Can't obfuscate empty string";
        return {};
    }

    QByteArray ret(5, 0);
    ret[0] = 3;
    qToBigEndian(seed, ret.data() + 1);

    const QByteArray toChecksum = ret + "\xff\xff" + input;
    uint32_t crc32 = 0xffffffff;
    for (const char c : toChecksum) {
        uint32_t val = (crc32 ^ c) & 0xff;
        for (int i=0; i<8; i++) {
            val = (val & 1) ? (val >>1 ) ^ 0xedb88320 : val >> 1;
        }
        crc32 = val ^ (crc32 >> 8);
    }
    crc32 ^= 0xffffffff;

    const uint16_t computedChecksum = (crc32 >> 16) ^ crc32;

    QByteArray checksumBytes(sizeof(computedChecksum), 0);
    qToBigEndian(computedChecksum, checksumBytes.data());
    input.prepend(checksumBytes);

    if (seed != 0) {
        const int steps = (seed & 0x1f) % input.size();
        std::rotate(input.begin(), input.begin() + steps, input.end());
        uint32_t key = (seed >> 5) & 0xFFFFFFFF;
        for (char &c : input) {
            key = (key * obfuscation::itemKey) % obfuscation::itemMask;
            c ^= key;
        }
    }

    ret.append(input);

    return ret;
}

// could be simpler and more efficient and who uses powerpc these days, but meh
template <typename T>
static bool readInt(T *output, QIODevice *input)
{
    char data[sizeof(T)];
    if (input->read(data, sizeof(data)) != qint64(sizeof(data))) {
        return false;
    }
    *output = qFromLittleEndian<T>(data);
    return true;
}

template <typename T>
static bool writeInt(const T input, QIODevice *output)
{
    char data[sizeof(T)];
    qToLittleEndian(input, data);
    const qint64 written = output->write(data, sizeof(data));
    if (written != qint64(sizeof(data))) {
        qWarning() << "short writE" << written << sizeof(data);
        return false;
    }
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

static bool writeString(const QString &input, QIODevice *output)
{
    const QByteArray utf8 = input.toUtf8() + '\0';
    const int stringLength = utf8.length();
    if (!writeInt(stringLength, output) || stringLength < 0) {
        return false;
    }

    output->write(utf8);

    return true;
}

bool Savegame::load(const QString &filePath)
{
    if (!ItemData::isValid()) {
        qWarning() << "Databases not loaded!";
        return false;
    }
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
//    qDebug() << "Custom formats" << m_header.customFormatCount;

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
//        qDebug() << "Format" << format.id << format.entry;
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

    char *dataRaw = data.data();
    for (int i=data.size() - 1; i >= 0; i--) {
        // I want a prize for ugly code
        // Premature optimization^Wobfuscation (it's probably not faster than doing it the pretty way)
        dataRaw[i] ^= (i < int(sizeof(obfuscation::prefixMask)) ? obfuscation::prefixMask[i] : dataRaw[i - sizeof(obfuscation::prefixMask)])
            ^ obfuscation::xorMask[i % sizeof(obfuscation::xorMask)];
    }

    if (!m_character->ParseFromArray(dataRaw, data.size())) {
        // protobuf never gives us anything, but whatever
        QMessageBox::warning(nullptr, "Invalid file", "Failed to parse file contents (protobuf parse failed):\n" + QString::fromStdString(m_character->InitializationErrorString()));
        return false;
    }

    if (!m_character->IsInitialized()) {
        QMessageBox::warning(nullptr, "Invalid file", "Failed to parse file contents (protobuf not initialized):\n" + QString::fromStdString(m_character->InitializationErrorString()));
        return false;
    }

    qDebug() << "Items:" << m_character->inventory_items_size();
//    int maxBits = 0;
//    if (m_character->inventory_items_size() > 0) {
    for (int itemIndex=0; itemIndex<m_character->inventory_items_size(); itemIndex++) {
        const ::OakSave::OakInventoryItemSaveGameData& entry = m_character->inventory_items(itemIndex);
        QByteArray deobfuscated = deobfuscateItem(QByteArray::fromStdString(entry.item_serial_number()));
        QByteArray obfuscated = obfuscateItem(deobfuscated, qFromBigEndian<int32_t>(entry.item_serial_number().data() + 1));

        if (entry.item_serial_number() != obfuscated.toStdString()) {
            qWarning() << "OBfuscation failed" << deobfuscated.toHex(' ');
            qDebug() << obfuscated.toHex(' ');
            qDebug() << QByteArray::fromStdString(entry.item_serial_number()).toHex(' ');
        }
        InventoryItem item = parseItem(entry.item_serial_number());
        if (item.isValid()) {
            const std::string reEncoded = serializeItem(item);
            if (entry.item_serial_number() == reEncoded){
                item.writable = true;
            } else {
                qWarning() << "Re-encoding failed" << item.objectShortName;
                qDebug() << "Encoded: " << QByteArray::fromStdString(reEncoded).toHex(' ');
                qDebug() << "Original:" << deobfuscated.toHex(' ');
            }

            m_items.append(item);
        } else {
            qWarning() << "Invalid item:" << itemIndex;
        }
    }
    emit itemsChanged();
//    qDebug() << "Max bits:" << maxBits;


    const QString backupFilename = file.fileName() + ".backup";
    QFile::remove(backupFilename);
    file.copy(backupFilename);

    emit nameChanged(characterName());
    emit xpChanged(xp());
    emit levelChanged(level());
    emit moneyChanged(money());
    emit eridiumChanged(eridium());

    emit uuidChanged(QString::fromStdString(m_character->save_game_guid()));
    emit saveSlotChanged(m_character->save_game_id());

    emit fileLoaded();

    return true;
}

InventoryItem Savegame::parseItem(const std::string &obfuscatedSerial)
{
    QByteArray serial = deobfuscateItem(QByteArray::fromStdString(obfuscatedSerial));
    if (serial.isEmpty()) {
        qWarning() << "Couldn't deobfuscate";
        return {};
    }

    BitParser bits(serial);
    if (bits.eat(8) != 128) {
        qWarning() << "Invalid start";
        QMessageBox::warning(nullptr, "Invalid file", tr("Item data has wrong start."));
        return {};
    }

    InventoryItem item;
    item.version = bits.eat(7);
    item.seed = qFromBigEndian<int32_t>(obfuscatedSerial.data() + 1);
    if (item.version > m_maxItemVersion) {
        QMessageBox::warning(nullptr, "Invalid file", tr("Item version is too high (%1, we only support %2").arg(item.version, m_maxItemVersion));
        item.remainingBits = bits.m_bits;
        return item;
    }
    item.balance = getAspect("InventoryBalanceData", item.version, &bits);
    if (!item.balance.isValid()) {
        QMessageBox::warning(nullptr, "Invalid file", tr("Invalid item balance"));
        qWarning() << "Invalid item balance";
        item.remainingBits = bits.m_bits;
        return item;
    }

    item.objectShortName = item.balance.val.split('/', QString::SkipEmptyParts).last().split('.', QString::SkipEmptyParts).last();
    item.name = ItemData::englishName(item.objectShortName);

    item.data = getAspect("InventoryData", item.version, &bits); // these seem wrong
    if (!item.data.isValid()) {
        QMessageBox::warning(nullptr, "Invalid file", tr("Invalid item data"));
        item.remainingBits = bits.m_bits;
        return item;
    }
    item.manufacturer = getAspect("ManufacturerData", item.version, &bits);
    if (!item.manufacturer.isValid()) {
        QMessageBox::warning(nullptr, "Invalid file", tr("Invalid item manufacturer"));
        item.remainingBits = bits.m_bits;
        return item;
    }
    item.level = bits.eat(7);
    item.numberOfParts = bits.eat(6);

    item.partsCategory = ItemData::partCategory(item.balance.val.toLower());
    bool itemFailed = false;
    if (!item.partsCategory.isEmpty()) {
        for (int partIndex = 0; partIndex < item.numberOfParts; partIndex++) {
            InventoryItem::Aspect part = getAspect(item.partsCategory, item.version, &bits);
            if (!part.isValid()) {
                qWarning() << "Invalid" << item.balance.val << item.partsCategory;
                //                    QMessageBox::warning(nullptr, "Invalid file", tr("Failed to get item part %1 for item %2.").arg(partIndex).arg(item.name));
                itemFailed = true;
                break;
                //                    return false;
            }
            item.parts.append(part);
        }
    } else {
        qWarning() << "Item not in parts database:" << item.balance.val;
        itemFailed = true;
    }

    if (!itemFailed) {
        const int genericPartsCount = bits.eat(4);
        for (int partIndex = 0; partIndex < genericPartsCount; partIndex++) {
            InventoryItem::Aspect genericPart = getAspect("InventoryGenericPartData", item.version, &bits);
            if (!genericPart.isValid()) {
                qWarning() << "Invalid generic item part number" << partIndex;
                itemFailed = true;
                break;
            }
            item.genericParts.append(genericPart);
            qDebug() << "Got generic part" << genericPart.index;
        }
    }

    if (!itemFailed) {
        const int itemWearCount = bits.eat(8);
        for (int index = 0; index<itemWearCount; index++) {
            item.itemWearMaybe.append(bits.eat(8));
        }
        item.numCustom = bits.eat(4);
        if (item.numCustom > 0) {
            qWarning() << "We don't know what num custom is, we have" << item.numCustom;
        }
    }

    if (!itemFailed) {
        if (bits.m_bits.count() > 7 || bits.m_bits.count('1') > 0) {
            qWarning() << "There should be only zero padding left, we have" << bits.m_bits;
        }
    }

    item.remainingBits = bits.m_bits;

    return item;
}

std::string Savegame::serializeItem(const InventoryItem &item)
{
    BitParser bits;
    bits.put(128, 8);
    bits.put(item.version, 7);
    putAspect(item.balance,"InventoryBalanceData", item.version, &bits);
    putAspect(item.data,"InventoryData", item.version, &bits);
    putAspect(item.manufacturer,"ManufacturerData", item.version, &bits);
    bits.put(item.level, 7);
    bits.put(item.parts.count(), 6);

    QString itemPartCategory = ItemData::partCategory(item.balance.val.toLower());
    for (const InventoryItem::Aspect &part : item.parts) {
        putAspect(part, itemPartCategory, item.version, &bits);
    }

    bits.put(item.genericParts.count(), 4);
    for (const InventoryItem::Aspect &genericPart : item.genericParts) {
        putAspect(genericPart, itemPartCategory, item.version, &bits);
    }

    bits.put(item.itemWearMaybe.count(), 8);
    for (const uint8_t itemWear : item.itemWearMaybe) {
        bits.put(itemWear, 8);
    }

    bits.m_bits.append(item.remainingBits);
    return obfuscateItem(bits.toBinaryData(), item.seed).toStdString();
}

InventoryItem::Aspect Savegame::getAspect(const QString &category, const int requiredVersion, BitParser *bits)
{
    InventoryItem::Aspect aspect;
    aspect.bits = ItemData::requiredBits(category, requiredVersion);
    if (aspect.bits <= 0) {
        qWarning() << "Invalid aspect";
        return {};
    }
    aspect.index = bits->eat(aspect.bits);
    if (aspect.index < 0) {
        qWarning() << "Invalid index" << aspect.index;
        return {};
    }
    if (aspect.index == 0) { // it is for some weird reason 1-indexed
        qWarning() << "Zero index for" << category;
        return {};
    }
    aspect.val = ItemData::getItemAsset(category, aspect.index - 1);
    if (aspect.val.isEmpty()) {
        qWarning() << "Can't find val for" << category << aspect.index;
        return {};
    }

    return aspect;
}

void Savegame::putAspect(const InventoryItem::Aspect &aspect, const QString &category, const int requiredVersion, BitParser *bits)
{
    bits->put(aspect.index, ItemData::requiredBits(category, requiredVersion));
}

bool Savegame::save(const QString filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "Failed to open file", file.errorString());
        return false;
    }
    file.write("GVAS");
    bool couldWriteHeader =
            writeInt(m_header.savegameVersion, &file) &&
            writeInt(m_header.packageVersion, &file) &&
            writeInt(m_header.engineMajorVersion, &file) &&
            writeInt(m_header.engineMinorVersion, &file) &&
            writeInt(m_header.enginePatchVersion, &file) &&
            writeInt(m_header.engineBuild, &file) &&
            writeString(m_header.buildId, &file) &&
            writeInt(m_header.customFormatVersion, &file) &&
            writeInt(m_header.customFormatCount, &file);

    if (!couldWriteHeader) {
        QMessageBox::warning(nullptr, "Invalid header", "Failed to write header to file:\n" + file.errorString());
        return false;
    }

    for (const Header::CustomFormat &format : m_header.customFormats) {
        file.write(format.id.toRfc4122());
        writeInt(format.entry, &file);
    }
    writeString(m_header.savegameType, &file);

    QByteArray data = QByteArray::fromStdString(m_character->SerializeAsString());

    char *dataRaw = data.data();
    for (int i=0; i<data.size(); i++) {
        // I want a prize for ugly code
        // Premature optimization^Wobfuscation (it's probably not faster than doing it the pretty way)
        dataRaw[i] ^= (i < int(sizeof(obfuscation::prefixMask)) ? obfuscation::prefixMask[i] : dataRaw[i - sizeof(obfuscation::prefixMask)])
            ^ obfuscation::xorMask[i % sizeof(obfuscation::xorMask)];
    }
    qDebug() << file.pos();

    writeInt(data.length(), &file);
    file.write(data);

    return true;
}

void Savegame::addInventoryItemPart(const int index, const InventoryItem::Aspect &part)
{
    qDebug() << "Adding" << part.val;

    m_items[index].parts.append(part);
    m_character->mutable_inventory_items(index)->set_item_serial_number(serializeItem(m_items[index]));
}

void Savegame::removeInventoryItemPart(const int index, const QString partId)
{
    qDebug() << "Trying to find" << partId;
    QMutableVectorIterator<InventoryItem::Aspect> it(m_items[index].parts);
    while(it.hasNext()) {
        if (it.next().val.endsWith(partId)) {
            qDebug() << " >>>>>>>>>>>>>>>>>>> Removing" << it.value().val;
            it.remove();
        }
    }
//    m_items[index].parts.remove(partIndex);
    m_character->mutable_inventory_items(index)->set_item_serial_number(serializeItem(m_items[index]));
}

void Savegame::setItemLevel(const int index, const int newLevel)
{
    if (index < 0 || index >= m_items.count()) {
        qWarning() << "item index out of range" << index;
        return;
    }
    if (newLevel < Constants::minLevel || newLevel > Constants::maxLevel) {
        qWarning() << "Level out of range" << newLevel;
        return;
    }
    m_items[index].level = newLevel;

    m_character->mutable_inventory_items(index)->set_item_serial_number(serializeItem(m_items[index]));
}

int Savegame::ammoAmount(const QString &name) const
{
    // fuckings to protobuf
    const std::string ammoId = ("/Game/GameData/Weapons/Ammo/Resource_Ammo_" + name + ".Resource_Ammo_" + name).toStdString();

    for (const OakSave::ResourcePoolSavegameData &pool : m_character->resource_pools()) {
        if (pool.resource_path() != ammoId) {
            continue;
        }
        return pool.amount();
    }
    qWarning() << "FAiled to find" << name;
    return -1;
}

void Savegame::setAmmoAmount(const QString &name, const int amount)
{
    // fuckings to protobuf
    const std::string ammoId = ("/Game/GameData/Weapons/Ammo/Resource_Ammo_" + name + ".Resource_Ammo_" + name).toStdString();

    for (OakSave::ResourcePoolSavegameData &pool : *m_character->mutable_resource_pools()) {
        if (pool.resource_path() != ammoId) {
            continue;
        }
        pool.set_amount(amount);
        return;
    }

    qWarning() << "FAiled to find" << name;
}

int Savegame::sduAmount(const QString &name) const
{
    // fuckings to protobuf
    const std::string sduId = ("/Game/Pickups/SDU/SDU_" + name + ".SDU_" + name).toStdString();

    for (const OakSave::OakSDUSaveGameData &sdu : m_character->sdu_list()) {
        if (sdu.sdu_data_path() != sduId) {
            continue;
        }
        return sdu.sdu_level();
    }
    qDebug() << "No SDU" << name;
    return -1;
}

void Savegame::setSduAmount(const QString &name, const int amount)
{
    // fuckings to protobuf
    const std::string sduId = ("/Game/Pickups/SDU/SDU_" + name + ".SDU_" + name).toStdString();

    for (OakSave::OakSDUSaveGameData &sdu : *m_character->mutable_sdu_list()) {
        qDebug() << "sdu path:" << QString::fromStdString(sdu.sdu_data_path());
        if (sdu.sdu_data_path() != sduId) {
            continue;
        }
        sdu.set_sdu_level(amount);
        return;
    }

    qWarning() << "FAiled to find" << name;
}

QStringList Savegame::activeMissions() const
{
    QStringList ret;

    for (const OakSave::MissionPlaythroughSaveGameData &playthrough : m_character->mission_playthroughs_data()) {
        for (const OakSave::MissionStatusPlayerSaveGameData &mission : playthrough.mission_list()) {
            if (mission.status() != OakSave::MissionStatusPlayerSaveGameData_MissionState_MS_Active) {
                continue;
            }
            ret.append(QString::fromStdString(mission.mission_class_path()));
        }
    }

    return ret;
}

// not horribly efficient, but I don't want to expose the rest to the crappy protobuf API, it's bad enough as it is
QVector<bool> Savegame::objectivesCompleted(const QString &missionID)
{
    const std::string protobufSucks = missionID.toStdString();

    QVector<bool> ret;

    for (const OakSave::MissionPlaythroughSaveGameData &playthrough : m_character->mission_playthroughs_data()) {
        for (const OakSave::MissionStatusPlayerSaveGameData &mission : playthrough.mission_list()) {
            if (mission.mission_class_path() != protobufSucks) {
                continue;
            }

            for (const int32_t objectiveState : mission.objectives_progress()) {
                switch(objectiveState) {
                case 0:
                    ret.append(false);
                    break;
                case 1:
                    ret.append(true);
                    break;

                    // dunno what these are
                case 30:
                case 512:
                case 17408:
                default:
                    qWarning() << "Unknown objective state" << objectiveState << "for mission" << missionID;
                    break;
                }

            }
        }
    }

    return ret;
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

int Savegame::money() const
{
    return currencyAmount(Constants::Currency::Money);
}

void Savegame::setMoney(const int amount)
{
    if (amount == money()) {
        return;
    }

    setCurrency(Constants::Currency::Money, amount);
    emit moneyChanged(amount);
}

int Savegame::eridium() const
{
    return currencyAmount(Constants::Currency::Eridium);
}

void Savegame::setEridium(const int amount)
{
    if (amount == eridium()) {
        return;
    }

    setCurrency(Constants::Currency::Eridium, amount);
    emit eridiumChanged(amount);
}

void Savegame::regenerateUuid()
{
    m_character->set_save_game_guid(QUuid::createUuid().toString(QUuid::Id128).toUpper().toStdString());
    emit uuidChanged(QString::fromStdString(m_character->save_game_guid()));
}

void Savegame::setSaveSlot(const int slotId)
{
    m_character->set_save_game_id(slotId);
    emit saveSlotChanged(m_character->save_game_id());
}

int Savegame::currencyAmount(const Constants::Currency currency) const
{
    for (const OakSave::InventoryCategorySaveData &item : m_character->inventory_category_list()) {
        if (Constants::currencyByHash(item.base_category_definition_hash()) != currency) {
            continue;
        }
        return item.quantity();
    }
    qWarning() << "Failed to find category" << currency;

    return 0;

}

void Savegame::setCurrency(const Constants::Currency currency, const int amount)
{
    for (OakSave::InventoryCategorySaveData &item : *m_character->mutable_inventory_category_list()) {
        if (Constants::currencyByHash(item.base_category_definition_hash()) != currency) {
            continue;
        }
        item.set_quantity(amount);
    }

    qDebug() << "Failed to find category, adding new";
    int hash = Constants::hashByCurrency(currency);
    if (hash == -1) {
        qWarning() << "Failed to find hash for" << currency;
        return;
    }
    OakSave::InventoryCategorySaveData newItem;
    newItem.set_base_category_definition_hash(hash);
    newItem.set_quantity(amount);
    m_character->mutable_inventory_category_list()->Add(std::move(newItem));
}
