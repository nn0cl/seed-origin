#include <cassert>

#include "OutboundFrameQueue.h"

namespace outbound_frame_queue_tests {

std::vector<uint8_t> frameWithByte(uint8_t value) {
    return std::vector<uint8_t>(network::FRAME_HEADER_SIZE, value);
}

void preserves_fifo_order() {
    server::OutboundFrameQueue queue;
    std::string error;
    assert(queue.enqueue(frameWithByte(1), error));
    assert(queue.enqueue(frameWithByte(2), error));
    std::vector<uint8_t> first;
    std::vector<uint8_t> second;
    assert(queue.pop(first));
    assert(queue.pop(second));
    assert(first[0] == 1);
    assert(second[0] == 2);
    assert(queue.empty());
}

void rejects_invalid_frame_without_mutation() {
    server::OutboundFrameQueue queue;
    std::string error;
    assert(!queue.enqueue(std::vector<uint8_t>(), error));
    assert(queue.empty());
}

void rejects_frame_after_capacity_without_mutation() {
    server::OutboundFrameQueue queue;
    std::string error;
    const std::vector<uint8_t> frame = frameWithByte(7);
    for (size_t i = 0; i < server::MAX_OUTBOUND_FRAMES; ++i) {
        assert(queue.enqueue(frame, error));
    }
    assert(queue.size() == server::MAX_OUTBOUND_FRAMES);
    assert(!queue.enqueue(frameWithByte(8), error));
    assert(queue.size() == server::MAX_OUTBOUND_FRAMES);
}

} // namespace outbound_frame_queue_tests
