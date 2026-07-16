namespace action_input_tests {
void rejects_missing_players_for_target_action();
void permits_field_action_without_players();
void rejects_unknown_action_type();
}

namespace field_state_tests {
void removes_players_by_full_id();
void accepts_empty_frame();
}

namespace connection_tests {
void close_is_idempotent();
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

namespace network_command_tests {
void validates_login_without_client_internal_id();
void rejects_invalid_session_and_oversized_payload();
}

namespace login_command_handler_tests {
void creates_temporary_session_from_valid_login();
void rejects_client_supplied_internal_id();
}

namespace server_runtime_tests {
void rejects_commands_before_start();
void drains_valid_commands_in_fifo_order();
void dispatches_pending_commands_in_fifo_order();
}

namespace network_frame_tests {
void round_trips_a_valid_command();
void rejects_incomplete_and_oversized_frames();
}

namespace frame_accumulator_tests {
void joins_partial_frame_and_preserves_multiple_frames();
}

namespace server_command_dispatcher_tests {
void accepts_login();
void rejects_unimplemented_command();
}

int main() {
    action_input_tests::rejects_missing_players_for_target_action();
    action_input_tests::permits_field_action_without_players();
    action_input_tests::rejects_unknown_action_type();
    field_state_tests::removes_players_by_full_id();
    field_state_tests::accepts_empty_frame();
    connection_tests::close_is_idempotent();
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
    network_command_tests::validates_login_without_client_internal_id();
    network_command_tests::rejects_invalid_session_and_oversized_payload();
    login_command_handler_tests::creates_temporary_session_from_valid_login();
    login_command_handler_tests::rejects_client_supplied_internal_id();
    server_runtime_tests::rejects_commands_before_start();
    server_runtime_tests::drains_valid_commands_in_fifo_order();
    server_runtime_tests::dispatches_pending_commands_in_fifo_order();
    network_frame_tests::round_trips_a_valid_command();
    network_frame_tests::rejects_incomplete_and_oversized_frames();
    frame_accumulator_tests::joins_partial_frame_and_preserves_multiple_frames();
    server_command_dispatcher_tests::accepts_login();
    server_command_dispatcher_tests::rejects_unimplemented_command();
    return 0;
}
