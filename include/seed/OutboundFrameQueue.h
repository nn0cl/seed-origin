#ifndef SEED_OUTBOUND_FRAME_QUEUE_H
#define SEED_OUTBOUND_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <deque>
#include <string>
#include <vector>

#include "NetworkFrameCodec.h"

namespace server {

static const size_t MAX_OUTBOUND_FRAMES = 256;
static const size_t MAX_OUTBOUND_FRAME_SIZE = network::FRAME_HEADER_SIZE + 16384;

class OutboundFrameQueue {
public:
    bool enqueue(const std::vector<uint8_t>& frame, std::string& error);
    bool front(std::vector<uint8_t>& frame) const;
    bool consumeFront(size_t bytes);
    bool pop(std::vector<uint8_t>& frame);
    void clear();
    size_t size() const;
    bool empty() const;

private:
    std::deque<std::vector<uint8_t> > frames;
};

}

#endif
