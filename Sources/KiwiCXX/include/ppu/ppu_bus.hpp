#pragma once

#include <cstdlib>
#include <vector>

#include "mappers/mapper.hpp"

class PictureBus {

private:
    /// the VRAM on the picture bus
    std::vector<std::uint8_t> ram;
    /// indexes where they start in RAM vector
    // std::size_t NameTable0, NameTable1, NameTable2, NameTable3;
    std::size_t name_tables[4] = { 0, 0, 0, 0 };
    /// the palette for decoding RGB tuples
    std::vector<std::uint8_t> palette;
    /// a pointer to the mapper on the cartridge
    Mapper* mapper;

public:
    /// Initialize a new picture bus.
    PictureBus() : ram(0x800), palette(0x20), mapper(nullptr) { };;

    /// Read a byte from an address on the VRAM.
    ///
    /// @param address the 16-bit address of the byte to read in the VRAM
    ///
    /// @return the byte located at the given address
    ///
    std::uint8_t read(std::uint16_t address);

    /// Write a byte to an address in the VRAM.
    ///
    /// @param address the 16-bit address to write the byte to in VRAM
    /// @param value the byte to write to the given address
    ///
    void write(std::uint16_t address, std::uint8_t value);

    /// Set the mapper pointer to a new value.
    ///
    /// @param mapper the new mapper pointer for the bus to use
    ///
    inline void set_mapper(Mapper* mapper) { this->mapper = mapper; update_mirroring(); };

    /// Read a color index from the palette.
    ///
    /// @param address the address of the palette color
    ///
    /// @return the index of the RGB tuple in the color array
    ///
    inline std::uint8_t read_palette(std::uint8_t address) { return palette[address]; };

    /// Update the mirroring and name table from the mapper.
    void update_mirroring();

};