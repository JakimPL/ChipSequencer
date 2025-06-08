#include "target.hpp"

bool is_target_splitter(const Target target) {
    return target == Target::SPLITTER_OUTPUT ||
           target == Target::SPLITTER_DSP;
}

bool is_target_output(const Target target) {
    return target == Target::SPLITTER_OUTPUT ||
           target == Target::SPLITTER_DSP ||
           target == Target::DIRECT_OUTPUT ||
           target == Target::DIRECT_DSP;
}

bool is_target_parameter(const Target target) {
    return !is_target_output(target);
}
