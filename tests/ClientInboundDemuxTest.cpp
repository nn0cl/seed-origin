#include <cassert>
#include <string>
#include <vector>

#include "ClientInboundDemux.h"
#include "DisconnectResponseCodec.h"
#include "LoginResponseCodec.h"
#include "WorldUpdateFrameCodec.h"

namespace client_inbound_demux_tests {
namespace {

bool encodeLogin(const network::LoginResponse& response,
                 std::vector<uint8_t>& frame) {
    std::string error;
    return network::encodeLoginResponseFrame(response, frame, error);
}

bool encodeUpdate(const network::WorldUpdate& update, std::vector<uint8_t>& frame) {
    std::string error;
    return network::encodeWorldUpdateFrame(update, frame, error);
}

}

void splits_login_response_and_world_update_from_one_buffer() {
    const network::LoginResponse accepted = {
        network::CURRENT_PROTOCOL_VERSION, network::LoginResponseStatus::Accepted,
        21, std::string()};
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 1, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0"};
    std::vector<uint8_t> login;
    std::vector<uint8_t> world;
    assert(encodeLogin(accepted, login));
    assert(encodeUpdate(snapshot, world));
    std::vector<uint8_t> mixed = login;
    mixed.insert(mixed.end(), world.begin(), world.end());

    client::ClientInboundDemux demux;
    std::vector<client::InboundFrame> frames;
    std::string error;
    assert(demux.append(mixed, frames, error));
    assert(frames.size() == 2);
    assert(frames[0].kind == client::InboundFrameKind::LoginResponse);
    assert(frames[0].bytes == login);
    assert(frames[1].kind == client::InboundFrameKind::WorldUpdate);
    assert(frames[1].bytes == world);
}

void splits_disconnect_response_from_login_and_world_update() {
    const network::LoginResponse accepted = {
        network::CURRENT_PROTOCOL_VERSION, network::LoginResponseStatus::Accepted,
        21, std::string()};
    const network::DisconnectResponse ended = {
        network::CURRENT_PROTOCOL_VERSION,
        network::DisconnectResponseStatus::Accepted, 21, std::string()};
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 1, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0"};
    std::vector<uint8_t> login;
    std::vector<uint8_t> disconnect;
    std::vector<uint8_t> world;
    std::string error;
    assert(encodeLogin(accepted, login));
    assert(network::encodeDisconnectResponseFrame(ended, disconnect, error));
    assert(encodeUpdate(snapshot, world));
    std::vector<uint8_t> mixed = login;
    mixed.insert(mixed.end(), disconnect.begin(), disconnect.end());
    mixed.insert(mixed.end(), world.begin(), world.end());

    client::ClientInboundDemux demux;
    std::vector<client::InboundFrame> frames;
    assert(demux.append(mixed, frames, error));
    assert(frames.size() == 3);
    assert(frames[0].kind == client::InboundFrameKind::LoginResponse);
    assert(frames[1].kind == client::InboundFrameKind::DisconnectResponse);
    assert(frames[1].bytes == disconnect);
    assert(frames[2].kind == client::InboundFrameKind::WorldUpdate);
}

void joins_partial_inbound_frames_without_dispatch() {
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 2, 2, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0"};
    std::vector<uint8_t> world;
    assert(encodeUpdate(snapshot, world));
    assert(world.size() > 4);

    client::ClientInboundDemux demux;
    std::vector<client::InboundFrame> frames;
    std::string error;
    const std::vector<uint8_t> first(world.begin(), world.begin() + 4);
    const std::vector<uint8_t> rest(world.begin() + 4, world.end());
    assert(demux.append(first, frames, error));
    assert(frames.empty());
    assert(demux.append(rest, frames, error));
    assert(frames.size() == 1);
    assert(frames[0].kind == client::InboundFrameKind::WorldUpdate);
    assert(frames[0].bytes == world);
}

void rejects_invalid_inbound_prefix_after_two_bytes() {
    const uint8_t corrupt[] = {0xDE, 0xAD};
    client::ClientInboundDemux demux;
    std::vector<client::InboundFrame> frames;
    std::string error;
    assert(!demux.append(std::vector<uint8_t>(corrupt, corrupt + 2), frames, error));
    assert(error == "inbound frame prefix is invalid");
    assert(demux.failed());
}

void waits_for_second_prefix_byte_before_dispatch() {
    const uint8_t partial[] = {0x57};
    client::ClientInboundDemux demux;
    std::vector<client::InboundFrame> frames;
    std::string error;
    assert(demux.append(std::vector<uint8_t>(partial, partial + 1), frames, error));
    assert(frames.empty());
    assert(!demux.failed());
}

} // namespace client_inbound_demux_tests
