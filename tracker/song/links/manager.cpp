#include <iostream>
#include <map>
#include <sstream>

#include "../../general.hpp"
#include "manager.hpp"

void LinkManager::reset() {
    links[static_cast<size_t>(ItemType::CHANNEL)].clear();
    links[static_cast<size_t>(ItemType::DSP)].clear();
    links[static_cast<size_t>(ItemType::COMMANDS)].clear();
    commands_links.clear();
}

void LinkManager::set_link(Link &link, void *item, const uint8_t i) {
    remove_key(link);

    switch (link.target) {
    case Target::SPLITTER_OUTPUT:
    case Target::DIRECT_OUTPUT: {
        link.base = &output;
        break;
    }
    case Target::SPLITTER_DSP:
    case Target::DIRECT_DSP: {
        link.base = &dsp_input;
        break;
    }
    case Target::ENVELOPE: {
        if (link.index >= envelopes.size()) {
            link.base = &output;
        } else {
            link.base = envelopes[link.index];
        }
        break;
    }
    case Target::SEQUENCE: {
        if (link.index >= sequences.size()) {
            link.base = &output;
        } else {
            link.base = sequences[link.index];
        }
        break;
    }
    case Target::COMMANDS_SEQUENCE: {
        if (link.index >= commands_sequences.size()) {
            link.base = &output;
        } else {
            link.base = commands_sequences[link.index];
        }
        break;
    }
    case Target::ORDER: {
        if (link.index >= orders.size()) {
            link.base = &output;
        } else {
            link.base = orders[link.index];
        }
        break;
    }
    case Target::OSCILLATOR: {
        if (link.index >= oscillators.size()) {
            link.base = &output;
        } else {
            link.base = oscillators[link.index];
        }
        break;
    }
    case Target::WAVETABLE: {
        if (link.index >= wavetables.size()) {
            link.base = &output;
        } else {
            link.base = wavetables[link.index];
        }
        break;
    }
    case Target::DSP: {
        if (link.index >= dsps.size()) {
            link.base = &output;
        } else {
            link.base = dsps[link.index];
        }
        break;
    }
    case Target::CHANNEL: {
        if (link.index >= channels.size()) {
            link.base = &output;
        } else {
            link.base = channels[link.index];
        }
        break;
    }
    case Target::COMMANDS_CHANNEL: {
        if (link.index >= commands_channels.size()) {
            link.base = &output;
        } else {
            link.base = commands_channels[link.index];
        }
        break;
    }
    case Target::COUNT:
    default: {
        throw std::runtime_error("Invalid link target " + std::to_string(static_cast<int>(link.target)));
    }
    }

    link.id = i;
    link.item = item;
    link.pointer = link.base + link.offset;
    assign_key(link);
}

void LinkManager::set_channels_links() {
    const size_t link_type = static_cast<size_t>(ItemType::CHANNEL);
    for (size_t i = 0; i < channels.size(); i++) {
        Link &link = links[link_type][i];
        void *item = channels[i];
        try {
            set_link(link, item, i);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error setting link for a channel " << i << ": "
                      << exception.what() << std::endl;
        }
    }
}

void LinkManager::set_dsps_links() {
    const size_t link_type = static_cast<size_t>(ItemType::DSP);
    for (size_t i = 0; i < dsps.size(); i++) {
        Link &link = links[link_type][i];
        void *item = dsps[i];
        try {
            set_link(link, item, i);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error setting link for a DSP " << i << ": "
                      << exception.what() << std::endl;
        }
    }
}

void LinkManager::set_commands_links() {
    const size_t link_type = static_cast<size_t>(ItemType::COMMANDS);
    links[link_type].clear();
    for (size_t index = 0; index < commands_sequences.size(); index++) {
        for (auto &[element, link] : commands_links[index]) {
            const uint16_t i = (index << 8) + element;
            try {
                set_link(link, nullptr, i);
            } catch (const std::out_of_range &exception) {
                std::cerr << "Error setting link for a commands sequence ("
                          << index << ", " << element << "): "
                          << exception.what() << std::endl;
            }

            links[link_type].push_back(link);
        }
    }
}

void LinkManager::set_links() {
    clear();
    set_channels_links();
    set_dsps_links();
    set_commands_links();
    save_targets();
}

void LinkManager::save_targets() {
    pointers_map.clear();
    targets = {nullptr};
    targets[0] = &(output[0]);

    std::map<void *, size_t> index_map;
    void *output_pointer = &(output[0]);
    const LinkKey output_key = {Target::DIRECT_OUTPUT, 0, 0};
    pointers_map.push_back({output_pointer, output_key});
    index_map[output_pointer] = 0;

    size_t size = 1;
    for (const ItemType type : {ItemType::CHANNEL, ItemType::DSP, ItemType::COMMANDS}) {
        for (Link &link : links[static_cast<size_t>(type)]) {
            if (link.target == Target::COUNT) {
                continue;
            }

            const auto it = index_map.find(link.pointer);
            if (it == index_map.end()) {
                targets[size] = link.pointer;
                link.table_id = size;
                index_map[link.pointer] = link.table_id;
                pointers_map.push_back({link.pointer, link.key});
                size++;
            } else {
                link.table_id = it->second;
            }

            link.assign_output();
        }
    }
}

void LinkManager::realign_links(const Target target, const size_t index, const ItemType type) {
    size_t link_type = static_cast<size_t>(type);
    for (Link &link : links[link_type]) {
        if (link.target == target && link.index >= index) {
            remove_key(link);
            link.index = std::max(0, static_cast<int>(link.index) - 1);
            assign_key(link);
        }
    }
}

void LinkManager::realign_links(const Target target, const size_t index) {
    remove_dependencies(target, index);
    realign_links(target, index, ItemType::CHANNEL);
    realign_links(target, index, ItemType::DSP);
    realign_links(target, index, ItemType::COMMANDS);
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

std::string LinkManager::get_link_reference(const LinkKey key) const {
    std::string reference;
    switch (key.target) {
    case Target::SPLITTER_OUTPUT:
    case Target::DIRECT_OUTPUT: {
        reference = "output";
        break;
    }
    case Target::SPLITTER_DSP:
    case Target::DIRECT_DSP: {
        reference = "dsp_input";
        break;
    }
    case Target::ENVELOPE: {
        reference = "envelopes.envelope_" + std::to_string(key.index);
        break;
    }
    case Target::SEQUENCE: {
        reference = "sequences.sequence_" + std::to_string(key.index);
        break;
    }
    case Target::COMMANDS_SEQUENCE: {
        reference = "commands_sequences.commands_sequence_" + std::to_string(key.index);
        break;
    }
    case Target::ORDER: {
        reference = "orders.order_" + std::to_string(key.index);
        break;
    }
    case Target::OSCILLATOR: {
        reference = "oscillators.oscillator_" + std::to_string(key.index);
        break;
    }
    case Target::WAVETABLE: {
        reference = "wavetables.wavetable_" + std::to_string(key.index);
        break;
    }
    case Target::DSP: {
        reference = "dsps.dsp_" + std::to_string(key.index);
        break;
    }
    case Target::CHANNEL: {
        reference = "channels.channel_" + std::to_string(key.index);
        break;
    }
    case Target::COMMANDS_CHANNEL: {
        reference = "commands_channels.commands_sequence_" + std::to_string(key.index);
        break;
    }
    case Target::COUNT:
    default: {
        throw std::runtime_error("Invalid link target: " + std::to_string(static_cast<int>(key.target)));
    }
    }

    if (key.offset != 0) {
        reference += " + " + std::to_string(key.offset);
    }

    return reference;
}

std::string LinkManager::get_link_reference(const ItemType type, const size_t index) const {
    std::string reference;
    const Link &link = links[static_cast<size_t>(type)][index];
    return get_link_reference(link.key);
}

std::vector<std::pair<void *, LinkKey>> LinkManager::get_pointers_map() const {
    return pointers_map;
}

std::pair<void *, LinkKey> LinkManager::get_pointer_and_key(const size_t index) const {
    return pointers_map.at(index);
}

size_t LinkManager::find_pointer_id_by_key(const LinkKey key) const {
    size_t id = 0;
    for (const auto &[pointer, link_key] : pointers_map) {
        if (link_key == key) {
            return id;
        }
        id++;
    }

    return 0;
}

std::vector<std::pair<ItemType, uint8_t>> LinkManager::find_dependencies(const Target target, const size_t index) const {
    std::set<std::pair<ItemType, uint8_t>> dependencies;
    for (const auto &array : links) {
        for (const Link &link : array) {
            if (link.target == target && link.index == index) {
                dependencies.insert({link.type, link.id});
            }
        }
    }

    return std::vector<std::pair<ItemType, uint8_t>>(dependencies.begin(), dependencies.end());
}

void LinkManager::remove_dependencies(const Target target, const size_t index) {
    const std::vector<std::pair<ItemType, uint8_t>> dependencies = find_dependencies(target, index);
    for (const auto &[type, id] : dependencies) {
        size_t link_type = static_cast<size_t>(type);
        Link &link = links[link_type][id];
        link.target = Target::SPLITTER_OUTPUT;
        link.index = 0;
        link.offset = 0;
        void *item = nullptr;
        switch (type) {
        case ItemType::CHANNEL: {
            item = channels[id];
            break;
        }
        case ItemType::DSP: {
            item = dsps[id];
            break;
        }
        case ItemType::COMMANDS:
        case ItemType::COUNT:
        default: {
            break;
        }
        }
        set_link(link, item, id);
    }
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
    const LinkKey key = {link.target, static_cast<int>(link.index), link.offset};
    link.key = key;
    map[key].push_back(&link);
}

void LinkManager::validate_key_and_link(const LinkKey key, const Link *link) const {
    if (key.target == Target::COUNT) {
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
    case Target::SPLITTER_OUTPUT:
    case Target::SPLITTER_DSP:
    case Target::DIRECT_OUTPUT:
    case Target::DIRECT_DSP: {
        return TargetVariableType::Float;
    }
    case Target::ENVELOPE:
    case Target::SEQUENCE:
    case Target::COMMANDS_CHANNEL:
    case Target::COMMANDS_SEQUENCE:
    case Target::ORDER:
    case Target::OSCILLATOR:
    case Target::WAVETABLE:
    case Target::DSP:
    case Target::CHANNEL: {
        try {
            const size_t index = routing_variables.at(key.target).get_index_from_offset(key);
            return routing_variables.at(key.target).types[index];
        } catch (const std::out_of_range &exception) {
            return TargetVariableType::Byte;
        }
    }
    case Target::COUNT:
    default: {
        throw std::runtime_error("Invalid target type: " + std::to_string(static_cast<int>(key.target)));
    }
    }

    throw std::runtime_error("Invalid target type: " + std::to_string(static_cast<int>(key.target)));
}

void LinkManager::capture_parameter(const LinkKey key, const Link *link) {
    validate_key_and_link(key, link);
    const TargetVariableType type = get_type(key);
    switch (type) {
    case TargetVariableType::Byte:
        snapshot[key] = *reinterpret_cast<uint8_t *>(link->pointer);
        break;
    case TargetVariableType::Word:
        snapshot[key] = *reinterpret_cast<uint16_t *>(link->pointer);
        break;
    case TargetVariableType::Dword:
        snapshot[key] = *reinterpret_cast<uint32_t *>(link->pointer);
        break;
    case TargetVariableType::Float:
        snapshot[key] = *reinterpret_cast<_Float32 *>(link->pointer);
        break;
    case TargetVariableType::Count:
    default:
        throw std::runtime_error("Unknown target variable type: " + std::to_string(static_cast<int>(type)));
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
    case TargetVariableType::Byte:
        *reinterpret_cast<uint8_t *>(link->pointer) = std::get<uint8_t>(snapshot.at(key));
        break;
    case TargetVariableType::Word:
        *reinterpret_cast<uint16_t *>(link->pointer) = std::get<uint16_t>(snapshot.at(key));
        break;
    case TargetVariableType::Dword:
        *reinterpret_cast<uint32_t *>(link->pointer) = std::get<uint32_t>(snapshot.at(key));
        break;
    case TargetVariableType::Float:
        *reinterpret_cast<_Float32 *>(link->pointer) = std::get<_Float32>(snapshot.at(key));
        break;
    case TargetVariableType::Count:
    default:
        throw std::runtime_error("Unknown target variable type: " + std::to_string(static_cast<int>(type)));
    }
}

void LinkManager::restore_parameters() const {
    for (const auto &pair : map) {
        const LinkKey &key = pair.first;
        const std::vector<Link *> &links = pair.second;
        for (const Link *link : links) {
            try {
                restore_parameter(key, link);
            } catch (const std::out_of_range &exception) {
                std::cerr << "Error restoring parameter for a key " << static_cast<int>(key.target)
                          << ", " << key.index << " link: " << exception.what() << std::endl;
            }
        }
    }
}

void LinkManager::clear() {
    map.clear();
    snapshot.clear();
}

std::pair<uint8_t, uint8_t> LinkManager::unpack_command_id(const uint16_t id) {
    return {static_cast<uint8_t>(id >> 8), static_cast<uint8_t>(id & 0xFF)};
}
