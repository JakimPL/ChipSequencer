#include <iostream>

#include "../../general.hpp"
#include "manager.hpp"

void LinkManager::set_link(Link &link, void *item, const uint8_t i) const {
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
}

void LinkManager::set_links() const {
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

void LinkManager::realign_links(const size_t index, const Target target, const ItemType type) const {
    size_t link_type = static_cast<size_t>(type);
    for (Link &link : links[link_type]) {
        if (link.target == target && link.index >= index) {
            link.index = std::max(0, link.index - 1);
        }
    }
}

void LinkManager::realign_links(const size_t index, const Target target) const {
    realign_links(index, target, ItemType::CHANNEL);
    realign_links(index, target, ItemType::DSP);
}
