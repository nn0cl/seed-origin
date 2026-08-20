#ifndef SEED_PENDING_INPUT_BUFFER_H
#define SEED_PENDING_INPUT_BUFFER_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

namespace client {

inline constexpr std::size_t MAX_PENDING_PREDICTION_INPUTS = 64;

struct PendingMoveInput {
    uint64_t clientInputSequence;
    float dx;
    float dy;
    float dz;
};

class PendingInputBuffer {
public:
    PendingInputBuffer();

    bool push(const PendingMoveInput& input);
    void acknowledgeUpTo(uint64_t lastProcessedInputSequence);
    std::vector<PendingMoveInput> unacked() const;
    void clear();
    size_t size() const;

private:
    std::deque<PendingMoveInput> pending;
};

}

#endif
