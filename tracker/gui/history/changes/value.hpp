#pragma once

template <typename T>
struct ValueChange {
    T &parameter;
    T old_value;
    T new_value;

    ValueChange(T &param, const T &old)
        : parameter(param), old_value(old), new_value(param) {}
};
