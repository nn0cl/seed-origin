#include "OutboundFrameQueue.h"

namespace server {

bool OutboundFrameQueue::enqueue(const std::vector<uint8_t>& frame, std::string& error) {
    if (frame.size() < network::FRAME_HEADER_SIZE ||
        frame.size() > MAX_OUTBOUND_FRAME_SIZE) {
        error = "outbound frame size is invalid";
        return false;
    }
    if (frames.size() >= MAX_OUTBOUND_FRAMES) {
        error = "outbound frame queue is full";
        return false;
    }
    frames.push_back(frame);
    error.clear();
    return true;
}

bool OutboundFrameQueue::pop(std::vector<uint8_t>& frame) {
    if (frames.empty()) {
        frame.clear();
        return false;
    }
    frame = frames.front();
    frames.pop_front();
    return true;
}

void OutboundFrameQueue::clear() {
    frames.clear();
}

size_t OutboundFrameQueue::size() const {
    return frames.size();
}

bool OutboundFrameQueue::empty() const {
    return frames.empty();
}

}
