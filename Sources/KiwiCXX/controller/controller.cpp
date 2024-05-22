#include "controller/controller.hpp"

void Controller::strobe(std::uint8_t b) {
    is_strobe = (b & 1);
    if (!is_strobe)
        joypad_bits = joypad_buttons;
}

std::uint8_t Controller::read() {
    std::uint8_t ret;
    if (is_strobe)
        ret = (joypad_buttons & 1);
    else {
        ret = (joypad_bits & 1);
        joypad_bits >>= 1;
    }
    return ret | 0x40;
}