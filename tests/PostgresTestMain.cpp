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

int main() {
    postgres_identity_alias_store_tests::round_trips_records_through_postgres();
    postgres_player_session_store_tests::claims_valid_unexpired_challenge();
    postgres_player_session_store_tests::rejects_expired_challenge();
    postgres_player_session_store_tests::rejects_already_claimed_challenge();
    postgres_player_session_store_tests::login_use_case_creates_player_session_row();
    postgres_player_session_store_tests::extends_active_session();
    postgres_player_session_store_tests::is_active_rejects_expired_session();
    return 0;
}
