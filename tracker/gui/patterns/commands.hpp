#ifndef GUI_PATTERNS_COMMANDS_HPP
#define GUI_PATTERNS_COMMANDS_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "../../song/output.hpp"
#include "../../structures/commands/sequence.hpp"
#include "../init.hpp"
#include "../inputs/string.hpp"

enum class CommandSelection {
    None,
    Command,
    Value,
};

struct CommandsPattern {
    CommandsPattern();
    CommandsPattern(const uint8_t sequence_index);

    CommandSelection selection = CommandSelection::None;
    uint8_t sequence_index = -1;
    int current_row = -1;
    int steps = 0;
    std::vector<std::string> commands = {};
    std::vector<std::string> values = {};
    std::vector<uint8_t> durations = {};
    std::vector<uint8_t> indices = {};

    StringInputHandler commands_handler;
    StringInputHandler values_handler;

    static double cast_portamento_to_double(const uint16_t value);
    static void split_portamento_value(const std::string &command_value, uint8_t &channel, uint16_t &value);
    static void split_change_value_parts(
        const std::string &command_value,
        TargetVariableType &target_variable_type,
        Target &target,
        uint8_t &index,
        uint16_t &offset,
        uint32_t &value
    );

    static std::string from_portamento(const uint8_t channel, const uint16_t value);
    static std::string from_portamento(const uint8_t channel, const double value);
    static std::string from_gainer(const uint16_t value);
    static std::string from_gainer(const double value);
    static std::string from_change_value(const TargetVariableType type, const LinkKey key, const uint32_t value);
    static std::string from_output_type(const OutputType &output_type, const int &value_integer, const float &value_float);
    LinkKey get_command_key(const CommandChangeValue *command) const;

    void from_sequence(const uint8_t sequence_index);
    std::vector<Command> to_command_vector() const;
    void to_buffer(const size_t sequence_index) const;
    void save_links(size_t sequence_index) const;
    int calculate_playing_row(size_t channel_index);

    void handle_input(const int min_row = 0, const int max_row = -1);
    void set_selection(const int row, const CommandSelection item);
    void add_command(const std::string &command = "", const std::string &value = "");
};

#endif // GUI_PATTERNS_COMMANDS_HPP
