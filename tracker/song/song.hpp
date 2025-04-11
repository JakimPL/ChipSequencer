#ifndef SONG_SONG_HPP
#define SONG_SONG_HPP

#include <nlohmann/json.hpp>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../version.hpp"
#include "../tuning/frequencies.hpp"
#include "../tuning/scale.hpp"
#include "link.hpp"
#include "validation.hpp"

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

    uint8_t output_channels = DEFAULT_OUTPUT_CHANNELS;
    uint64_t song_length = 0;
    uint16_t max_rows = 0;

    void generate_header_vector(std::stringstream &asm_content, const std::string &name, const std::string &short_name, const size_t size) const;
    std::string generate_header_asm_file() const;
    std::string generate_data_asm_file() const;
    nlohmann::json create_header_json() const;
    nlohmann::json import_header(const std::string &directory);

    std::string get_element_path(const std::string &directory, const std::string prefix, const size_t i, const char separator = '/') const;

    void calculate_song_length();

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
    void compile_sources(const std::string &directory, const std::string &filename, const bool compress) const;
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
    void compile(const std::string &filename, bool compress = true) const;

    void set_title(const std::string &title);
    void set_author(const std::string &author);

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

    void set_link(Link &link, void *item, const u_int8_t i) const;
    void set_links() const;
    void realign_links(const size_t index, const Target target, const ItemType type) const;
    void realign_links(const size_t index, const Target target) const;

    void set_buffer_offsets();

    void remove_envelope(const size_t index);
    void remove_sequence(const size_t index);
    void remove_order(const size_t index);
    void remove_wavetable(const size_t index);
    void remove_oscillator(const size_t index);
    void remove_channel(const size_t index);
    void remove_dsp(const size_t index);

    std::pair<ValidationResult, int> validate();
};

#endif // SONG_SONG_HPP