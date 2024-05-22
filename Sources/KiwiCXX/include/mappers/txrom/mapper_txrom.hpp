#pragma once

#include <array>

#include "mappers/mapper.hpp"

class MapperTXROM : public Mapper {

private:
    std::function<void(void)> mirroring_callback;
    std::function<void(void)> interrupt_cb;
    NameTableMirroring mirroring = NameTableMirroring::HORIZONTAL;
public:
    MapperTXROM(Cartridge& cart, std::function<void(void)> mirroring_cb, std::function<void(void)> interrupt_cb);

    const std::uint8_t* prg_bank0, *prg_bank1, *prg_bank2, *prg_bank3;
    std::array<std::uint32_t, 8> chr_banks;

    uint32_t target_register = 0;
    bool prg_bank_mode = false;
    bool chr_inversion = false;
    std::uint32_t bank_register[8]{};

    std::vector<std::uint8_t> prg_ram, mirroring_ram;

    bool irq_enabled = false, irq_pending = false;
    std::uint8_t irq_count = 0, irq_latch = 0;

    std::uint8_t readCHR(std::uint16_t address);
    std::uint8_t readPRG(std::uint16_t address);

    void writeCHR(std::uint16_t address, std::uint8_t value);
    void writePRG(std::uint16_t address, std::uint8_t value);

    const std::uint8_t* getPagePtr(std::uint16_t address);
    inline NameTableMirroring getNameTableMirroring() { return mirroring; };
};