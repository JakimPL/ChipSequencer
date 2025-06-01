#include "../general.hpp"
#include "../utils/file.hpp"
#include "channel.hpp"

void Channel::serialize(std::ofstream &file) const {
    write_data(file, this, sizeof(Channel));
}

Channel *Channel::deserialize(std::ifstream &file) {
    Channel *channel = resource_manager.allocate<Channel>();
    read_data(file, &channel->splitter, sizeof(channel->splitter));
    read_data(file, &channel->pitch, sizeof(channel->pitch));
    read_data(file, &channel->target, sizeof(channel->target));
    read_data(file, &channel->output_flag, sizeof(channel->output_flag));
    read_data(file, &channel->flag, sizeof(channel->flag));
    read_data(file, &channel->envelope_index, sizeof(channel->envelope_index));
    read_data(file, &channel->order_index, sizeof(channel->order_index));
    read_data(file, &channel->oscillator_index, sizeof(channel->oscillator_index));
    read_data(file, &channel->fraction, sizeof(channel->fraction));
    return channel;
}
