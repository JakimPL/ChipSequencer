#ifndef SONG_LINK_HPP
#define SONG_LINK_HPP

#include <cstdint>
#include <map>
#include <vector>

enum class Target : uint8_t {
    OUTPUT_CHANNEL,
    DSP_CHANNEL,
    ENVELOPE,
    SEQUENCE,
    ORDER,
    OSCILLATOR,
    WAVETABLE,
    DSP,
    CHANNEL
};

enum class ItemType : u_int8_t {
    CHANNEL = 0,
    DSP = 1
};

struct Link {
    ItemType type;
    uint8_t id;
    Target target;
    uint8_t index;
    uint16_t offset;
    void *item = nullptr;
    void *base = nullptr;
    void *pointer = nullptr;

    void assign_output();
    void serialize(std::ofstream &file) const;
};

typedef std::vector<std::vector<Link>> Links;

#endif // SONG_LINK_HPP
