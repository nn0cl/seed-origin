#ifndef SEED_CLIENT_INPUT_SEQUENCE_H
#define SEED_CLIENT_INPUT_SEQUENCE_H

#include <cstdint>
#include <map>
#include <string>

namespace server {

inline constexpr uint64_t MAX_CLIENT_INPUT_AHEAD = 64;

enum class ClientInputAdmission {
    Accept,
    IgnoreStale,
    RejectTooFarAhead,
    RejectInvalid
};

enum class MovementEnqueueResult {
    Enqueued,
    IgnoredStale,
    Rejected
};

class ClientInputSequenceTracker {
public:
    ClientInputSequenceTracker();

    ClientInputAdmission admit(int64_t sessionId,
                               uint64_t clientInputSequence) const;
    void noteAccepted(int64_t sessionId, uint64_t clientInputSequence);
    void noteProcessed(int64_t sessionId, uint64_t clientInputSequence);
    uint64_t lastProcessed(int64_t sessionId) const;
    void rebase(int64_t sessionId, uint64_t lastProcessedInputSequence);

private:
    std::map<int64_t, uint64_t> lastAccepted;
    std::map<int64_t, uint64_t> lastProcessedBySession;
};

[[nodiscard]] bool parseMovePayload(const std::string& payload,
                                    uint64_t& clientInputSequence, float& dx,
                                    float& dy, float& dz);

struct MovementAck {
    int64_t sessionId;
    float x;
    float y;
    float z;
    uint64_t worldTick;
    uint64_t lastProcessedInputSequence;
};

[[nodiscard]] bool formatMovementAck(const MovementAck& ack, std::string& payload,
                                     std::string& error);
[[nodiscard]] bool parseMovementAck(const std::string& payload, MovementAck& ack,
                                    std::string& error);
[[nodiscard]] bool parseMovementEventSession(const std::string& payload,
                                             int64_t& sessionId);
[[nodiscard]] bool tryParseAttachedMovementAck(const std::string& payload,
                                               MovementAck& ack);

struct PublicMovementPose {
    int64_t sessionId;
    float x;
    float y;
    float z;
};

[[nodiscard]] bool tryParsePublicMovementPose(const std::string& payload,
                                              PublicMovementPose& pose);

}

#endif
