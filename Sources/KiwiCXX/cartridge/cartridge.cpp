#include <fstream>

#include "cartridge/cartridge.hpp"

void Cartridge::loadFromFile(std::string path) {
    // create a stream to load the ROM file
    std::ifstream romFile(path, std::ios_base::binary | std::ios_base::in);
    // create a byte vector for the iNES header
    std::vector<std::uint8_t> header;
    header.resize(0x10);
    romFile.read(reinterpret_cast<char*>(&header[0]), 0x10);
    // read internal data
    name_table_mirroring = header[6] & 0xB;
    mapper_number = ((header[6] >> 4) & 0xf) | (header[7] & 0xf0);

    has_extended_ram = header[6] & 0x2;
    // read PRG-ROM 16KB banks
    std::uint8_t banks = header[4];
    prg_rom.resize(0x4000 * banks);
    romFile.read(reinterpret_cast<char*>(&prg_rom[0]), 0x4000 * banks);
    // read CHR-ROM 8KB banks
    std::uint8_t vbanks = header[5];
    if (!vbanks)
        return;
    chr_rom.resize(0x2000 * vbanks);
    romFile.read(reinterpret_cast<char*>(&chr_rom[0]), 0x2000 * vbanks);
}

void Cartridge::loadFromData(uint32_t* data) {
    // create a byte vector for the iNES header
    std::vector<std::uint8_t> header;
    header.resize(0x10);
    memcpy((char*)&header[0], data, 0x10);
    // romFile.read(reinterpret_cast<char*>(&header[0]), 0x10);
    // read internal data
    name_table_mirroring = header[6] & 0xB;
    mapper_number = ((header[6] >> 4) & 0xf) | (header[7] & 0xf0);

    has_extended_ram = header[6] & 0x2;
    // read PRG-ROM 16KB banks
    std::uint8_t banks = header[4];
    prg_rom.resize(0x4000 * banks);
    memcpy((char*)&prg_rom[0], data, 0x4000 * banks);
    // romFile.read(reinterpret_cast<char*>(&prg_rom[0]), 0x4000 * banks);
    // read CHR-ROM 8KB banks
    std::uint8_t vbanks = header[5];
    if (!vbanks)
        return;
    chr_rom.resize(0x2000 * vbanks);
    memcpy((char*)&chr_rom[0], data, 0x2000 * vbanks);
    // romFile.read(reinterpret_cast<char*>(&chr_rom[0]), 0x2000 * vbanks);
}
