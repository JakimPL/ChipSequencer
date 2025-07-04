#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../version.hpp"
#include "../tuning/frequencies.hpp"
#include "../tuning/scale.hpp"
#include "compilation.hpp"
#include "validation.hpp"
#include "links/link.hpp"

class Song {
  private:
    struct Header {
        std::string author = DEFAULT_AUTHOR;
        std::string title = DEFAULT_TITLE;
        std::string message = DEFAULT_MESSAGE;
        std::string version = TRACKER_VERSION;
    } header;

    struct Tuning {
        uint8_t edo = DEFAULT_EDO;
        double a4_frequency = DEFAULT_A4_FREQUENCY;
    } tuning;

    uint8_t output_channels = DEFAULT_OUTPUT_CHANNELS;
    uint64_t song_length = 0;
    uint16_t max_rows = 0;

    bool clear_temp = false;

    static void generate_targets_asm(std::stringstream &asm_content);
    static void generate_header_vector(
        std::stringstream &asm_content,
        const std::string &name,
        const std::string &short_name,
        size_t size,
        char separator = '/'
    );
    std::string generate_header_asm_file() const;
    static std::string generate_data_asm_file(CompilationTarget compilation_target, char separator = '/');

    static void set_used_flags(std::stringstream &asm_content);
    nlohmann::json create_header_json() const;
    nlohmann::json import_header(const std::string &filename);
    static nlohmann::json save_gui_state();

    static std::string get_element_path(const std::string &directory, const std::string &prefix, size_t i, char separator);
    static std::string get_element_path(const std::filesystem::path &directory, const std::string &prefix, size_t i);
    void calculate_song_length();

    static void serialize_dsp(std::ofstream &file, void *dsp);
    static void *deserialize_dsp(std::ifstream &file);
    static void *deserialize_oscillator(std::ifstream &file);

    void export_all(const std::filesystem::path &directory, CompilationTarget compilation_target) const;
    static void import_all(const std::filesystem::path &directory, const nlohmann::json &json);

    void export_header_asm_file(const std::filesystem::path &directory) const;
    void export_data_asm_file(const std::filesystem::path &directory, CompilationTarget compilation_target) const;
    void export_header(const std::filesystem::path &directory) const;
    static void export_gui_state(const std::filesystem::path &directory);
    static void export_lock_registry(const std::filesystem::path &directory);

    template <typename T>
    static void export_series(const std::filesystem::path &directory, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes);

    template <typename T>
    static void export_arrays(const std::filesystem::path &directory, const std::string &prefix, const std::vector<T> &arrays);

    static void export_channels(const std::filesystem::path &directory);
    static void export_dsps(const std::filesystem::path &directory);
    static void export_commands_sequences(const std::filesystem::path &directory);
    static void export_links(const std::filesystem::path &filename);

    static void import_channels(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_dsps(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_links(const std::filesystem::path &directory, const nlohmann::json &json);

    static void import_envelopes(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_sequences(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_orders(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_wavetables(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_oscillators(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_commands_sequences(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_commands_channels(const std::filesystem::path &directory, const nlohmann::json &json);
    static void import_gui_state(const std::filesystem::path &directory);
    static void import_lock_registry(const std::filesystem::path &directory);

    static void compile_sources(const std::string &directory, const std::string &filename, CompilationScheme scheme, const std::string &platform = "linux");
    static void compress_directory(const std::string &directory, const std::string &output_file);
    static void decompress_archive(const std::string &output_file, const std::string &directory);

    static void update_sizes();
    void clear_data();

    static std::set<size_t> get_channel_orders();
    static std::set<size_t> get_commands_channel_orders();

    static void add_dsp_dependencies(
        std::set<std::string> &dependencies,
        std::vector<std::string> &names,
        const std::vector<Link> &links,
        size_t dsp_index
    );

  public:
    Song();

    void new_song();
    void load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename);
    void compile(const std::string &filename, CompilationScheme scheme, CompilationTarget compilation_target) const;
    void render(const std::string &filename);

    std::string get_title() const;
    std::string get_author() const;
    std::string get_message() const;
    void set_title(const std::string &title);
    void set_author(const std::string &author);
    void set_message(const std::string &message);

    void change_tuning(uint8_t new_edo, double base_frequency);

    uint16_t get_max_rows();
    uint64_t get_song_length();
    uint8_t get_output_channels() const;
    void set_output_channels(uint8_t channels);

    Envelope *add_envelope(size_t index = -1);
    Sequence *add_sequence(size_t index = -1);
    Order *add_order(size_t index = -1);
    Wavetable *add_wavetable(size_t index = -1);
    void *add_oscillator(size_t index = -1);
    Channel *add_channel(size_t index = -1);
    void *add_dsp(size_t index = -1);
    CommandsSequence *add_commands_sequence(size_t index = -1);
    CommandsChannel *add_commands_channel(size_t index = -1);

    static Envelope *insert_envelope(Envelope *envelope, size_t index = -1);
    static Sequence *insert_sequence(Sequence *sequence, size_t index = -1);
    static Order *insert_order(Order *order, size_t index = -1);
    static Wavetable *insert_wavetable(Wavetable *wavetable, size_t index = -1);
    static void *insert_oscillator(void *oscillator, size_t index = -1);
    static Channel *insert_channel(Channel *channel, size_t index = -1);
    static void *insert_dsp(void *dsp, size_t index = -1);
    static CommandsSequence *insert_commands_sequence(CommandsSequence *sequence, size_t index = -1);
    static CommandsChannel *insert_commands_channel(CommandsChannel *channel, size_t index = -1);

    Envelope *duplicate_envelope(size_t index);
    Sequence *duplicate_sequence(size_t index);
    Order *duplicate_order(size_t index);
    Wavetable *duplicate_wavetable(size_t index);
    void *duplicate_oscillator(size_t index);
    Channel *duplicate_channel(size_t index);
    void *duplicate_dsp(size_t index);
    CommandsChannel *duplicate_commands_channel(size_t index);
    CommandsSequence *duplicate_commands_sequence(size_t index);

    void remove_envelope(size_t index);
    void remove_sequence(size_t index);
    void remove_order(size_t index);
    void remove_wavetable(size_t index);
    void remove_oscillator(size_t index);
    void remove_channel(size_t index);
    void remove_dsp(size_t index);
    void remove_commands_channel(size_t index);
    void remove_commands_sequence(size_t index);

    std::pair<ValidationResult, int> validate();
    static std::vector<std::string> find_envelope_dependencies(size_t envelope_index);
    static std::vector<std::string> find_sequence_dependencies(size_t sequence_index);
    static std::vector<std::string> find_order_dependencies(size_t order_index);
    static std::vector<std::string> find_wavetable_dependencies(size_t wavetable_index);
    static std::vector<std::string> find_oscillator_dependencies(size_t oscillator_index);
    static std::vector<std::string> find_commands_sequence_dependencies(size_t sequence_index);
    static std::vector<std::string> find_dsp_dependencies(size_t dsp_index);

    static size_t calculate_dsps(Effect effect);
    static size_t calculate_oscillators(Generator generator);
    static size_t calculate_commands(Instruction instruction);
    static float calculate_real_bpm();
    static float get_row_duration();
};
