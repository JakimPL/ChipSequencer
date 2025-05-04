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
