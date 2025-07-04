#include "../maps/routing.hpp"
#include "../song/song.hpp"
#include "../utils/math.hpp"
#include "../utils/string.hpp"
#include "history/manager.hpp"
#include "history/actions/add.hpp"
#include "history/actions/command.hpp"
#include "history/actions/lock.hpp"
#include "history/actions/note.hpp"
#include "history/actions/remove.hpp"
#include "history/actions/routing.hpp"
#include "history/actions/sequence.hpp"
#include "history/actions/text.hpp"
#include "history/actions/value.hpp"
#include "history/actions/wavetable.hpp"
#include "names.hpp"
#include "undo.hpp"

template <typename T>
void perform_action(
    GUIPanel *owner,
    const LinkKey key,
    T &reference,
    const T old_value
) {
    if (old_value != reference) {
        const std::string label = get_key_name(key);
        const auto value_change = ValueChange<T>(reference, old_value);
        history_manager.add_action(
            std::make_unique<ChangeValueAction<T>>(label, owner, key, value_change)
        );
    }
}

void perform_action_lock(
    GUIPanel *owner,
    const LinkKey key,
    const bool &reference
) {
    const std::string label = target_names.at(key.target) + " " + std::to_string(key.index);
    history_manager.add_action(
        std::make_unique<ChangeLockAction>(label, owner, key, reference)
    );
}

void perform_action_float(
    GUIPanel *owner,
    const LinkKey key,
    float &reference,
    const float old_value,
    const char *format
) {
    std::string old_value_string = convert_double_to_string(old_value, format);
    std::string new_value_string = convert_double_to_string(reference, format);

    if (old_value != reference && old_value_string != new_value_string) {
        const std::string label = get_key_name(key);
        const auto value_change = ValueChange<float>(reference, old_value);
        history_manager.add_action(
            std::make_unique<ChangeValueAction<float>>(label, owner, key, value_change)
        );
    }
}

void perform_action_routing(
    GUIPanel *owner,
    const LinkKey key,
    const InputKey &input_key,
    const LinkKey &new_value,
    const LinkKey &old_value
) {
    if (old_value != new_value) {
        std::string label = key.target == Target::CHANNEL ? "Channel " : "DSP ";
        label += std::to_string(key.index);
        const auto value_change = RoutingChange(input_key, old_value, new_value);
        history_manager.add_action(
            std::make_unique<ChangeRoutingAction>(label, owner, key, value_change)
        );
    }
}

void perform_action_order_sequence(
    GUIPanel *owner,
    const LinkKey key,
    const size_t sequence_index,
    const size_t old_sequence,
    const size_t new_sequence
) {
    if (old_sequence != new_sequence) {
        const std::string label = "Order " + std::to_string(key.index);
        const OrderSequenceChange value_change = {sequence_index, old_sequence, new_sequence};
        history_manager.add_action(
            std::make_unique<ChangeOrderSequenceAction>(label, owner, key, value_change)
        );
    }
}

template <size_t n>
void perform_action_string(
    GUIPanel *owner,
    const LinkKey key,
    char (&buffer)[n],
    const std::string &old_value
) {
    const std::string new_value = buffer;
    if (old_value != new_value) {
        std::string label = get_key_name(key);
        const auto value_change = TextChange<n>(buffer, old_value, new_value);
        history_manager.add_action(
            std::make_unique<ChangeTextAction<n>>(label, owner, key, value_change)
        );
    }
}

void perform_action_note(
    GUIPanel *owner,
    const LinkKey key,
    const PatternRow pattern_row,
    const uint8_t old_note,
    const uint8_t new_note
) {
    if (old_note != new_note) {
        const std::string label = "Sequence " + std::to_string(key.index);
        const NoteChange value_change = {pattern_row, old_note, new_note};
        history_manager.add_action(
            std::make_unique<ChangeNoteAction>(label, owner, key, value_change)
        );
    }
}

void perform_action_command(
    GUIPanel *owner,
    LinkKey key,
    PatternRow pattern_row,
    CommandValue old_command,
    CommandValue new_command
) {
    if (old_command != new_command) {
        const std::string label = "Sequence " + std::to_string(key.index);
        const CommandChange value_change = {pattern_row, old_command, new_command};
        history_manager.add_action(
            std::make_unique<ChangeCommandAction>(label, owner, key, value_change)
        );
    }
}

void perform_action_add(
    GUIPanel *owner,
    const LinkKey key
) {
    const std::string label = target_names.at(key.target) + " " + std::to_string(key.index);
    AddFunction add = [owner, key]() -> void * {
        void *result;
        switch (key.target) {
        case Target::ENVELOPE: {
            result = song.add_envelope();
            break;
        }
        case Target::SEQUENCE: {
            result = song.add_sequence();
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            result = song.add_commands_sequence();
            break;
        }
        case Target::ORDER: {
            result = song.add_order();
            break;
        }
        case Target::OSCILLATOR: {
            result = song.add_oscillator();
            break;
        }
        case Target::WAVETABLE: {
            result = song.add_wavetable();
            break;
        }
        case Target::DSP: {
            result = song.add_dsp();
            break;
        }
        case Target::CHANNEL: {
            result = song.add_channel();
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            result = song.add_commands_channel();
            break;
        }
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type for add operation: " + std::to_string(static_cast<int>(key.target)));
        }
        }

        owner->update();
        return result;
    };

    RemoveFunction remove = [owner, key](size_t index) -> void {
        switch (key.target) {
        case Target::ENVELOPE: {
            song.remove_envelope(index);
            break;
        }
        case Target::SEQUENCE: {
            song.remove_sequence(index);
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            song.remove_commands_sequence(index);
            break;
        }
        case Target::ORDER: {
            song.remove_order(index);
            break;
        }
        case Target::OSCILLATOR: {
            song.remove_oscillator(index);
            break;
        }
        case Target::WAVETABLE: {
            song.remove_wavetable(index);
            break;
        }
        case Target::DSP: {
            song.remove_dsp(index);
            break;
        }
        case Target::CHANNEL: {
            song.remove_channel(index);
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            song.remove_commands_channel(index);
            break;
        }
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type for remove operation: " + std::to_string(static_cast<int>(key.target)));
        }
        }

        owner->update();
    };
    history_manager.add_action(
        std::make_unique<AddItemAction>(label, owner, key, add, remove)
    );
}

template <typename T>
void perform_action_remove(
    GUIPanel *owner,
    const LinkKey key,
    const T *pointer
) {
    const T item = *pointer;
    const std::string label = target_names.at(key.target) + " " + std::to_string(key.index);
    RemoveFunction remove = [owner, key](size_t index) -> void {
        switch (key.target) {
        case Target::ENVELOPE: {
            song.remove_envelope(index);
            break;
        }
        case Target::SEQUENCE: {
            song.remove_sequence(index);
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            song.remove_commands_sequence(index);
            break;
        }
        case Target::ORDER: {
            song.remove_order(index);
            break;
        }
        case Target::OSCILLATOR: {
            song.remove_oscillator(index);
            break;
        }
        case Target::WAVETABLE: {
            song.remove_wavetable(index);
            break;
        }
        case Target::DSP: {
            song.remove_dsp(index);
            break;
        }
        case Target::CHANNEL: {
            song.remove_channel(index);
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            song.remove_commands_channel(index);
            break;
        }
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type for remove operation: " + std::to_string(static_cast<int>(key.target)));
        }
        }

        owner->from();
        owner->update();
    };

    RestoreFunction<T> restore = [owner, key](T *restored_item, size_t index) -> T * {
        T *result = nullptr;
        if constexpr (std::is_same_v<T, Envelope>) {
            assert(key.target == Target::ENVELOPE && "Type mismatch: Envelope expected");
            result = static_cast<T *>(song.insert_envelope(restored_item, index));
        } else if constexpr (std::is_same_v<T, Sequence>) {
            assert(key.target == Target::SEQUENCE && "Type mismatch: Sequence expected");
            result = static_cast<T *>(song.insert_sequence(restored_item, index));
        } else if constexpr (std::is_same_v<T, CommandsSequence>) {
            assert(key.target == Target::COMMANDS_SEQUENCE && "Type mismatch: CommandsSequence expected");
            result = static_cast<T *>(song.insert_commands_sequence(restored_item, index));
        } else if constexpr (std::is_same_v<T, Order>) {
            assert(key.target == Target::ORDER && "Type mismatch: Order expected");
            result = static_cast<T *>(song.insert_order(restored_item, index));
        } else if constexpr (std::is_same_v<T, Oscillator>) {
            assert(key.target == Target::OSCILLATOR && "Type mismatch: Oscillator expected");
            result = static_cast<T *>(song.insert_oscillator(restored_item, index));
        } else if constexpr (std::is_same_v<T, Wavetable>) {
            assert(key.target == Target::WAVETABLE && "Type mismatch: Wavetable expected");
            result = static_cast<T *>(song.insert_wavetable(restored_item, index));
        } else if constexpr (std::is_same_v<T, DSP>) {
            assert(key.target == Target::DSP && "Type mismatch: DSP expected");
            result = static_cast<T *>(song.insert_dsp(restored_item, index));
        } else if constexpr (std::is_same_v<T, Channel>) {
            assert(key.target == Target::CHANNEL && "Type mismatch: Channel expected");
            result = static_cast<T *>(song.insert_channel(restored_item, index));
        } else if constexpr (std::is_same_v<T, CommandsChannel>) {
            assert(key.target == Target::COMMANDS_CHANNEL && "Type mismatch: CommandsChannel expected");
            result = static_cast<T *>(song.insert_commands_channel(restored_item, index));
        }

        if (!result) {
            throw std::runtime_error("Incompatible type for target: " + std::to_string(static_cast<int>(key.target)));
        }

        owner->from();
        owner->update();
        return result;
    };

    history_manager.add_action(
        std::make_unique<RemoveItemAction<T>>(
            label, owner, key, item, remove, restore
        )
    );
}

void perform_action_wavetable(
    GUIPanel *owner,
    const LinkKey key,
    int &wavetable_size,
    std::vector<float> &wavetable,
    const std::vector<float> &old_wavetable
) {
    const std::string label = "Wavetable " + std::to_string(key.index);
    const WavetableChange value_change = WavetableChange(wavetable_size, wavetable, old_wavetable);
    history_manager.add_action(
        std::make_unique<ChangeWavetableAction>(label, owner, key, value_change)
    );
}

template <typename T>
void perform_action_pattern_selection(
    GUIPanel *owner,
    const LinkKey key,
    const std::string &label,
    const PatternSelectionChange<T> &changes,
    const SetItemsFunction<T> function
) {
    if (changes.empty()) {
        return;
    }

    history_manager.add_action(
        std::make_unique<ChangePatternSelectionAction<T>>(label, owner, key, changes, function)
    );
}

template void perform_action<float>(
    GUIPanel *owner,
    const LinkKey key,
    float &reference,
    const float old_value
);

template void perform_action<int>(
    GUIPanel *owner,
    const LinkKey key,
    int &reference,
    const int old_value
);

template void perform_action<bool>(
    GUIPanel *owner,
    const LinkKey key,
    bool &reference,
    const bool old_value
);

template void perform_action_string<GUI_MAX_STRING_LENGTH>(
    GUIPanel *owner,
    const LinkKey key,
    char (&buffer)[GUI_MAX_STRING_LENGTH],
    const std::string &old_value
);

template void perform_action_remove<Envelope>(GUIPanel *owner, const LinkKey key, const Envelope *pointer);
template void perform_action_remove<Sequence>(GUIPanel *owner, const LinkKey key, const Sequence *pointer);
template void perform_action_remove<CommandsSequence>(GUIPanel *owner, const LinkKey key, const CommandsSequence *pointer);
template void perform_action_remove<Order>(GUIPanel *owner, const LinkKey key, const Order *pointer);
template void perform_action_remove<Oscillator>(GUIPanel *owner, const LinkKey key, const Oscillator *pointer);
template void perform_action_remove<Wavetable>(GUIPanel *owner, const LinkKey key, const Wavetable *pointer);
template void perform_action_remove<DSP>(GUIPanel *owner, const LinkKey key, DSP const *pointer);
template void perform_action_remove<Channel>(GUIPanel *owner, const LinkKey key, const Channel *pointer);
template void perform_action_remove<CommandsChannel>(GUIPanel *owner, const LinkKey key, const CommandsChannel *pointer);

template void perform_action_pattern_selection<uint8_t>(
    GUIPanel *owner,
    const LinkKey key,
    const std::string &label,
    const PatternSelectionChange<uint8_t> &changes,
    const SetItemsFunction<uint8_t> function
);

template void perform_action_pattern_selection<CommandValue>(
    GUIPanel *owner,
    const LinkKey key,
    const std::string &label,
    const PatternSelectionChange<CommandValue> &changes,
    const SetItemsFunction<CommandValue> function
);
