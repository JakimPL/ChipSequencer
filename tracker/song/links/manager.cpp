#include <iostream>

#include "../../general.hpp"
#include "manager.hpp"

void LinkManager::set_link(Link &link, void *item, const uint8_t i) {
    remove_key(link);

    switch (link.target) {
    case Target::OUTPUT_CHANNEL:
        link.base = &output;
        break;
    case Target::DSP_CHANNEL:
        link.base = &dsp_input;
        break;
    case Target::ENVELOPE:
        if (link.index >= envelopes.size()) {
            link.base = &output;
        } else {
            link.base = envelopes[link.index];
        }
        break;
    case Target::SEQUENCE:
        if (link.index >= sequences.size()) {
            link.base = &output;
        } else {
            link.base = sequences[link.index];
        }
        break;
    case Target::ORDER:
        if (link.index >= orders.size()) {
            link.base = &output;
        } else {
            link.base = orders[link.index];
        }
        break;
    case Target::OSCILLATOR:
        if (link.index >= oscillators.size()) {
            link.base = &output;
        } else {
            link.base = oscillators[link.index];
        }
        break;
    case Target::WAVETABLE:
        if (link.index >= wavetables.size()) {
            link.base = &output;
        } else {
            link.base = wavetables[link.index];
        }
        break;
    case Target::DSP:
        if (link.index >= dsps.size()) {
            link.base = &output;
        } else {
            link.base = dsps[link.index];
        }
        break;
    case Target::CHANNEL:
        if (link.index >= channels.size()) {
            link.base = &output;
        } else {
            link.base = channels[link.index];
        }
        break;
    }

    link.id = i;
    link.item = item;
    link.pointer = link.base + link.offset;
    link.assign_output();

    assign_key(link);
}

void LinkManager::set_links() {
    size_t link_type = static_cast<size_t>(ItemType::CHANNEL);
    for (size_t i = 0; i < channels.size(); i++) {
        Link &link = links[link_type][i];
        void *item = channels[i];
        try {
            set_link(link, item, i);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error setting link for a channel " << i << ": " << exception.what() << std::endl;
        }
    }

    link_type = static_cast<size_t>(ItemType::DSP);
    for (size_t i = 0; i < dsps.size(); i++) {
        Link &link = links[link_type][i];
        void *item = dsps[i];
        try {
            set_link(link, item, i);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error setting link for a DSP " << i << ": " << exception.what() << std::endl;
        }
    }
}

void LinkManager::realign_links(const size_t index, const Target target, const ItemType type) {
    size_t link_type = static_cast<size_t>(type);
    for (Link &link : links[link_type]) {
        if (link.target == target && link.index >= index) {
            remove_key(link);
            link.index = std::max(0, link.index - 1);
            assign_key(link);
        }
    }
}

void LinkManager::realign_links(const size_t index, const Target target) {
    realign_links(index, target, ItemType::CHANNEL);
    realign_links(index, target, ItemType::DSP);
}

bool LinkManager::is_linked(const Target target, const size_t index, const uint16_t offset) const {
    const LinkKey key = {target, index, offset};
    return map.find(key) != map.end();
}

void LinkManager::remove_key(Link &link) {
    auto it = map.find(link.key);
    if (it != map.end()) {
        map.erase(it);
    }

    const LinkKey key;
    link.key = key;
}

void LinkManager::assign_key(Link &link) {
    const LinkKey key = {link.target, link.index, link.offset};
    link.key = key;
    map[key] = &link;
}
