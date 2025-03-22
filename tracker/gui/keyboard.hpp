#ifndef GUI_KEYBOARD_HPP
#define GUI_KEYBOARD_HPP

#include <unordered_map>
#include <string>

const std::unordered_map<char, std::string> key_mapping = {
    {'Z', "C-0"},
    {'S', "C#0"},
    {'X', "D-0"},
    {'D', "D#0"},
    {'C', "E-0"},
    {'V', "F-0"},
    {'G', "F#0"},
    {'B', "G-0"},
    {'H', "G#0"},
    {'N', "A-0"},
    {'J', "A#0"},
    {'M', "B-0"},
    {'Q', "C-1"},
    {'2', "C#1"},
    {'W', "D-1"},
    {'3', "D#1"},
    {'E', "E-1"},
    {'R', "F-1"},
    {'5', "F#1"},
    {'T', "G-1"},
    {'6', "G#1"},
    {'Y', "A-1"},
    {'7', "A#1"},
    {'U', "B-1"},
    {'I', "C-2"},
    {'9', "C#2"},
    {'O', "D-2"},
    {'0', "D#2"},
    {'P', "E-2"},
    {'[', "F-2"},
    {'=', "F#2"},
    {']', "G-2"}
};

#endif // GUI_KEYBOARD_HPP
