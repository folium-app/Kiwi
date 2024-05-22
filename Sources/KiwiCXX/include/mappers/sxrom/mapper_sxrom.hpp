#pragma once

#include "mappers/mapper.hpp"

class MapperSxROM : public Mapper { // MMC1

private:
    /// The mirroring callback on the PPU
    std::function<void(void)> mirroring_callback;
    /// the mirroring mode on the device
    NameTableMirroring mirroing;
    /// whether the cartridge uses character RAM
    bool has_character_ram;
    /// the mode for CHR ROM
    int mode_chr;
    /// the mode for PRG ROM
    int mode_prg;
    /// a temporary register
    std::uint8_t temp_register;
    /// a write counter
    int write_counter;
    /// the PRG register
    std::uint8_t register_prg;
    /// The first CHR register
    std::uint8_t register_chr0;
    /// The second CHR register
    std::uint8_t register_chr1;
    /// The first PRG bank
    const std::uint8_t* first_bank_prg;
    /// The second PRG bank
    const std::uint8_t* second_bank_prg;
    /// The first CHR bank
    const std::uint8_t* first_bank_chr;
    /// The second CHR bank
    const std::uint8_t* second_bank_chr;
    /// The character RAM on the cartridge
    std::vector<std::uint8_t> character_ram;

    /// TODO: what does this do
    void calculatePRGPointers();

public:
    /// Create a new mapper with a cartridge.
    ///
    /// @param cart a reference to a cartridge for the mapper to access
    /// @param mirroring_cb the callback to change mirroring modes on the PPU
    ///
    MapperSxROM(Cartridge& cart, std::function<void(void)> mirroring_cb);

    /// Read a byte from the PRG RAM.
    ///
    /// @param address the 16-bit address of the byte to read
    /// @return the byte located at the given address in PRG RAM
    ///
    std::uint8_t readPRG(std::uint16_t address);

    /// Write a byte to an address in the PRG RAM.
    ///
    /// @param address the 16-bit address to write to
    /// @param value the byte to write to the given address
    ///
    void writePRG(std::uint16_t address, std::uint8_t value);

    /// Read a byte from the CHR RAM.
    ///
    /// @param address the 16-bit address of the byte to read
    /// @return the byte located at the given address in CHR RAM
    ///
    std::uint8_t readCHR(std::uint16_t address);

    /// Write a byte to an address in the CHR RAM.
    ///
    /// @param address the 16-bit address to write to
    /// @param value the byte to write to the given address
    ///
    void writeCHR(std::uint16_t address, std::uint8_t value);

    /// Return the page pointer for the given address.
    ///
    /// @param address the address of the page pointer to get
    /// @return the page pointer at the given address
    ///
    const std::uint8_t* getPagePtr(std::uint16_t address);

    /// Return the name table mirroring mode of this mapper.
    inline NameTableMirroring getNameTableMirroring() { return mirroing; };

};