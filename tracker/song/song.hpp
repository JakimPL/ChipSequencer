#ifndef SONG_SONG_HPP
#define SONG_SONG_HPP

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

    void generate_targets_asm(
        std::stringstream &asm_content,
        const CompilationTarget compilation_target,
        const char separator = '/'
    ) const;
    void generate_header_vector(
        std::stringstream &asm_content,
        const std::string &name,
        const std::string &short_name,
        const size_t size,
        const char separator = '/'
    ) const;
    std::string generate_header_asm_file() const;
    std::string generate_data_asm_file(const CompilationTarget compilation_target, const char separator = '/') const;

    void set_used_flags(std::stringstream &asm_content) const;
    nlohmann::json create_header_json() const;
    nlohmann::json import_header(const std::string &directory);
    nlohmann::json save_gui_state() const;

    std::string get_element_path(const std::string &directory, const std::string prefix, const size_t i, const char separator = '/') const;
    void calculate_song_length();

    void serialize_dsp(std::ofstream &file, void *dsp) const;
    void *deserialize_dsp(std::ifstream &file) const;
    void *deserialize_oscillator(std::ifstream &file) const;

    void export_all(const std::string &directory, const CompilationTarget compilation_target) const;
    void import_all(const std::string &directory, const nlohmann::json &json);

    void export_header_asm_file(const std::string &directory) const;
    void export_data_asm_file(const std::string &directory, const CompilationTarget compilation_target) const;
    void export_header(const std::string &directory) const;
    void export_gui_state(const std::string &directory) const;

    template <typename T>
    void export_series(const std::string &directory, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes) const;

    template <typename T>
    void export_arrays(const std::string &directory, const std::string &prefix, const std::vector<T> &arrays) const;

    void export_channels(const std::string &directory) const;
    void export_dsps(const std::string &directory) const;
    void export_commands_sequences(const std::string &directory) const;
    void export_links(const std::string &filename) const;

    void import_channels(const std::string &directory, const nlohmann::json &json);
    void import_dsps(const std::string &directory, const nlohmann::json &json);
    void import_links(const std::string &directory, const nlohmann::json &json);

    void import_envelopes(const std::string &directory, const nlohmann::json &json);
    void import_sequences(const std::string &directory, const nlohmann::json &json);
    void import_orders(const std::string &directory, const nlohmann::json &json);
    void import_wavetables(const std::string &directory, const nlohmann::json &json);
    void import_oscillators(const std::string &directory, const nlohmann::json &json);
    void import_commands_sequences(const std::string &directory, const nlohmann::json &json);
    void import_commands_channels(const std::string &directory, const nlohmann::json &json);
    void import_gui_state(const std::string &directory);

    int run_command(const std::string &command) const;
    void compile_sources(const std::string &directory, const std::string &filename, const CompilationScheme scheme, const std::string platform = "linux") const;
    void compress_directory(const std::string &directory, const std::string &output_file) const;
    void decompress_archive(const std::string &output_file, const std::string &directory);

    void update_sizes();
    void clear_data();

    std::set<size_t> get_channel_orders() const;
    std::set<size_t> get_commands_channel_orders() const;

  public:
    Song();
    ~Song();

    void new_song();
    void load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename);
    void compile(const std::string &filename, const CompilationScheme scheme, const CompilationTarget compilation_target) const;
    void render(const std::string &filename);

    std::string get_title() const;
    std::string get_author() const;
    std::string get_message() const;
    void set_title(const std::string &title);
    void set_author(const std::string &author);
    void set_message(const std::string &message);

    void change_tuning(const uint8_t new_edo, const double base_frequency);

    uint16_t get_max_rows();
    uint64_t get_song_length();
    uint8_t get_output_channels() const;
    void set_output_channels(const uint8_t channels);

    Envelope *add_envelope();
    Sequence *add_sequence();
    Order *add_order();
    Wavetable *add_wavetable();
    void *add_oscillator();
    Channel *add_channel();
    void *add_dsp();
    CommandsSequence *add_commands_sequence();
    CommandsChannel *add_commands_channel();

    Envelope *duplicate_envelope(const size_t index);
    Sequence *duplicate_sequence(const size_t index);
    Order *duplicate_order(const size_t index);
    Wavetable *duplicate_wavetable(const size_t index);
    void *duplicate_oscillator(const size_t index);
    Channel *duplicate_channel(const size_t index);
    void *duplicate_dsp(const size_t index);
    CommandsChannel *duplicate_commands_channel(const size_t index);
    CommandsSequence *duplicate_commands_sequence(const size_t index);

    void remove_envelope(const size_t index);
    void remove_sequence(const size_t index);
    void remove_order(const size_t index);
    void remove_wavetable(const size_t index);
    void remove_oscillator(const size_t index);
    void remove_channel(const size_t index);
    void remove_dsp(const size_t index);
    void remove_commands_channel(const size_t index);
    void remove_commands_sequence(const size_t index);

    std::pair<ValidationResult, int> validate();
    std::vector<std::string> find_envelope_dependencies(const size_t envelope_index) const;
    std::vector<std::string> find_sequence_dependencies(const size_t sequence_index) const;
    std::vector<std::string> find_order_dependencies(const size_t order_index) const;
    std::vector<std::string> find_wavetable_dependencies(const size_t wavetable_index) const;
    std::vector<std::string> find_oscillator_dependencies(const size_t oscillator_index) const;
    std::vector<std::string> find_commands_sequence_dependencies(const size_t sequence_index) const;

    size_t calculate_dsps(const Effect effect) const;
    size_t calculate_oscillators(const Generator generator) const;
    size_t calculate_commands(const Instruction instruction) const;
    float calculate_real_bpm() const;
    float get_row_duration() const;
};

#endif // SONG_SONG_HPP