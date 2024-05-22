#include "bus/bus.hpp"

std::uint8_t MainBus::read(std::uint16_t address) {
    if (address < 0x2000)
        return ram[address & 0x7ff];
    else if (address < 0x4020) {
        // PPU registers, mirrored
        if (address < 0x4000) {
            auto it = read_callbacks.find(static_cast<IORegisters>(address & 0x2007));
            if (it != read_callbacks.end())
                return (it->second)();
            // Second object is the pointer to the function object
            // Dereference the function pointer and call it
            else
                return 0x00;
        }
        // only *some* IO registers
        else if (address < 0x4018 && address >= 0x4014) {
            auto it = read_callbacks.find(static_cast<IORegisters>(address));
            if (it != read_callbacks.end())
                return (it->second)();
            // Second object is the pointer to the function object
            // Dereference the function pointer and call it
            else
                return 0x00;
        }
        else {
            return 0x00;
        }
    }
    else if (address < 0x6000) {
        return 0x00;
    }
    else if (address < 0x8000) {
        if (mapper->hasExtendedRAM()) {
            return extended_ram[address - 0x6000];
        }
    }
    else {
        return mapper->readPRG(address);
    }
    return 0;
}

void MainBus::write(std::uint16_t address, std::uint8_t value) {
    if (address < 0x2000)
        ram[address & 0x7ff] = value;
    else if (address < 0x4020) {
        //PPU registers, mirrored
        if (address < 0x4000) {
            auto it = write_callbacks.find(static_cast<IORegisters>(address & 0x2007));
            if (it != write_callbacks.end())
                (it->second)(value);
            //Second object is the pointer to the function object
            //Dereference the function pointer and call it
            else
                return;
        }
        //only some registers
        else if (address < 0x4017 && address >= 0x4014) {
            auto it = write_callbacks.find(static_cast<IORegisters>(address));
            if (it != write_callbacks.end())
                (it->second)(value);
            //Second object is the pointer to the function object
            //Dereference the function pointer and call it
            else
                return;
        }
        else {
            return;
        }
    }
    else if (address < 0x6000) {
        return;
    }
    else if (address < 0x8000) {
        if (mapper->hasExtendedRAM()) {
            extended_ram[address - 0x6000] = value;
        }
    }
    else {
        mapper->writePRG(address, value);
    }
}

const std::uint8_t* MainBus::get_page_pointer(std::uint8_t page) {
    std::uint16_t address = page << 8;
    if (address < 0x2000)
        return &ram[address & 0x7ff];
    else if (address < 0x4020)
        return nullptr;
    else if (address < 0x6000)
        return nullptr;
    else if (address < 0x8000)
        if (mapper->hasExtendedRAM())
            return &extended_ram[address - 0x6000];

    return nullptr;
}

void MainBus::set_mapper(Mapper* mapper) {
    this->mapper = mapper;
    if (mapper->hasExtendedRAM())
        extended_ram.resize(0x2000);
}