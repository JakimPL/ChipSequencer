#pragma once

#pragma once

#include <complex>
#include <deque>
#include <memory>
#include <vector>
#include <fftw3.h>

class FFT {
  public:
    FFT(size_t fft_size = 1024);
    ~FFT();

    void compute(const std::deque<_Float32> &samples);

    const std::vector<float> &get_magnitudes() const;
    const std::vector<std::complex<float>> &get_complex_output() const;
    size_t get_size() const;

  private:
    size_t fft_size;
    std::vector<float> input_buffer;
    std::vector<std::complex<float>> output_buffer;
    std::vector<float> magnitude_buffer;
    fftwf_plan fft_plan;

    std::vector<float> window;
    void apply_window();
    void create_window();
};
