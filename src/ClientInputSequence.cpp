#include "ClientInputSequence.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace server {
namespace {

bool parseFiniteFloat(const std::string& text, float& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    return static_cast<bool>(input) && !(input >> trailing) &&
           std::isfinite(value);
}

bool parseUint64(const std::string& text, uint64_t& value) {
    if (text.empty() || text[0] == '-') return false;
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    return static_cast<bool>(input) && !(input >> trailing);
}

bool parseInt64(const std::string& text, int64_t& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    return static_cast<bool>(input) && !(input >> trailing);
}

bool parseAckField(const std::string& fields, const std::string& key,
                   std::string& value) {
    const std::string prefix = key + ":";
    const std::string::size_type start = fields.find(prefix);
    if (start == std::string::npos) return false;
    const std::string::size_type valueStart = start + prefix.size();
    const std::string::size_type end = fields.find(';', valueStart);
    if (end == std::string::npos) value = fields.substr(valueStart);
    else value = fields.substr(valueStart, end - valueStart);
    return !value.empty();
}

}

ClientInputSequenceTracker::ClientInputSequenceTracker()
    : lastAccepted(), lastProcessedBySession() {}

ClientInputAdmission ClientInputSequenceTracker::admit(
    int64_t sessionId, uint64_t clientInputSequence) const {
    if (sessionId <= 0 || clientInputSequence == 0) {
        return ClientInputAdmission::RejectInvalid;
    }
    uint64_t previous = 0;
    std::map<int64_t, uint64_t>::const_iterator found =
        lastAccepted.find(sessionId);
    if (found != lastAccepted.end()) previous = found->second;
    if (clientInputSequence <= previous) return ClientInputAdmission::IgnoreStale;
    if (clientInputSequence - previous > MAX_CLIENT_INPUT_AHEAD) {
        return ClientInputAdmission::RejectTooFarAhead;
    }
    return ClientInputAdmission::Accept;
}

void ClientInputSequenceTracker::noteAccepted(int64_t sessionId,
                                              uint64_t clientInputSequence) {
    lastAccepted[sessionId] = clientInputSequence;
}

void ClientInputSequenceTracker::noteProcessed(int64_t sessionId,
                                               uint64_t clientInputSequence) {
    if (clientInputSequence == 0) return;
    uint64_t& value = lastProcessedBySession[sessionId];
    if (clientInputSequence > value) value = clientInputSequence;
}

uint64_t ClientInputSequenceTracker::lastProcessed(int64_t sessionId) const {
    std::map<int64_t, uint64_t>::const_iterator found =
        lastProcessedBySession.find(sessionId);
    return found == lastProcessedBySession.end() ? 0 : found->second;
}

void ClientInputSequenceTracker::rebase(int64_t sessionId,
                                        uint64_t lastProcessedInputSequence) {
    lastAccepted[sessionId] = lastProcessedInputSequence;
    lastProcessedBySession[sessionId] = lastProcessedInputSequence;
}

bool parseMovePayload(const std::string& payload, uint64_t& clientInputSequence,
                      float& dx, float& dy, float& dz) {
    clientInputSequence = 0;
    dx = 0.0f;
    dy = 0.0f;
    dz = 0.0f;
    const std::string::size_type colon = payload.find(':');
    const std::string::size_type firstComma = payload.find(',');
    std::string deltas = payload;
    if (colon != std::string::npos &&
        (firstComma == std::string::npos || colon < firstComma)) {
        if (!parseUint64(payload.substr(0, colon), clientInputSequence) ||
            clientInputSequence == 0) {
            return false;
        }
        deltas = payload.substr(colon + 1);
    }
    std::istringstream input(deltas);
    char separator = 0;
    if (!(input >> dx >> separator) || separator != ',') return false;
    if (!(input >> dy >> separator) || separator != ',') return false;
    if (!(input >> dz)) return false;
    input >> std::ws;
    if (!input.eof()) return false;
    return std::isfinite(dx) && std::isfinite(dy) && std::isfinite(dz);
}

bool formatMovementAck(const MovementAck& ack, std::string& payload,
                       std::string& error) {
    if (ack.sessionId <= 0 || !std::isfinite(ack.x) || !std::isfinite(ack.y) ||
        !std::isfinite(ack.z)) {
        error = "movement ack contains invalid state";
        return false;
    }
    std::ostringstream output;
    output << "movementAck=session:" << ack.sessionId
           << ";x:" << ack.x
           << ";y:" << ack.y
           << ";z:" << ack.z
           << ";worldTick:" << ack.worldTick
           << ";lastProcessedInputSequence:" << ack.lastProcessedInputSequence;
    payload = output.str();
    error.clear();
    return true;
}

bool parseMovementAck(const std::string& payload, MovementAck& ack,
                      std::string& error) {
    const std::string prefix = "movementAck=";
    if (payload.compare(0, prefix.size(), prefix) != 0) {
        error = "world update is not a movement ack";
        return false;
    }
    const std::string fields = payload.substr(prefix.size());
    std::string sessionText;
    std::string xText;
    std::string yText;
    std::string zText;
    std::string tickText;
    std::string seqText;
    if (!parseAckField(fields, "session", sessionText) ||
        !parseAckField(fields, "x", xText) ||
        !parseAckField(fields, "y", yText) ||
        !parseAckField(fields, "z", zText) ||
        !parseAckField(fields, "worldTick", tickText) ||
        !parseAckField(fields, "lastProcessedInputSequence", seqText) ||
        !parseInt64(sessionText, ack.sessionId) || ack.sessionId <= 0 ||
        !parseFiniteFloat(xText, ack.x) || !parseFiniteFloat(yText, ack.y) ||
        !parseFiniteFloat(zText, ack.z) ||
        !parseUint64(tickText, ack.worldTick) ||
        !parseUint64(seqText, ack.lastProcessedInputSequence)) {
        error = "movement ack contains invalid fields";
        return false;
    }
    error.clear();
    return true;
}

bool parseMovementEventSession(const std::string& payload, int64_t& sessionId) {
    const std::string prefix = "movement=session:";
    if (payload.compare(0, prefix.size(), prefix) != 0) return false;
    const std::string::size_type end = payload.find(';', prefix.size());
    const std::string text =
        end == std::string::npos
            ? payload.substr(prefix.size())
            : payload.substr(prefix.size(), end - prefix.size());
    return parseInt64(text, sessionId) && sessionId > 0;
}

bool tryParseAttachedMovementAck(const std::string& payload, MovementAck& ack) {
    if (!parseMovementEventSession(payload, ack.sessionId)) return false;
    std::string xText;
    std::string yText;
    std::string zText;
    std::string tickText;
    std::string seqText;
    if (!parseAckField(payload, "x", xText) ||
        !parseAckField(payload, "y", yText) ||
        !parseAckField(payload, "z", zText) ||
        !parseAckField(payload, "worldTick", tickText) ||
        !parseAckField(payload, "lastProcessedInputSequence", seqText) ||
        !parseFiniteFloat(xText, ack.x) || !parseFiniteFloat(yText, ack.y) ||
        !parseFiniteFloat(zText, ack.z) ||
        !parseUint64(tickText, ack.worldTick) ||
        !parseUint64(seqText, ack.lastProcessedInputSequence)) {
        return false;
    }
    return true;
}

bool tryParsePublicMovementPose(const std::string& payload,
                                PublicMovementPose& pose) {
    if (!parseMovementEventSession(payload, pose.sessionId)) return false;
    std::istringstream fields(payload);
    std::string field;
    bool haveX = false;
    bool haveY = false;
    bool haveZ = false;
    while (std::getline(fields, field, ';')) {
        const std::string::size_type separator = field.find('=');
        if (separator == std::string::npos) continue;
        const std::string key = field.substr(0, separator);
        const std::string value = field.substr(separator + 1);
        if (key == "x") {
            if (!parseFiniteFloat(value, pose.x)) return false;
            haveX = true;
        } else if (key == "y") {
            if (!parseFiniteFloat(value, pose.y)) return false;
            haveY = true;
        } else if (key == "z") {
            if (!parseFiniteFloat(value, pose.z)) return false;
            haveZ = true;
        }
    }
    return haveX && haveY && haveZ;
}

}
