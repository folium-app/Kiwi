#include "mappers/mapper.hpp"
#include "mappers/cnrom/mapper_cnrom.hpp"
#include "mappers/nrom/mapper_nrom.hpp"
#include "mappers/sxrom/mapper_sxrom.hpp"
#include "mappers/txrom/mapper_txrom.hpp"
#include "mappers/uxrom/mapper_uxrom.hpp"

Mapper* Mapper::create(Cartridge& game, std::function<void(void)> mirroring_cb, std::function<void(void)> interrupt_cb) {
    switch (static_cast<Mapper::Type>(game.getMapper())) {
    case NROM:
        return new MapperNROM(game);
    case SxROM:
        return new MapperSxROM(game, mirroring_cb);
    case UxROM:
        return new MapperUxROM(game);
    case CNROM:
        return new MapperCNROM(game);
    case TxROM:
        return new MapperTXROM(game, mirroring_cb, interrupt_cb);
    default:
        return nullptr;
    }
}