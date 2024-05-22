#include "mappers/cnrom/mapper_cnrom.hpp"

MapperCNROM::MapperCNROM(Cartridge& cart) : Mapper(cart), select_chr(0) {
    is_one_bank = cart.getROM().size() == 0x4000;
};

std::uint8_t MapperCNROM::readPRG(std::uint16_t address) {
    if (!is_one_bank)
        return cartridge.getROM()[address - 0x8000];
    // mirrored
    else
        return cartridge.getROM()[(address - 0x8000) & 0x3fff];
}

const std::uint8_t* MapperCNROM::getPagePtr(std::uint16_t address) {
    if (!is_one_bank)
        return &cartridge.getROM()[address - 0x8000];
    // mirrored
    else
        return &cartridge.getROM()[(address - 0x8000) & 0x3fff];
}

void MapperCNROM::writeCHR(std::uint16_t address, std::uint8_t value) {

}