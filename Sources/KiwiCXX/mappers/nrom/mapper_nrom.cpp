#include "mappers/nrom/mapper_nrom.hpp"

MapperNROM::MapperNROM(Cartridge& cart) : Mapper(cart) {
    if (cart.getROM().size() == 0x4000) // 1 bank
        is_one_bank = true;
    else // 2 banks
        is_one_bank = false;

    if (cart.getVROM().size() == 0) {
        has_character_ram = true;
        character_ram.resize(0x2000);
    }
    else
        has_character_ram = false;
}

std::uint8_t MapperNROM::readPRG(std::uint16_t address) {
    if (!is_one_bank)
        return cartridge.getROM()[address - 0x8000];
    else //mirrored
        return cartridge.getROM()[(address - 0x8000) & 0x3fff];
}

void MapperNROM::writePRG(std::uint16_t address, std::uint8_t value) {

}

std::uint8_t MapperNROM::readCHR(std::uint16_t address) {
    if (has_character_ram)
        return character_ram[address];
    else
        return cartridge.getVROM()[address];
}

void MapperNROM::writeCHR(std::uint16_t address, std::uint8_t value) {
    if (has_character_ram)
        character_ram[address] = value;
    else
        return;
}

const std::uint8_t* MapperNROM::getPagePtr(std::uint16_t address) {
    if (!is_one_bank)
        return &cartridge.getROM()[address - 0x8000];
    else //mirrored
        return &cartridge.getROM()[(address - 0x8000) & 0x3fff];
}