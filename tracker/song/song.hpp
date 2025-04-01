#ifndef SONG_SONG_HPP
#define SONG_SONG_HPP

#include <nlohmann/json.hpp>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../version.hpp"
#include "link.hpp"

struct Song {
    struct Header {
        std::string author = "Unknown";
        std::string title = "Untitled";
        std::string version = TRACKER_VERSION;
    } header;

    uint16_t &bpm;
    _Float32 &normalizer;
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

    uint8_t output_channels = 1;
    uint32_t song_length = 186253;

    Song(
        uint16_t &bpm_reference,
        _Float32 &normalizer_reference,
        Envelopes &env,
        Sequences &seq,
        Orders &ord,
        Oscillators &osc,
        Wavetables &wav,
        DSPs &dsp,
        Channels &chn,
        Offsets &offsets,
        Links &lnk
    )
        : bpm(bpm_reference),
          normalizer(normalizer_reference),
          envelopes(env),
          sequences(seq),
          orders(ord),
          oscillators(osc),
          wavetables(wav),
          dsps(dsp),
          channels(chn),
          buffer_offsets(offsets),
          links(lnk) {
        current_offsets = new uint16_t[0];
        buffer_offsets = current_offsets;
        set_links();
    }

    ~Song() {
        delete[] current_offsets;
    }

    void new_song();
    void load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename, const bool compile = true) const;

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

  private:
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
    void compile_sources(const std::string &directory) const;
    void compress_directory(const std::string &directory, const std::string &output_file) const;
    void decompress_archive(const std::string &output_file, const std::string &directory);

    void clear_data();
    void delete_oscillator(void *oscillator);
    void delete_dsp(void *dsp);
};

#endif // SONG_SONG_HPP