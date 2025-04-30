#ifndef SONG_SONG_HPP
#define SONG_SONG_HPP

#include <nlohmann/json.hpp>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../version.hpp"
#include "../tuning/frequencies.hpp"
#include "../tuning/scale.hpp"
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

    void generate_header_vector(
        std::stringstream &asm_content,
        const std::string &name,
        const std::string &short_name,
        const size_t size,
        const char separator = '/'
    ) const;
    void generate_header_channel_vector(std::stringstream &asm_content, const char separator = '/') const;
    void generate_header_dsp_vector(std::stringstream &asm_content, const char separator) const;
    void set_used_flags(std::stringstream &asm_content) const;
    std::string generate_header_asm_file() const;
    std::string generate_data_asm_file(const char separator = '/') const;
    nlohmann::json create_header_json() const;
    nlohmann::json import_header(const std::string &directory);

    std::string get_element_path(const std::string &directory, const std::string prefix, const size_t i, const char separator = '/') const;

    void calculate_song_length();
    size_t calculate_dsps(const uint8_t effect) const;
    size_t calculate_oscillators(const uint8_t generator) const;

    void serialize_dsp_header(std::ofstream &file, void *dsp) const;
    void serialize_dsp_body(std::ofstream &file, void *dsp) const;
    void *deserialize_dsp(std::ifstream &header_file, std::ifstream &body_file) const;
    void *deserialize_oscillator(std::ifstream &file) const;

    void export_all(const std::string &directory) const;
    void import_all(const std::string &directory, const nlohmann::json &json);

    void export_header_asm_file(const std::string &directory) const;
    void export_data_asm_file(const std::string &directory) const;
    void export_header(const std::string &directory) const;

    template <typename T>
    void export_series(const std::string &song_dir, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes) const;

    template <typename T>
    void export_arrays(const std::string &directory, const std::string &prefix, const std::vector<T> &arrays) const;

    void export_channels(const std::string &directory) const;
    void export_dsps(const std::string &directory) const;
    void export_links(const std::string &filename) const;
    void export_offsets(const std::string &filename) const;

    void import_channels(const std::string &song_dir, const nlohmann::json &json);
    void import_dsps(const std::string &song_dir, const nlohmann::json &json);
    void import_links(const std::string &song_dir, const nlohmann::json &json);

    void import_envelopes(const std::string &song_dir, const nlohmann::json &json);
    void import_sequences(const std::string &song_dir, const nlohmann::json &json);
    void import_orders(const std::string &song_dir, const nlohmann::json &json);
    void import_wavetables(const std::string &song_dir, const nlohmann::json &json);
    void import_oscillators(const std::string &song_dir, const nlohmann::json &json);

    int run_command(const std::string &command) const;
    void compile_sources(const std::string &directory, const std::string &filename, const bool compress, const std::string platform = "linux") const;
    void compress_directory(const std::string &directory, const std::string &output_file) const;
    void decompress_archive(const std::string &output_file, const std::string &directory);

    void update_sizes();
    void clear_data();
    void delete_oscillator(void *oscillator);
    void delete_dsp(void *dsp);

  public:
    Song();
    ~Song();

    void new_song();
    void load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename);
    void compile(const std::string &filename, bool compress = true, const std::string platform = "linux") const;
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

    Envelope *duplicate_envelope(const size_t index);
    Sequence *duplicate_sequence(const size_t index);
    Order *duplicate_order(const size_t index);
    Wavetable *duplicate_wavetable(const size_t index);
    void *duplicate_oscillator(const size_t index);
    Channel *duplicate_channel(const size_t index);
    void *duplicate_dsp(const size_t index);

    void set_buffer_offsets();

    void remove_envelope(const size_t index);
    void remove_sequence(const size_t index);
    void remove_order(const size_t index);
    void remove_wavetable(const size_t index);
    void remove_oscillator(const size_t index);
    void remove_channel(const size_t index);
    void remove_dsp(const size_t index);

    std::pair<ValidationResult, int> validate();
    std::vector<size_t> find_envelope_dependencies(const size_t envelope_index) const;
    std::vector<size_t> find_sequence_dependencies(const size_t sequence_index) const;
    std::vector<size_t> find_order_dependencies(const size_t order_index) const;
    std::vector<size_t> find_wavetable_dependencies(const size_t wavetable_index) const;
    std::vector<size_t> find_oscillator_dependencies(const size_t oscillator_index) const;
};

#endif // SONG_SONG_HPP