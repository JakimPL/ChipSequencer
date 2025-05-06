#include "input.hpp"

InputHandler::InputHandler(int &index)
    : index(index) {
}

void InputHandler::set_limit(const int max) {
    limit = max;
}

void InputHandler::clear() {
    buffer.clear();
}

std::string InputHandler::get_buffer() const {
    return buffer;
}

void InputHandler::set_buffer(const std::string &value) {
    buffer = value;
}
