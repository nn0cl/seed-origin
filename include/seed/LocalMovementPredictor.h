#ifndef SEED_LOCAL_MOVEMENT_PREDICTOR_H
#define SEED_LOCAL_MOVEMENT_PREDICTOR_H

#include <cstddef>
#include <cstdint>

#include "PendingInputBuffer.h"

namespace client {

inline constexpr float PREDICTION_CORRECTION_SECONDS = 0.15f;
inline constexpr float PREDICTION_SNAP_DISTANCE = 5.0f;

struct PredictedPose {
    float x;
    float y;
    float z;
};

struct ClientPredictionMetrics {
    float lastErrorDistance;
    uint64_t reconcileCount;
    float averagePendingInputCount;
};

class LocalMovementPredictor {
public:
    LocalMovementPredictor();

    void reset(float x, float y, float z);
    bool predict(uint64_t clientInputSequence, float dx, float dy, float dz);
    bool reconcile(float authoritativeX, float authoritativeY,
                   float authoritativeZ, uint64_t worldTick,
                   uint64_t lastProcessedInputSequence);
    bool applySnapshotBaseline(float x, float y, float z, uint64_t worldTick,
                               uint64_t lastProcessedInputSequence);
    void stepRender(float dtSeconds);

    const PredictedPose& predicted() const;
    const PredictedPose& rendered() const;
    size_t pendingCount() const;
    const ClientPredictionMetrics& metrics() const;
    uint64_t lastAckedInputSequence() const;
    uint64_t lastWorldTick() const;

private:
    PredictedPose predictedPose;
    PredictedPose renderedPose;
    PendingInputBuffer pending;
    uint64_t lastAcked;
    uint64_t worldTick;
    float correctionSeconds;
    ClientPredictionMetrics predictionMetrics;

    void replayUnacked();
    void recordReconcile(float errorDistance);
    void correctRender(float errorDistance, bool snap);
};

}

#endif
