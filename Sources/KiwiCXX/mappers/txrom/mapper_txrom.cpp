#include "mappers/txrom/mapper_txrom.hpp"

MapperTXROM::MapperTXROM(Cartridge& cart, std::function<void(void)> mirroring_cb, std::function<void(void)> interrupt_cb) : Mapper(cart), mirroring_callback(mirroring_cb), interrupt_cb(interrupt_cb), prg_ram(32 * 1024), mirroring_ram(4 * 1024) {
    prg_bank0 = &cart.getROM()[cart.getROM().size() - 0x4000];
    prg_bank1 = &cart.getROM()[cart.getROM().size() - 0x2000];
    prg_bank2 = &cart.getROM()[cart.getROM().size() - 0x4000];
    prg_bank3 = &cart.getROM()[cart.getROM().size() - 0x2000];

    for (auto& chr_bank : chr_banks)
        chr_bank = cart.getVROM().size() - 0x400;

    chr_banks[0] = cart.getVROM().size() - 0x800;
    chr_banks[3] = cart.getVROM().size() - 0x800;
};

std::uint8_t MapperTXROM::readCHR(std::uint16_t address) {
    if (address < 0x1FFF) {
        const auto bankSelect = address >> 10;
        // get the configured base address for the bank
        const auto baseAddress = chr_banks[bankSelect];
        const auto offset = address & 0x3ff;
        return cartridge.getVROM()[baseAddress + offset];
    }
    else if (address <= 0x2FFF) {
        return mirroring_ram[address - 0x2000];
    }

    return 0x00;
};

std::uint8_t MapperTXROM::readPRG(std::uint16_t address) {
    switch (address) {
    case 0x6000 ... 0x7FFF:
        return prg_ram[address & 0x1FFF];
    case 0x8000 ... 0x9FFF:
        return *(prg_bank0 + (address & 0x1FFF));
    case 0xA000 ... 0xBFFF:
        return *(prg_bank1 + (address & 0x1FFF));
    case 0xC000 ... 0xDFFF:
        return *(prg_bank2 + (address & 0x1FFF));
    case 0xE000 ... 0xFFFF:
        return *(prg_bank3 + (address & 0x1FFF));
    }

    return 0x00;
};

void MapperTXROM::writeCHR(std::uint16_t address, std::uint8_t value) {
    if (address >= 0x2000 && address <= 0x2FFF) {
        mirroring_ram[address - 0x2000] = value;
    }
};

void MapperTXROM::writePRG(std::uint16_t address, std::uint8_t value) {
    if (address >= 0x6000 && address <= 0x7FFF) {
        prg_ram[address & 0x1FFF] = value;
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        if (!(address & 0x01)) {
            target_register = value & 0x7;
            prg_bank_mode = value & 0x40;
            chr_inversion = value & 0x80;
        } else {
            bank_register[target_register] = value;
            
            if (chr_inversion == 0) {
                chr_banks[0] = (bank_register[0] & 0xFE) * 0x0400;
                chr_banks[1] = (bank_register[0] & 0xFE) * 0x0400 + 0x0400;
                chr_banks[2] = (bank_register[1] & 0xFE) * 0x0400;
                chr_banks[3] = (bank_register[1] & 0xFE) * 0x0400 + 0x0400;
                chr_banks[4] = bank_register[2] * 0x0400;
                chr_banks[5] = bank_register[3] * 0x0400;
                chr_banks[6] = bank_register[4] * 0x0400;
                chr_banks[7] = bank_register[5] * 0x0400;
            } else if (chr_inversion == 1) {
                chr_banks[0] = bank_register[2] * 0x0400;
                chr_banks[1] = bank_register[3] * 0x0400;
                chr_banks[2] = bank_register[4] * 0x0400;
                chr_banks[3] = bank_register[5] * 0x0400;
                chr_banks[4] = (bank_register[0] & 0xFE) * 0x0400;
                chr_banks[5] = (bank_register[0] & 0xFE) * 0x0400 + 0x0400;
                chr_banks[6] = (bank_register[1] & 0xFE) * 0x0400;
                chr_banks[7] = (bank_register[1] & 0xFE) * 0x0400 + 0x0400;
            }

            if (prg_bank_mode == 0) {
                prg_bank0 = &cartridge.getROM()[(bank_register[6] & 0x3F) * 0x2000];
                prg_bank1 = &cartridge.getROM()[(bank_register[7] & 0x3F) * 0x2000];
                prg_bank2 = &cartridge.getROM()[cartridge.getROM().size() - 0x4000];
                prg_bank3 = &cartridge.getROM()[cartridge.getROM().size() - 0x2000];
            } else if (prg_bank_mode == 1) {
                prg_bank0 = &cartridge.getROM()[cartridge.getROM().size() - 0x4000];
                prg_bank1 = &cartridge.getROM()[(bank_register[7] & 0x3F) * 0x2000];
                prg_bank2 = &cartridge.getROM()[(bank_register[6] & 0x3F) * 0x2000];
                prg_bank3 = &cartridge.getROM()[cartridge.getROM().size() - 0x2000];
            }
        }
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        if (!(address & 0x01)) {
            if (cartridge.getNameTableMirroring() & 0x8) {
                mirroring = NameTableMirroring::FOUR_SCREEN;
            } else if (value & 0x01) {
                mirroring = NameTableMirroring::HORIZONTAL;
            } else {
                mirroring = NameTableMirroring::VERTICAL;
            }
            
            mirroring_callback();
        } else {
            // PRG Ram Protect
        }
    } else if (address >= 0xC000 && address <= 0xDFFF) {
        if (!(address & 0x01)) {
            irq_latch = value;
        } else {
            irq_count = 0;
            irq_pending = true;
        }
    } else if (address >= 0xE000) {
        irq_enabled = (address & 0x01) == 0x01;
    }
};

const std::uint8_t* MapperTXROM::getPagePtr(std::uint16_t address) {
    /*
    if (address < 0xc000)
        return (prg_bank0 + (address & 0x3fff));
    else
        return (prg_bank1 + (address & 0x3fff));
        */

    return nullptr;
};
