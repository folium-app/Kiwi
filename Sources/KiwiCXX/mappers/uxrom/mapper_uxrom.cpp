#include "mappers/uxrom/mapper_uxrom.hpp"

MapperUxROM::MapperUxROM(Cartridge& cart) :
    Mapper(cart),
    select_prg(0) {
    if (cart.getVROM().size() == 0) {
        has_character_ram = true;
        character_ram.resize(0x2000);
    }
    else
        has_character_ram = false;

    // last - 16KB
    last_bank_pointer = &cart.getROM()[cart.getROM().size() - 0x4000];
}

std::uint8_t MapperUxROM::readPRG(std::uint16_t address) {
    if (address < 0xc000)
        return cartridge.getROM()[((address - 0x8000) & 0x3fff) | (select_prg << 14)];
    else
        return *(last_bank_pointer + (address & 0x3fff));
}

const std::uint8_t* MapperUxROM::getPagePtr(std::uint16_t address) {
    if (address < 0xc000)
        return &cartridge.getROM()[((address - 0x8000) & 0x3fff) | (select_prg << 14)];
    else
        return last_bank_pointer + (address & 0x3fff);
}

std::uint8_t MapperUxROM::readCHR(std::uint16_t address) {
    if (has_character_ram)
        return character_ram[address];
    else
        return cartridge.getVROM()[address];
}

void MapperUxROM::writeCHR(std::uint16_t address, std::uint8_t value) {
    if (has_character_ram)
        character_ram[address] = value;
    else
        return;
}