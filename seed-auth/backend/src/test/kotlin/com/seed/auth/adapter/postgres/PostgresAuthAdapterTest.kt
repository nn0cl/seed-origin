package com.seed.auth.adapter.postgres

import com.seed.auth.domain.ChallengeKey
import com.seed.auth.domain.PlayerSessionKey
import java.time.Instant
import java.util.UUID
import org.junit.jupiter.api.Assumptions.assumeTrue
import org.junit.jupiter.api.Test
import kotlin.test.assertEquals
import kotlin.test.assertFalse
import kotlin.test.assertNotEquals
import kotlin.test.assertNotNull
import kotlin.test.assertTrue

/**
 * LISS-0151 Phase 1 Red — Postgres + pgcrypto adapters for seed_auth ports.
 *
 * Skips when SEED_IDENTITY_DB_URL is unset (same convention as C++ postgres tests).
 * Expected Red before Green: unresolved references to adapter types.
 */
class PostgresAuthAdapterTest {
    @Test
    fun saves_user_with_pgcrypto_hash_and_finds_by_username() {
        assumeEnv()
        val username = "adapter-user-${UUID.randomUUID()}"
        val password = "secret-password"
        val codec = PgcryptoPasswordCodec.fromEnvironment()
        val users = PostgresUserStore.fromEnvironment()

        assertFalse(users.exists(username))
        users.save(username, codec.hash(password))
        assertTrue(users.exists(username))

        val stored = users.find(username)
        assertNotNull(stored)
        assertEquals(username, stored.username)
        assertNotEquals(password, stored.passwordHash)
        assertTrue(codec.matches(password, stored.passwordHash))
        assertFalse(codec.matches("wrong", stored.passwordHash))
    }

    @Test
    fun issues_player_challenge_row() {
        assumeEnv()
        val username = "challenge-user-${UUID.randomUUID()}"
        val codec = PgcryptoPasswordCodec.fromEnvironment()
        val users = PostgresUserStore.fromEnvironment()
        users.save(username, codec.hash("secret"))
        val userId = users.find(username)!!.userId

        val challenges = PostgresPlayerChallengeStore.fromEnvironment()
        val key = ChallengeKey("challenge-${UUID.randomUUID()}")
        val expiresAt = Instant.parse("2026-08-20T12:00:00Z")
        challenges.issue(userId, key, expiresAt)

        assertTrue(challenges.existsUnclaimed(key))
    }

    @Test
    fun revoke_marks_player_session_revoked() {
        assumeEnv()
        val username = "session-user-${UUID.randomUUID()}"
        val codec = PgcryptoPasswordCodec.fromEnvironment()
        val users = PostgresUserStore.fromEnvironment()
        users.save(username, codec.hash("secret"))
        val userId = users.find(username)!!.userId

        val sessions = PostgresPlayerSessionStore.fromEnvironment()
        val key = PlayerSessionKey("session-${UUID.randomUUID()}")
        sessions.insertActiveForTest(userId, key, Instant.parse("2026-08-20T13:00:00Z"))
        assertTrue(sessions.isActive(key, Instant.parse("2026-08-20T12:00:00Z")))

        sessions.revoke(key)
        assertFalse(sessions.isActive(key, Instant.parse("2026-08-20T12:00:00Z")))
    }

    private fun assumeEnv() {
        val url = System.getenv("SEED_IDENTITY_DB_URL")
        assumeTrue(
            !url.isNullOrBlank(),
            "SEED_IDENTITY_DB_URL not set; skipping Postgres auth adapter test",
        )
    }
}
