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

bool LinkManager::is_linked(const LinkKey key) const {
    const auto it = map.find(key);
    return it != map.end() && !it->second.empty();
}

std::vector<Link *> LinkManager::get_links(const LinkKey key) const {
    const auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }

    return {};
}

void LinkManager::remove_key(Link &link) {
    const auto map_it = map.find(link.key);
    if (map_it != map.end()) {
        std::vector<Link *> &links_vector = map_it->second;
        auto vec_it = std::find(links_vector.begin(), links_vector.end(), &link);
        if (vec_it != links_vector.end()) {
            links_vector.erase(vec_it);
        }

        if (links_vector.empty()) {
            map.erase(map_it);
        }
    }

    const LinkKey key;
    link.key = key;
}

void LinkManager::assign_key(Link &link) {
    const LinkKey key = {link.target, link.index, link.offset};
    link.key = key;
    map[key].push_back(&link);
}

void LinkManager::validate_key_and_link(const LinkKey key, const Link *link) const {
    if (key.target == Target::UNUSED) {
        return;
    }
    if (link == nullptr) {
        throw std::runtime_error("Link is null");
    }
    if (link->pointer == nullptr) {
        throw std::runtime_error("Link pointer is null");
    }
    if (key.target != link->target) {
        throw std::runtime_error("Link target does not match key target");
    }
    if (key.offset != link->offset) {
        throw std::runtime_error("Link offset does not match key target");
    }
}

TargetVariableType LinkManager::get_type(const LinkKey key) const {
    switch (key.target) {
    case Target::OUTPUT_CHANNEL: {
        return TargetVariableType::Float;
    }
    case Target::DSP_CHANNEL: {
        return TargetVariableType::Float;
    }
    default: {
        const size_t index = routing_variables.at(key.target).offset_to_index.at(key.offset);
        return routing_variables.at(key.target).types[index];
    }
    }
}

void LinkManager::capture_parameter(const LinkKey key, const Link *link) {
    validate_key_and_link(key, link);
    const TargetVariableType type = get_type(key);
    switch (type) {
    case TargetVariableType::Int8:
        snapshot[key] = *reinterpret_cast<uint8_t *>(link->pointer);
        break;
    case TargetVariableType::Int16:
        snapshot[key] = *reinterpret_cast<uint16_t *>(link->pointer);
        break;
    case TargetVariableType::Int32:
        snapshot[key] = *reinterpret_cast<uint32_t *>(link->pointer);
        break;
    case TargetVariableType::Float:
        snapshot[key] = *reinterpret_cast<_Float32 *>(link->pointer);
        break;
    default:
        throw std::runtime_error("Unknown target variable type");
    }
}

void LinkManager::capture_parameters() {
    snapshot.clear();
    for (const auto &pair : map) {
        const LinkKey &key = pair.first;
        const std::vector<Link *> &links = pair.second;
        for (const Link *link : links) {
            capture_parameter(key, link);
        }
    }
}

void LinkManager::restore_parameter(const LinkKey key, const Link *link) const {
    validate_key_and_link(key, link);
    const TargetVariableType type = get_type(key);
    switch (type) {
    case TargetVariableType::Int8:
        *reinterpret_cast<uint8_t *>(link->pointer) = std::get<uint8_t>(snapshot.at(key));
        break;
    case TargetVariableType::Int16:
        *reinterpret_cast<uint16_t *>(link->pointer) = std::get<uint16_t>(snapshot.at(key));
        break;
    case TargetVariableType::Int32:
        *reinterpret_cast<uint32_t *>(link->pointer) = std::get<uint32_t>(snapshot.at(key));
        break;
    case TargetVariableType::Float:
        *reinterpret_cast<_Float32 *>(link->pointer) = std::get<_Float32>(snapshot.at(key));
        break;
    default:
        throw std::runtime_error("Unknown target variable type");
    }
}

void LinkManager::restore_parameters() const {
    for (const auto &pair : map) {
        const LinkKey &key = pair.first;
        const std::vector<Link *> &links = pair.second;
        for (const Link *link : links) {
            restore_parameter(key, link);
        }
    }
}
