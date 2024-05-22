#pragma once

#include "bus/bus.hpp"
#include "cartridge/cartridge.hpp"
#include "controller/controller.hpp"
#include "cpu/cpu.hpp"
#include "ppu/ppu.hpp"
#include "ppu/ppu_bus.hpp"

#include <xbrz/xbrz.h>

class Emulator {

private:
    /// The number of cycles in 1 frame
    const static int CYCLES_PER_FRAME = 29781;

    /// the virtual cartridge with ROM and mapper data
    Cartridge cartridge;
    /// the 2 controllers on the emulator
    Controller controllers[2];

    /// the main data bus of the emulator
    MainBus bus;
    /// the picture bus from the PPU of the emulator
    PictureBus picture_bus;
    /// The emulator's CPU
    CPU cpu;
    /// the emulators' PPU
    PPU ppu;

    /// the main data bus of the emulator
    MainBus backup_bus;
    /// the picture bus from the PPU of the emulator
    PictureBus backup_picture_bus;
    /// The emulator's CPU
    CPU backup_cpu;
    /// the emulators' PPU
    PPU backup_ppu;

    /// Skip DMA cycle and perform a DMA copy.
    void DMA(std::uint8_t page);

public:
    /// The width of the NES screen in pixels
    const static int WIDTH = SCANLINE_VISIBLE_DOTS;
    /// The height of the NES screen in pixels
    const static int HEIGHT = VISIBLE_SCANLINES;

    /// Initialize a new emulator with a path to a ROM file.
    ///
    /// @param rom_path the path to the ROM for the emulator to run
    ///
    Emulator(std::string rom_path);
    Emulator(uint32_t* rom_data);

    /// Return a 32-bit pointer to the screen buffer's first address.
    ///
    /// @return a 32-bit pointer to the screen buffer's first address
    ///
    inline std::uint32_t* get_screen_buffer() {
        // static std::vector<uint32_t> trgt(256 * 240);
        // xbrz::scale(xbrz::SCALE_FACTOR_MAX, ppu.get_screen_buffer(), trgt.data(), WIDTH, HEIGHT, xbrz::ColorFormat::ARGB);
        return ppu.get_screen_buffer();
        
        // return ppu.get_screen_buffer();
    };

    /// Return a 8-bit pointer to the RAM buffer's first address.
    ///
    /// @return a 8-bit pointer to the RAM buffer's first address
    ///
    inline std::uint8_t* get_memory_buffer() { return bus.get_memory_buffer(); };

    /// Return a pointer to a controller port
    ///
    /// @param port the port of the controller to return the pointer to
    /// @return a pointer to the byte buffer for the controller state
    ///
    inline std::uint8_t* get_controller(int port) { return controllers[port].get_joypad_buffer(); };

    /// Load the ROM into the NES.
    inline void reset() { cpu.reset(bus); ppu.reset(); };

    /// Perform a step on the emulator, i.e., a single frame.
    void step();

    /// Create a backup state on the emulator.
    void backup();

    /// Restore the backup state on the emulator.
    void restore();

};
