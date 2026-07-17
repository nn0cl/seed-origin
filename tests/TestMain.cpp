namespace action_input_tests {
void rejects_missing_players_for_target_action();
void permits_field_action_without_players();
void rejects_unknown_action_type();
}

namespace action_queue_tests {
void assigns_arrival_sequence_and_preserves_order();
void arrivals_after_frame_cutover_are_deferred();
void rejects_invalid_actions();
}

namespace server_tick_tests {
void advances_fixed_logical_frames_and_cuts_queue();
}

namespace world_update_tests {
void accepts_snapshot_and_event_envelopes();
void rejects_invalid_update_identity();
}

namespace world_update_sequence_tests {
void accepts_ordered_updates_and_initial_snapshot();
void detects_duplicates_and_gaps_without_advancing();
}

namespace world_update_sync_tests {
void requests_snapshot_on_gap_and_does_not_apply_it();
void resumes_after_snapshot_confirmation();
}

namespace field_state_tests {
void removes_players_by_full_id();
void accepts_empty_frame();
}

namespace connection_tests {
void close_is_idempotent();
void reports_no_pending_client_without_blocking();
}

namespace client_session_tests {
void receives_complete_commands_and_closes_peer_cleanly();
void flushes_one_queued_response_to_peer();
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
void ingests_decoded_commands_from_client_session();
void dispatches_ingested_login_to_session_registry();
void rejects_client_accept_when_runtime_is_stopped();
void clears_owned_clients_on_stop();
void rejects_client_frame_processing_when_runtime_is_stopped();
void rejects_frame_processing_when_runtime_is_stopped();
void advances_action_frame_after_network_processing();
}

namespace network_frame_tests {
void round_trips_a_valid_command();
void rejects_incomplete_and_oversized_frames();
}

namespace frame_accumulator_tests {
void joins_partial_frame_and_preserves_multiple_frames();
}

namespace login_response_codec_tests {
void round_trips_accepted_response();
void round_trips_rejected_response();
void rejects_invalid_response_identity();
}

namespace outbound_frame_queue_tests {
void preserves_fifo_order();
void rejects_invalid_frame_without_mutation();
void rejects_frame_after_capacity_without_mutation();
void consumes_partial_front_without_reordering();
}

namespace session_lifecycle_tests {
void rejects_duplicate_login_on_one_connection();
void logs_out_bound_session_on_disconnect();
void rejects_invalid_binding_without_mutation();
void clears_all_bound_sessions();
}

namespace world_frame_update_builder_tests {
void converts_actions_to_ordered_events();
void emits_no_update_for_empty_frame();
}

namespace server_command_dispatcher_tests {
void accepts_login();
void rejects_unimplemented_command();
}

namespace movement_command_handler_tests {
void rejects_malformed_or_unknown_move();
void accepts_bounded_move_for_existing_player();
}

int main() {
    action_input_tests::rejects_missing_players_for_target_action();
    action_input_tests::permits_field_action_without_players();
    action_input_tests::rejects_unknown_action_type();
    action_queue_tests::assigns_arrival_sequence_and_preserves_order();
    action_queue_tests::arrivals_after_frame_cutover_are_deferred();
    action_queue_tests::rejects_invalid_actions();
    server_tick_tests::advances_fixed_logical_frames_and_cuts_queue();
    world_update_tests::accepts_snapshot_and_event_envelopes();
    world_update_tests::rejects_invalid_update_identity();
    world_update_sequence_tests::accepts_ordered_updates_and_initial_snapshot();
    world_update_sequence_tests::detects_duplicates_and_gaps_without_advancing();
    world_update_sync_tests::requests_snapshot_on_gap_and_does_not_apply_it();
    world_update_sync_tests::resumes_after_snapshot_confirmation();
    field_state_tests::removes_players_by_full_id();
    field_state_tests::accepts_empty_frame();
    connection_tests::close_is_idempotent();
    connection_tests::reports_no_pending_client_without_blocking();
    client_session_tests::receives_complete_commands_and_closes_peer_cleanly();
    client_session_tests::flushes_one_queued_response_to_peer();
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
    server_runtime_tests::ingests_decoded_commands_from_client_session();
    server_runtime_tests::dispatches_ingested_login_to_session_registry();
    server_runtime_tests::rejects_client_accept_when_runtime_is_stopped();
    server_runtime_tests::clears_owned_clients_on_stop();
    server_runtime_tests::rejects_client_frame_processing_when_runtime_is_stopped();
    server_runtime_tests::rejects_frame_processing_when_runtime_is_stopped();
    server_runtime_tests::advances_action_frame_after_network_processing();
    network_frame_tests::round_trips_a_valid_command();
    network_frame_tests::rejects_incomplete_and_oversized_frames();
    login_response_codec_tests::round_trips_accepted_response();
    login_response_codec_tests::round_trips_rejected_response();
    login_response_codec_tests::rejects_invalid_response_identity();
    outbound_frame_queue_tests::preserves_fifo_order();
    outbound_frame_queue_tests::rejects_invalid_frame_without_mutation();
    outbound_frame_queue_tests::rejects_frame_after_capacity_without_mutation();
    outbound_frame_queue_tests::consumes_partial_front_without_reordering();
    session_lifecycle_tests::rejects_duplicate_login_on_one_connection();
    session_lifecycle_tests::logs_out_bound_session_on_disconnect();
    session_lifecycle_tests::rejects_invalid_binding_without_mutation();
    session_lifecycle_tests::clears_all_bound_sessions();
    world_frame_update_builder_tests::converts_actions_to_ordered_events();
    world_frame_update_builder_tests::emits_no_update_for_empty_frame();
    frame_accumulator_tests::joins_partial_frame_and_preserves_multiple_frames();
    server_command_dispatcher_tests::accepts_login();
    server_command_dispatcher_tests::rejects_unimplemented_command();
    movement_command_handler_tests::rejects_malformed_or_unknown_move();
    movement_command_handler_tests::accepts_bounded_move_for_existing_player();
    return 0;
}
