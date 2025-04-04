#ifndef SONG_SONG_HPP
#define SONG_SONG_HPP

#include <nlohmann/json.hpp>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../version.hpp"
#include "../tuning/frequencies.hpp"
#include "../tuning/scale.hpp"
#include "link.hpp"

class Song {
  private:
    struct Header {
        std::string author = "Unknown";
        std::string title = "Untitled";
        std::string version = TRACKER_VERSION;
    } header;

    struct Tuning {
        uint8_t edo = DEFAULT_EDO;
        double a4_frequency = DEFAULT_A4_FREQUENCY;
    } tuning;

    uint16_t &bpm;
    _Float32 &normalizer;

    uint8_t &num_channels;
    uint8_t &num_dsps;

    uint64_t &reference_frequency;
    _Float32 &note_divisor;

    Envelopes &envelopes;
    Sequences &sequences;
    Orders &orders;
    Oscillators &oscillators;
    Wavetables &wavetables;
    DSPs &dsps;
    Channels &channels;
    Offsets buffer_offsets;
    Offsets current_offsets = nullptr;
    Links &links;

    ScaleComposer &scale_composer;
    FrequencyTable &frequency_table;

    uint8_t output_channels = 1;
    uint32_t song_length = SONG_LENGTH;

    void generate_header_vector(std::stringstream &asm_content, const std::string &name, const std::string &short_name, const size_t size) const;
    std::string generate_header_asm_file() const;
    std::string generate_data_asm_file() const;
    nlohmann::json create_header_json() const;
    nlohmann::json import_header(const std::string &directory);

    std::string get_element_path(const std::string &directory, const std::string prefix, const size_t i, const char separator = '/') const;

    void set_link(Link &link, void *item, const u_int8_t i) const;
    void set_links();

    void serialize_channel(std::ofstream &file, Channel *channel) const;
    void serialize_dsp(std::ofstream &file, void *dsp) const;

    Channel *deserialize_channel(std::ifstream &file) const;
    void *deserialize_dsp(std::ifstream &file) const;
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
    void export_offsets(const std::string &filename) const;
    void export_links(const std::string &filename) const;

    void import_channels(const std::string &song_dir, const nlohmann::json &json);
    void import_dsps(const std::string &song_dir, const nlohmann::json &json);
    void import_offsets(const std::string &song_dir, const nlohmann::json &json);
    void import_links(const std::string &song_dir, const nlohmann::json &json);

    void import_envelopes(const std::string &song_dir, const nlohmann::json &json);
    void import_sequences(const std::string &song_dir, const nlohmann::json &json);
    void import_orders(const std::string &song_dir, const nlohmann::json &json);
    void import_wavetables(const std::string &song_dir, const nlohmann::json &json);
    void import_oscillators(const std::string &song_dir, const nlohmann::json &json);

    int run_command(const std::string &command) const;
    void compile_sources(const std::string &directory, const std::string &filename, const bool compress) const;
    void compress_directory(const std::string &directory, const std::string &output_file) const;
    void decompress_archive(const std::string &output_file, const std::string &directory);

    void update_sizes();
    void clear_data();
    void delete_oscillator(void *oscillator);
    void delete_dsp(void *dsp);

  public:
    Song(
        uint16_t &bpm_reference,
        _Float32 &normalizer_reference,
        uint8_t &num_channels_reference,
        uint8_t &num_dsps_reference,
        uint64_t &reference_frequency_reference,
        _Float32 &note_divisor_reference,
        Envelopes &envelopes_reference,
        Sequences &sequences_reference,
        Orders &orders_reference,
        Oscillators &oscillators_reference,
        Wavetables &wavetables_reference,
        DSPs &dsps_reference,
        Channels &channels_reference,
        Offsets &buffer_offsets_reference,
        Links &links_reference,
        ScaleComposer &scale_composer_reference,
        FrequencyTable &frequency_table_reference
    );

    ~Song();

    void new_song();
    void load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename) const;
    void compile(const std::string &filename, bool compress = true) const;

    void change_tuning(const uint8_t new_edo, const double base_frequency);

    Envelope *add_envelope();
    Sequence *add_sequence();
    Order *add_order();
    Wavetable *add_wavetable();
    void *add_oscillator();
    Channel *add_channel();
    void *add_dsp();

    void remove_envelope(const size_t index);
    void remove_sequence(const size_t index);
    void remove_order(const size_t index);
    void remove_wavetable(const size_t index);
    void remove_oscillator(const size_t index);
    void remove_channel(const size_t index);
    void remove_dsp(const size_t index);
};

#endif // SONG_SONG_HPP