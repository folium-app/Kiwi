#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Cartridge {

private:
    /// the PRG ROM
    std::vector<std::uint8_t> prg_rom;
    /// the CHR ROM
    std::vector<std::uint8_t> chr_rom;
    /// the name table mirroring mode
    std::uint8_t name_table_mirroring;
    /// the mapper ID number
    std::uint8_t mapper_number;
    /// whether this cartridge uses extended RAM
    bool has_extended_ram;

public:
    /// Initialize a new cartridge
    Cartridge() : name_table_mirroring(0), mapper_number(0), has_extended_ram(false) { };

    /// Return the ROM data.
    const inline std::vector<std::uint8_t>& getROM() { return prg_rom; };

    /// Return the VROM data.
    const inline std::vector<std::uint8_t>& getVROM() { return chr_rom; };

    /// Return the mapper ID number.
    inline std::uint8_t getMapper() { return mapper_number; };

    /// Return the name table mirroring mode.
    inline std::uint8_t getNameTableMirroring() { return name_table_mirroring; };

    /// Return a boolean determining whether this cartridge uses extended RAM.
    inline bool hasExtendedRAM() { return has_extended_ram; };

    /// Load a ROM file into the cartridge and build the corresponding mapper.
    void loadFromFile(std::string path);
    void loadFromData(uint32_t* path);

};
