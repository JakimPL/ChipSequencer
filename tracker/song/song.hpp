#ifndef SONG_SONG_HPP
#define SONG_SONG_HPP

#include <nlohmann/json.hpp>
#include "../constants.hpp"
#include "../structures.hpp"

struct Song {
    struct Header {
        std::string author;
        std::string title;
        std::string version;
    } header;

    uint16_t &bpm;
    _Float32 &normalizer;
    int output_channels;
    int song_length;
    Envelopes &envelopes;
    Sequences &sequences;
    Orders &orders;
    Oscillators &oscillators;
    Wavetables &wavetables;
    DSPs &dsps;
    Channels &channels;

    Song(
        uint16_t &bpm_reference,
        _Float32 &normalizer_reference,
        int output_ch,
        int song_len,
        Envelopes &env,
        Sequences &seq,
        Orders &ord,
        Oscillators &osc,
        Wavetables &wav,
        DSPs &dsp,
        Channels &chn
    )
        : bpm(bpm_reference),
          normalizer(normalizer_reference),
          output_channels(output_ch),
          song_length(song_len),
          envelopes(env),
          sequences(seq),
          orders(ord),
          oscillators(osc),
          wavetables(wav),
          dsps(dsp),
          channels(chn) {}

    ~Song() {
        // for (auto env : envelopes) delete env;
        // for (auto seq : sequences) delete seq;
        // for (auto ord : orders) delete ord;
        // for (auto osc : oscillators) delete osc;
        // for (auto wav : wavetables) delete wav;
        // for (auto dsp : dsps) delete dsp;
        // for (auto chn : channels) delete chn;
    }
    void save_to_file(const std::string &filename);

  private:
    std::string generate_asm_file() const;
    nlohmann::json create_header_json() const;

    template <typename T>
    void write_data(std::ofstream &file, const T *data, const size_t size) const;

    template <typename T>
    void write_vector(std::ofstream &file, const std::vector<T> &vector, const size_t size, bool write_count = true) const;

    template <typename T>
    void export_vector(const std::string &filename, const std::vector<T> &vector, const size_t size) const;

    void export_asm_file(const std::string &directory) const;
    void export_header(const std::string &directory) const;

    void compress_directory(const std::string &directory, const std::string &output_file) const;
};

#endif // SONG_SONG_HPP