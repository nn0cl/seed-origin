#include <cassert>
#include <cmath>

#include "LocalMovementPredictor.h"

namespace local_movement_predictor_tests {
namespace {

bool nearly(float actual, float expected) {
    return std::fabs(actual - expected) < 0.0001f;
}

}

void predicts_local_move_immediately() {
    client::LocalMovementPredictor predictor;
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    assert(nearly(predictor.predicted().x, 1.0f));
    assert(nearly(predictor.rendered().x, 1.0f));
    assert(predictor.pendingCount() == 1);
}

void reconciles_by_replaying_unacked_inputs() {
    client::LocalMovementPredictor predictor;
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(2, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(3, 1.0f, 0.0f, 0.0f));
    assert(nearly(predictor.predicted().x, 3.0f));
    assert(predictor.reconcile(1.0f, 0.0f, 0.0f, 4, 1));
    assert(predictor.pendingCount() == 2);
    assert(nearly(predictor.predicted().x, 3.0f));
    assert(predictor.metrics().reconcileCount >= 1);
}

void rebases_pending_inputs_from_snapshot() {
    client::LocalMovementPredictor predictor;
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(3, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(4, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(5, 1.0f, 0.0f, 0.0f));
    assert(predictor.applySnapshotBaseline(10.0f, 0.0f, 0.0f, 8, 3));
    assert(predictor.pendingCount() == 2);
    assert(nearly(predictor.predicted().x, 12.0f));
    assert(nearly(predictor.rendered().x, 12.0f));
}

void reconverges_under_delay_and_packet_loss() {
    const float delaysMs[3] = {80.0f, 150.0f, 250.0f};
    for (int delayIndex = 0; delayIndex < 3; ++delayIndex) {
        client::LocalMovementPredictor predictor;
        predictor.reset(0.0f, 0.0f, 0.0f);
        for (uint64_t seq = 1; seq <= 5; ++seq) {
            assert(predictor.predict(seq, 1.0f, 0.0f, 0.0f));
        }
        const uint64_t framesDelayed =
            static_cast<uint64_t>(delaysMs[delayIndex] / 50.0f);
        const uint64_t acked = framesDelayed == 0 ? 1 : framesDelayed;
        const uint64_t clampedAck = acked > 5 ? 5 : acked;
        assert(predictor.reconcile(static_cast<float>(clampedAck), 0.0f, 0.0f,
                                   clampedAck, clampedAck));
        assert(nearly(predictor.predicted().x, 5.0f));
        (void)delaysMs;
    }

    client::LocalMovementPredictor lossy;
    lossy.reset(0.0f, 0.0f, 0.0f);
    for (uint64_t seq = 1; seq <= 5; ++seq) {
        assert(lossy.predict(seq, 1.0f, 0.0f, 0.0f));
    }
    assert(lossy.reconcile(5.0f, 0.0f, 0.0f, 9, 5));
    assert(lossy.pendingCount() == 0);
    assert(nearly(lossy.predicted().x, 5.0f));
}

void smooths_small_error_and_snaps_large_error() {
    client::LocalMovementPredictor smallError;
    smallError.reset(0.0f, 0.0f, 0.0f);
    assert(smallError.predict(1, 1.0f, 0.0f, 0.0f));
    assert(smallError.reconcile(0.8f, 0.0f, 0.0f, 1, 1));
    const float before = smallError.rendered().x;
    assert(std::fabs(before - smallError.predicted().x) > 0.0001f);
    smallError.stepRender(0.15f);
    assert(std::fabs(smallError.rendered().x - smallError.predicted().x) < 0.05f);

    client::LocalMovementPredictor largeError;
    largeError.reset(0.0f, 0.0f, 0.0f);
    assert(largeError.predict(1, 1.0f, 0.0f, 0.0f));
    assert(largeError.reconcile(80.0f, 0.0f, 0.0f, 1, 1));
    assert(nearly(largeError.rendered().x, 80.0f));
    assert(nearly(largeError.predicted().x, 80.0f));
}

} // namespace local_movement_predictor_tests
