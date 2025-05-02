#include "../utils/file.hpp"
#include "channel.hpp"

void Channel::serialize_output(std::ofstream &file, const Channel *channel) const {
    write_data(file, &channel->output, sizeof(channel->output));
}

void Channel::serialize_body(std::ofstream &file, const Channel *channel) const {
    write_data(file, &channel->splitter, sizeof(channel->splitter));
    write_data(file, &channel->output_flag, sizeof(channel->output_flag));
    write_data(file, &channel->envelope_index, sizeof(channel->envelope_index));
    write_data(file, &channel->order_index, sizeof(channel->order_index));
    write_data(file, &channel->oscillator_index, sizeof(channel->oscillator_index));
    write_data(file, &channel->pitch, sizeof(channel->pitch));
    write_data(file, &channel->flag, sizeof(channel->flag));
    write_data(file, &channel->fraction, sizeof(channel->fraction));
    write_data(file, &channel->pad, sizeof(channel->pad));
}

Channel *Channel::deserialize(std::ifstream &output_file, std::ifstream &body_file) {
    Channel *channel = new Channel();
    read_data(output_file, &channel->output, sizeof(channel->output));

    read_data(body_file, &channel->splitter, sizeof(channel->splitter));
    read_data(body_file, &channel->output_flag, sizeof(channel->output_flag));
    read_data(body_file, &channel->envelope_index, sizeof(channel->envelope_index));
    read_data(body_file, &channel->order_index, sizeof(channel->order_index));
    read_data(body_file, &channel->oscillator_index, sizeof(channel->oscillator_index));
    read_data(body_file, &channel->pitch, sizeof(channel->pitch));
    read_data(body_file, &channel->flag, sizeof(channel->flag));
    read_data(body_file, &channel->fraction, sizeof(channel->fraction));

    return channel;
}
