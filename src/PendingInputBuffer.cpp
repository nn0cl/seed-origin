#include "PendingInputBuffer.h"

namespace client {

PendingInputBuffer::PendingInputBuffer() : pending() {}

bool PendingInputBuffer::push(const PendingMoveInput& input) {
    if (input.clientInputSequence == 0 ||
        pending.size() >= MAX_PENDING_PREDICTION_INPUTS) {
        return false;
    }
    if (!pending.empty() &&
        input.clientInputSequence <= pending.back().clientInputSequence) {
        return false;
    }
    pending.push_back(input);
    return true;
}

void PendingInputBuffer::acknowledgeUpTo(uint64_t lastProcessedInputSequence) {
    while (!pending.empty() &&
           pending.front().clientInputSequence <= lastProcessedInputSequence) {
        pending.pop_front();
    }
}

std::vector<PendingMoveInput> PendingInputBuffer::unacked() const {
    return std::vector<PendingMoveInput>(pending.begin(), pending.end());
}

void PendingInputBuffer::clear() { pending.clear(); }

size_t PendingInputBuffer::size() const { return pending.size(); }

}
