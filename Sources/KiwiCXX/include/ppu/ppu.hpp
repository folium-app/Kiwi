#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "ppu/ppu_bus.hpp"

/// The number of visible scan lines (i.e., the height of the screen)
const int VISIBLE_SCANLINES = 240;
/// The number of visible dots per scan line (i.e., the width of the screen)
const int SCANLINE_VISIBLE_DOTS = 256;
/// The number of cycles per scanline
const int SCANLINE_CYCLE_LENGTH = 341;
/// The last cycle of a scan line
const int SCANLINE_END_CYCLE = 341;
/// The last scanline per frame
const int FRAME_END_SCANLINE = 261;

/// The Picture Processing Unit (PPU) for the NES
class PPU {

private:
    /// The callback to fire when entering vertical blanking mode
    std::function<void(void)> vblank_callback;
    /// The OAM memory (sprites)
    std::vector<std::uint8_t> sprite_memory;
    /// OAM memory (sprites) for the next scanline
    std::vector<std::uint8_t> scanline_sprites;

    /// The current pipeline state of the PPU
    enum State {
        PRE_RENDER,
        RENDER,
        POST_RENDER,
        VERTICAL_BLANK
    } pipeline_state;

    /// The number of cycles left in the frame
    int cycles;
    /// the current scanline of the frame
    int scanline;
    /// whether the PPU is on an even frame
    bool is_even_frame;

    // Status

    /// whether the PPU is in vertical blanking mode
    bool is_vblank;
    /// whether sprite 0 has been hit (i.e., collision detection)
    bool is_sprite_zero_hit;

    // Registers

    /// the current data address to (read / write) (from / to)
    std::uint16_t data_address;
    /// a temporary address register
    std::uint16_t temp_address;
    /// the fine scrolling position
    std::uint8_t fine_x_scroll;
    /// TODO: doc
    bool is_first_write;
    /// The address of the data buffer
    std::uint8_t data_buffer;
    /// the read / write address for the OAM memory (sprites)
    std::uint8_t sprite_data_address;

    // Mask

    /// whether the PPU is showing sprites
    bool is_showing_sprites;
    /// whether the PPU is showing background pixels
    bool is_showing_background;
    /// whether the PPU is hiding sprites along the edges
    bool is_hiding_edge_sprites;
    /// whether the PPU is hiding the background along the edges
    bool is_hiding_edge_background;

    // Setup flags and variables

    /// TODO: doc
    bool is_long_sprites;
    /// whether the PPU is in the interrupt handler
    bool is_interrupting;

    /// TODO: doc
    enum CharacterPage {
        LOW,
        HIGH,
    } background_page, sprite_page;

    /// The value to increment the data address by
    std::uint16_t data_address_increment;

    /// The internal screen data structure as a vector representation of a
    /// matrix of height matching the visible scans lines and width matching
    /// the number of visible scan line dots
    std::uint32_t screen[VISIBLE_SCANLINES][SCANLINE_VISIBLE_DOTS];

public:
    /// Initialize a new PPU.
    PPU() : sprite_memory(64 * 4) { };

    /// Perform a single cycle on the PPU.
    void cycle(PictureBus& bus);

    /// Reset the PPU.
    void reset();

    /// Set the interrupt callback for the CPU.
    inline void set_interrupt_callback(std::function<void(void)> cb) { vblank_callback = cb; };

    /// TODO: doc
    void do_DMA(const std::uint8_t* page_ptr);

    // MARK: Callbacks mapped to CPU address space

    /// Set the control register to a new value.
    ///
    /// @param ctrl the new control register byte
    ///
    void control(std::uint8_t ctrl);

    /// Set the mask register to a new value.
    ///
    /// @param mask the new mask value
    ///
    void set_mask(std::uint8_t mask);

    /// Set the scroll register to a new value.
    ///
    /// @param scroll the new scroll register value
    ///
    void set_scroll(std::uint8_t scroll);

    /// Return the value in the PPU status register.
    std::uint8_t get_status();

    /// TODO: doc
    void set_data_address(std::uint8_t address);

    /// Read data off the picture bus.
    ///
    /// @param bus the bus to read data off of
    ///
    std::uint8_t get_data(PictureBus& bus);

    /// TODO: doc
    void set_data(PictureBus& bus, std::uint8_t data);

    /// Set the sprite data address to a new value.
    ///
    /// @param address the new OAM data address
    ///
    inline void set_OAM_address(std::uint8_t address) { sprite_data_address = address; };

    /// Read a byte from OAM memory at the sprite data address.
    ///
    /// @return the byte at the given address in OAM memory
    ///
    inline std::uint8_t get_OAM_data() { return sprite_memory[sprite_data_address]; };

    /// Write a byte to OAM memory at the sprite data address.
    ///
    /// @param value the byte to write to the given address
    ///
    inline void set_OAM_data(std::uint8_t value) { sprite_memory[sprite_data_address++] = value; };

    /// Return a pointer to the screen buffer.
    inline std::uint32_t* get_screen_buffer() { return *screen; };

};
