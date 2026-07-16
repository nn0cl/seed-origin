namespace action_input_tests {
void rejects_missing_players_for_target_action();
void permits_field_action_without_players();
void rejects_unknown_action_type();
}

namespace field_state_tests {
void removes_players_by_full_id();
void accepts_empty_frame();
}

void playerOwnershipCopyAssignmentTest();
void playerOwnershipContainerUpdateTest();

namespace seed_binary_tests {
void rejects_out_of_range_input();
void zero_fills_unused_block_bytes();
void rejects_missing_block_on_save();
}

namespace status_saturation_tests {
void saturates_positive_values();
void saturates_negative_values();
}

namespace session_registry_tests {
void assigns_unique_internal_ids_and_stable_aliases();
void treats_invalid_claims_as_anonymous();
void never_reuses_disconnected_ids();
}

int main() {
    action_input_tests::rejects_missing_players_for_target_action();
    action_input_tests::permits_field_action_without_players();
    action_input_tests::rejects_unknown_action_type();
    field_state_tests::removes_players_by_full_id();
    field_state_tests::accepts_empty_frame();
    player_ownership_tests::playerOwnershipCopyAssignmentTest();
    player_ownership_tests::playerOwnershipContainerUpdateTest();
    seed_binary_tests::rejects_out_of_range_input();
    seed_binary_tests::zero_fills_unused_block_bytes();
    seed_binary_tests::rejects_missing_block_on_save();
    status_saturation_tests::saturates_positive_values();
    status_saturation_tests::saturates_negative_values();
    session_registry_tests::assigns_unique_internal_ids_and_stable_aliases();
    session_registry_tests::treats_invalid_claims_as_anonymous();
    session_registry_tests::never_reuses_disconnected_ids();
    return 0;
}
