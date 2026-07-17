#include <cassert>

#include "WorldUpdateFrameCodec.h"

namespace world_update_frame_tests {

void round_trips_snapshot_and_rejects_wrong_magic() {
    const network::WorldUpdate source = {
        1, network::UpdateKind::Snapshot, 4, 8, 0, "ether.fire=1"};
    std::vector<uint8_t> frame;
    std::string error;
    assert(network::encodeWorldUpdateFrame(source, frame, error));
    network::WorldUpdate decoded = {};
    assert(network::decodeWorldUpdateFrame(frame, decoded, error));
    assert(decoded.sequence == 8 && decoded.payload == source.payload);
    frame[0] = 0;
    assert(!network::decodeWorldUpdateFrame(frame, decoded, error));
}

void rejects_truncated_or_oversized_world_update_frames() {
    std::vector<uint8_t> frame(network::WORLD_UPDATE_FRAME_HEADER_SIZE, 0);
    std::string error;
    network::WorldUpdate decoded = {};
    assert(!network::decodeWorldUpdateFrame(frame, decoded, error));
}

} // namespace world_update_frame_tests
