#include "emulator.hpp"

Emulator::Emulator(std::string rom_path) {
    // set the read callbacks
    bus.set_read_callback(PPUSTATUS, [&](void) {return ppu.get_status(); });
    bus.set_read_callback(PPUDATA, [&](void) {return ppu.get_data(picture_bus); });
    bus.set_read_callback(JOY1, [&](void) {return controllers[0].read(); });
    bus.set_read_callback(JOY2, [&](void) {return controllers[1].read(); });
    bus.set_read_callback(OAMDATA, [&](void) {return ppu.get_OAM_data(); });

    // set the write callbacks
    bus.set_write_callback(PPUCTRL, [&](std::uint8_t b) {ppu.control(b); });
    bus.set_write_callback(PPUMASK, [&](std::uint8_t b) {ppu.set_mask(b); });
    bus.set_write_callback(OAMADDR, [&](std::uint8_t b) {ppu.set_OAM_address(b); });
    bus.set_write_callback(PPUADDR, [&](std::uint8_t b) {ppu.set_data_address(b); });
    bus.set_write_callback(PPUSCROL, [&](std::uint8_t b) {ppu.set_scroll(b); });
    bus.set_write_callback(PPUDATA, [&](std::uint8_t b) {ppu.set_data(picture_bus, b); });
    bus.set_write_callback(OAMDMA, [&](std::uint8_t b) {DMA(b); });
    bus.set_write_callback(JOY1, [&](std::uint8_t b) {controllers[0].strobe(b); controllers[1].strobe(b); });
    bus.set_write_callback(OAMDATA, [&](std::uint8_t b) {ppu.set_OAM_data(b); });

    // set the interrupt callback for the PPU
    ppu.set_interrupt_callback([&]() { cpu.interrupt(bus, CPU::NMI_INTERRUPT); });
    // load the ROM from disk, expect that the Python code has validated it
    cartridge.loadFromFile(rom_path);
    // create the mapper based on the mapper ID in the iNES header of the ROM
    Mapper* mapper(Mapper::create(cartridge, [&]() { picture_bus.update_mirroring(); }, [&]() { cpu.interrupt(bus, CPU::InterruptType::IRQ_INTERRUPT); }));
    // give the IO buses a pointer to the mapper
    bus.set_mapper(mapper);
    picture_bus.set_mapper(mapper);
}

Emulator::Emulator(uint32_t* rom_data) {
    // set the read callbacks
    bus.set_read_callback(PPUSTATUS, [&](void) {return ppu.get_status(); });
    bus.set_read_callback(PPUDATA, [&](void) {return ppu.get_data(picture_bus); });
    bus.set_read_callback(JOY1, [&](void) {return controllers[0].read(); });
    bus.set_read_callback(JOY2, [&](void) {return controllers[1].read(); });
    bus.set_read_callback(OAMDATA, [&](void) {return ppu.get_OAM_data(); });

    // set the write callbacks
    bus.set_write_callback(PPUCTRL, [&](std::uint8_t b) {ppu.control(b); });
    bus.set_write_callback(PPUMASK, [&](std::uint8_t b) {ppu.set_mask(b); });
    bus.set_write_callback(OAMADDR, [&](std::uint8_t b) {ppu.set_OAM_address(b); });
    bus.set_write_callback(PPUADDR, [&](std::uint8_t b) {ppu.set_data_address(b); });
    bus.set_write_callback(PPUSCROL, [&](std::uint8_t b) {ppu.set_scroll(b); });
    bus.set_write_callback(PPUDATA, [&](std::uint8_t b) {ppu.set_data(picture_bus, b); });
    bus.set_write_callback(OAMDMA, [&](std::uint8_t b) {DMA(b); });
    bus.set_write_callback(JOY1, [&](std::uint8_t b) {controllers[0].strobe(b); controllers[1].strobe(b); });
    bus.set_write_callback(OAMDATA, [&](std::uint8_t b) {ppu.set_OAM_data(b); });

    // set the interrupt callback for the PPU
    ppu.set_interrupt_callback([&]() { cpu.interrupt(bus, CPU::NMI_INTERRUPT); });
    // load the ROM from disk, expect that the Python code has validated it
    cartridge.loadFromData(rom_data);
    // create the mapper based on the mapper ID in the iNES header of the ROM
    Mapper* mapper(Mapper::create(cartridge, [&]() { picture_bus.update_mirroring(); }, [&]() { cpu.interrupt(bus, CPU::InterruptType::IRQ_INTERRUPT); }));
    // give the IO buses a pointer to the mapper
    bus.set_mapper(mapper);
    picture_bus.set_mapper(mapper);
}

void Emulator::DMA(std::uint8_t page) {
    // skip the DMA cycles on the CPU
    cpu.skip_DMA_cycles();
    // do the DMA page change on the PPU
    ppu.do_DMA(bus.get_page_pointer(page));
}

void Emulator::step() {
    // render a single frame on the emulator
    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
        // 3 PPU steps per CPU step
        ppu.cycle(picture_bus);
        ppu.cycle(picture_bus);
        ppu.cycle(picture_bus);
        cpu.cycle(bus);
    }
}

void Emulator::backup() {
    backup_bus = bus;
    backup_picture_bus = picture_bus;
    backup_cpu = cpu;
    backup_ppu = ppu;
}

void Emulator::restore() {
    bus = backup_bus;
    picture_bus = backup_picture_bus;
    cpu = backup_cpu;
    ppu = backup_ppu;
}
