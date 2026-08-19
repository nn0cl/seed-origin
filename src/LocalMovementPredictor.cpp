#include "LocalMovementPredictor.h"

#include "MovementSimulation.h"

#include <cmath>

namespace client {
namespace {

float poseDistance(const PredictedPose& a, const PredictedPose& b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

}

LocalMovementPredictor::LocalMovementPredictor()
    : predictedPose{0.0f, 0.0f, 0.0f}, renderedPose{0.0f, 0.0f, 0.0f},
      pending(), lastAcked(0), worldTick(0), correctionSeconds(0.0f),
      predictionMetrics{0.0f, 0, 0.0f} {}

void LocalMovementPredictor::reset(float x, float y, float z) {
    predictedPose = {x, y, z};
    renderedPose = {x, y, z};
    pending.clear();
    lastAcked = 0;
    worldTick = 0;
    correctionSeconds = 0.0f;
    predictionMetrics = {0.0f, 0, 0.0f};
}

bool LocalMovementPredictor::predict(uint64_t clientInputSequence, float dx,
                                     float dy, float dz) {
    float nextX = predictedPose.x;
    float nextY = predictedPose.y;
    float nextZ = predictedPose.z;
    if (!server::integrateMovement(nextX, nextY, nextZ, dx, dy, dz)) return false;
    if (!pending.push({clientInputSequence, dx, dy, dz})) return false;
    predictedPose = {nextX, nextY, nextZ};
    renderedPose.x += dx;
    renderedPose.y += dy;
    renderedPose.z += dz;
    return true;
}

void LocalMovementPredictor::replayUnacked() {
    const std::vector<PendingMoveInput> remaining = pending.unacked();
    for (size_t i = 0; i < remaining.size(); ++i) {
        if (!server::integrateMovement(predictedPose.x, predictedPose.y,
                                       predictedPose.z, remaining[i].dx,
                                       remaining[i].dy, remaining[i].dz)) {
            break;
        }
    }
}

void LocalMovementPredictor::recordReconcile(float errorDistance) {
    predictionMetrics.lastErrorDistance = errorDistance;
    predictionMetrics.reconcileCount += 1;
    const float pendingCount = static_cast<float>(pending.size());
    const float count = static_cast<float>(predictionMetrics.reconcileCount);
    predictionMetrics.averagePendingInputCount +=
        (pendingCount - predictionMetrics.averagePendingInputCount) / count;
}

void LocalMovementPredictor::correctRender(float errorDistance, bool snap) {
    if (snap || errorDistance >= PREDICTION_SNAP_DISTANCE) {
        renderedPose = predictedPose;
        correctionSeconds = 0.0f;
        return;
    }
    if (errorDistance > 0.0001f) {
        correctionSeconds = PREDICTION_CORRECTION_SECONDS;
        return;
    }
    renderedPose = predictedPose;
    correctionSeconds = 0.0f;
}

bool LocalMovementPredictor::reconcile(float authoritativeX,
                                       float authoritativeY,
                                       float authoritativeZ, uint64_t tick,
                                       uint64_t lastProcessedInputSequence) {
    pending.acknowledgeUpTo(lastProcessedInputSequence);
    predictedPose = {authoritativeX, authoritativeY, authoritativeZ};
    replayUnacked();
    lastAcked = lastProcessedInputSequence;
    worldTick = tick;
    const float error = poseDistance(renderedPose, predictedPose);
    recordReconcile(error);
    correctRender(error, false);
    return true;
}

bool LocalMovementPredictor::applySnapshotBaseline(
    float x, float y, float z, uint64_t tick,
    uint64_t lastProcessedInputSequence) {
    pending.acknowledgeUpTo(lastProcessedInputSequence);
    predictedPose = {x, y, z};
    replayUnacked();
    lastAcked = lastProcessedInputSequence;
    worldTick = tick;
    renderedPose = predictedPose;
    correctionSeconds = 0.0f;
    recordReconcile(0.0f);
    return true;
}

void LocalMovementPredictor::stepRender(float dtSeconds) {
    if (dtSeconds < 0.0f) dtSeconds = 0.0f;
    if (correctionSeconds <= 0.0f) {
        renderedPose = predictedPose;
        return;
    }
    const float t = dtSeconds >= correctionSeconds ? 1.0f
                                                   : dtSeconds / correctionSeconds;
    renderedPose.x += (predictedPose.x - renderedPose.x) * t;
    renderedPose.y += (predictedPose.y - renderedPose.y) * t;
    renderedPose.z += (predictedPose.z - renderedPose.z) * t;
    correctionSeconds -= dtSeconds;
    if (correctionSeconds <= 0.0f) {
        correctionSeconds = 0.0f;
        renderedPose = predictedPose;
    }
}

const PredictedPose& LocalMovementPredictor::predicted() const {
    return predictedPose;
}

const PredictedPose& LocalMovementPredictor::rendered() const {
    return renderedPose;
}

size_t LocalMovementPredictor::pendingCount() const { return pending.size(); }

const ClientPredictionMetrics& LocalMovementPredictor::metrics() const {
    return predictionMetrics;
}

uint64_t LocalMovementPredictor::lastAckedInputSequence() const {
    return lastAcked;
}

uint64_t LocalMovementPredictor::lastWorldTick() const { return worldTick; }

}
