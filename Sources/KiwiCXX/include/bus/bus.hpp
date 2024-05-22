#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "mappers/mapper.hpp"

enum IORegisters {
    PPUCTRL = 0x2000,
    PPUMASK,
    PPUSTATUS,
    OAMADDR,
    OAMDATA,
    PPUSCROL,
    PPUADDR,
    PPUDATA,
    OAMDMA = 0x4014,
    JOY1 = 0x4016,
    JOY2 = 0x4017,
};

/// The main bus for data to travel along the NES hardware
class MainBus {

private:
    /// The RAM on the main bus
    std::vector<std::uint8_t> ram;
    /// The extended RAM (if the mapper has extended RAM)
    std::vector<std::uint8_t> extended_ram;
    /// a pointer to the mapper on the cartridge
    Mapper* mapper;
    /// a map of IO registers to callback methods for writes
    std::map<IORegisters, std::function<void(std::uint8_t)>> write_callbacks;
    /// a map of IO registers to callback methods for reads
    std::map<IORegisters, std::function<std::uint8_t(void)>> read_callbacks;

public:
    /// Initialize a new main bus.
    MainBus() : ram(0x800, 0), mapper(nullptr) {};

    /// Return a 8-bit pointer to the RAM buffer's first address.
    ///
    /// @return a 8-bit pointer to the RAM buffer's first address
    ///
    inline std::uint8_t* get_memory_buffer() { return &ram.front(); };

    /// Read a byte from an address on the RAM.
    ///
    /// @param address the 16-bit address of the byte to read in the RAM
    ///
    /// @return the byte located at the given address
    ///
    std::uint8_t read(std::uint16_t address);

    /// Write a byte to an address in the RAM.
    ///
    /// @param address the 16-bit address to write the byte to in RAM
    /// @param value the byte to write to the given address
    ///
    void write(std::uint16_t address, std::uint8_t value);

    /// Set the mapper pointer to a new value.
    ///
    /// @param mapper the new mapper pointer for the bus to use
    ///
    void set_mapper(Mapper* mapper);

    /// Set a callback for when writes occur.
    inline void set_write_callback(IORegisters reg, std::function<void(std::uint8_t)> callback) {
        write_callbacks.emplace(reg, callback);
    };

    /// Set a callback for when reads occur.
    inline void set_read_callback(IORegisters reg, std::function<std::uint8_t(void)> callback) {
        read_callbacks.emplace(reg, callback);
    };

    /// Return a pointer to the page in memory.
    const std::uint8_t* get_page_pointer(std::uint8_t page);

};