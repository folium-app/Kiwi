#pragma once

#include "mappers/mapper.hpp"

class MapperUxROM : public Mapper {

private:
    /// whether the cartridge use character RAM
    bool has_character_ram;
    /// the pointer to the last bank
    const std::uint8_t* last_bank_pointer;
    /// TODO: what is this?
    std::uint16_t select_prg;
    /// The character RAM on the mapper
    std::vector<std::uint8_t> character_ram;

public:
    /// Create a new mapper with a cartridge.
    ///
    /// @param cart a reference to a cartridge for the mapper to access
    ///
    MapperUxROM(Cartridge& cart);

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
    inline void writePRG(std::uint16_t address, std::uint8_t value) { select_prg = value; };

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

};