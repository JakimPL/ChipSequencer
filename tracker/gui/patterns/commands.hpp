#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <utility>

#include "../../song/output.hpp"
#include "../../structures/commands/sequence.hpp"
#include "../init.hpp"
#include "../inputs/string.hpp"

enum class CommandSelection {
    None,
    Command,
    Value,
};

using CommandValue = std::pair<std::string, std::string>;

struct CommandsPattern {
    CommandsPattern();
    explicit CommandsPattern(uint8_t sequence_index);

    size_t id = 0;
    int starting_row = 0;
    int current_row = -1;
    CommandSelection selection = CommandSelection::None;
    uint8_t sequence_index = -1;
    int steps = 0;
    std::vector<std::string> commands = {};
    std::vector<std::string> values = {};
    std::vector<uint8_t> durations = {};
    std::vector<uint8_t> indices = {};

    StringInputHandler commands_handler;
    StringInputHandler values_handler;

    static double cast_portamento_to_double(uint16_t value);
    static void split_portamento_value(const std::string &command_value, uint8_t &channel, uint16_t &value);
    static void split_change_value_parts(
        const std::string &command_value,
        TargetVariableType &target_variable_type,
        Target &target,
        uint8_t &index,
        uint16_t &offset,
        uint32_t &value
    );

    static std::string from_portamento(uint8_t channel, uint16_t value);
    static std::string from_portamento(uint8_t channel, double value);
    static std::string from_gainer(uint16_t value);
    static std::string from_gainer(double value);
    static std::string from_change_value(TargetVariableType type, LinkKey key, uint32_t value);
    static std::string from_output_type(const OutputType &output_type, const int &value_integer, const float &value_float);
    LinkKey get_command_key(const CommandChangeValue *command) const;

    void from_sequence(uint8_t sequence_index);
    std::vector<Command> to_command_vector() const;
    void to_buffer(size_t sequence_index) const;
    void save_links(size_t sequence_index) const;
    int calculate_playing_row(size_t channel_index);

    void clear_row(int row);
    bool is_row_valid(int row) const;
    CommandValue get_command(int row) const;
    void set_command(int row, const std::string &command, const std::string &value);
    void set_command(int row, const CommandValue &command_value);
    void handle_input();
    void set_selection(int row, CommandSelection item);
    void add_command(const std::string &command = "", const std::string &value = "");
};
