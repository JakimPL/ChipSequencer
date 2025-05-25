#include "input.hpp"

void InputHandler::set_limit(const int max) {
    limit = max;
}

void InputHandler::clear() {
    buffer.clear();
    capture_input = true;
}

std::string InputHandler::get_buffer() const {
    return buffer;
}

void InputHandler::set_buffer(const std::string &value) {
    buffer = value;
}
