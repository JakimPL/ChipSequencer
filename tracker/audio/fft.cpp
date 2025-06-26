#include <cmath>
#include <algorithm>

#include "fft.hpp"

FFT::FFT(size_t fft_size)
    : fft_size(fft_size) {

    input_buffer.resize(fft_size, 0.0f);
    output_buffer.resize(fft_size / 2 + 1);
    magnitude_buffer.resize(fft_size / 2 + 1, 0.0f);

    fft_plan = fftwf_plan_dft_r2c_1d(
        fft_size,
        input_buffer.data(),
        reinterpret_cast<fftwf_complex *>(output_buffer.data()),
        FFTW_MEASURE
    );

    create_window();
}

FFT::~FFT() {
    if (fft_plan) {
        fftwf_destroy_plan(fft_plan);
        fft_plan = nullptr;
    }
}

void FFT::compute(const std::deque<_Float32> &samples) {
    const size_t samples_to_copy = std::min(samples.size(), fft_size);
    size_t start_idx = samples.size() >= fft_size ? samples.size() - fft_size : 0;

    std::fill(input_buffer.begin(), input_buffer.end(), 0.0f);
    for (size_t i = 0; i < samples_to_copy; i++) {
        input_buffer[i] = samples[start_idx + i];
    }

    apply_window();
    fftwf_execute(fft_plan);

    for (size_t i = 0; i < magnitude_buffer.size(); i++) {
        float re = output_buffer[i].real();
        float im = output_buffer[i].imag();
        magnitude_buffer[i] = std::sqrt(re * re + im * im) / fft_size;
    }
}

void FFT::create_window() {
    window.resize(fft_size);
    for (size_t i = 0; i < fft_size; i++) {
        window[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (fft_size - 1)));
    }
}

void FFT::apply_window() {
    for (size_t i = 0; i < fft_size; i++) {
        input_buffer[i] *= window[i];
    }
}

const std::vector<float> &FFT::get_magnitudes() const {
    return magnitude_buffer;
}

const std::vector<std::complex<float>> &FFT::get_complex_output() const {
    return output_buffer;
}

size_t FFT::get_size() const {
    return fft_size;
}
