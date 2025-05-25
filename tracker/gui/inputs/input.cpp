#include "input.hpp"

void InputHandler::set_limit(const int max) {
    limit = max;
}

void InputHandler::clear(const bool capture) {
    buffer.clear();
    capture_input = capture;
}

std::string InputHandler::get_buffer() const {
    return buffer;
}

void InputHandler::set_buffer(const std::string &value) {
    buffer = value;
}
