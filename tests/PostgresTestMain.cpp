namespace postgres_identity_alias_store_tests {
void round_trips_records_through_postgres();
}

namespace postgres_player_session_store_tests {
void claims_valid_unexpired_challenge();
void rejects_expired_challenge();
void rejects_already_claimed_challenge();
void login_use_case_creates_player_session_row();
void extends_active_session();
void is_active_rejects_expired_session();
}

namespace postgres_challenge_server_bootstrap_tests {
void fails_when_identity_db_url_is_unset();
void creates_challenge_dispatcher_when_env_and_db_are_set();
}

namespace postgres_player_progression_store_tests {
void grants_base_experience_independently_of_job();
void grants_job_experience_independently_of_base();
void creates_equipment_instance_with_durability_and_exp();
void sockets_item_into_equipment();
void persists_stamina_on_stamina_event();
void reads_base_exp_threshold_from_master_data();
}

int main() {
    postgres_identity_alias_store_tests::round_trips_records_through_postgres();
    postgres_player_session_store_tests::claims_valid_unexpired_challenge();
    postgres_player_session_store_tests::rejects_expired_challenge();
    postgres_player_session_store_tests::rejects_already_claimed_challenge();
    postgres_player_session_store_tests::login_use_case_creates_player_session_row();
    postgres_player_session_store_tests::extends_active_session();
    postgres_player_session_store_tests::is_active_rejects_expired_session();
    postgres_challenge_server_bootstrap_tests::fails_when_identity_db_url_is_unset();
    postgres_challenge_server_bootstrap_tests::creates_challenge_dispatcher_when_env_and_db_are_set();
    postgres_player_progression_store_tests::grants_base_experience_independently_of_job();
    postgres_player_progression_store_tests::grants_job_experience_independently_of_base();
    postgres_player_progression_store_tests::creates_equipment_instance_with_durability_and_exp();
    postgres_player_progression_store_tests::sockets_item_into_equipment();
    postgres_player_progression_store_tests::persists_stamina_on_stamina_event();
    postgres_player_progression_store_tests::reads_base_exp_threshold_from_master_data();
    return 0;
}
