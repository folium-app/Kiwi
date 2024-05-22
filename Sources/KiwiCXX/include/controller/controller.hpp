#pragma once

#include <cstdint>

class Controller {

private:
    /// whether strobe is on
    bool is_strobe;
    /// the emulation of the buttons on the controller
    std::uint8_t joypad_buttons;
    /// the state of the buttons
    std::uint8_t joypad_bits;

public:
    /// Initialize a new controller.
    Controller() : is_strobe(true), joypad_buttons(0), joypad_bits(0) { };

    /// Return a pointer to the joypad buffer.
    inline std::uint8_t* get_joypad_buffer() { return &joypad_buttons; };

    /// Write buttons to the virtual controller.
    ///
    /// @param buttons the button bitmap to write to the controller
    ///
    inline void write_buttons(std::uint8_t buttons) { joypad_buttons = buttons; };

    /// Strobe the controller.
    void strobe(std::uint8_t b);

    /// Read the controller state.
    ///
    /// @return a state from the controller
    ///
    std::uint8_t read();

};