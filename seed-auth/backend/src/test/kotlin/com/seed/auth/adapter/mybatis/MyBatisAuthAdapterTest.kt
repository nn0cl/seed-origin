package com.seed.auth.adapter.mybatis

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
 * LISS-0156 Phase 1 Red — MyBatis + pgcrypto port adapters for seed_auth.
 *
 * Same acceptance scenarios as LISS-0151 Postgres JDBC adapters, but through
 * MyBatis-backed types. Skips when SEED_IDENTITY_DB_URL is unset.
 *
 * Expected Red before Green: unresolved references to MyBatis adapter types
 * (and Boot 4.1 / MyBatis 4.1 wiring not yet applied).
 */
class MyBatisAuthAdapterTest {
    @Test
    fun saves_user_with_pgcrypto_hash_and_finds_by_username() {
        assumeEnv()
        val username = "mybatis-user-${UUID.randomUUID()}"
        val password = "secret-password"
        val codec = MyBatisPasswordCodec.fromEnvironment()
        val users = MyBatisUserStore.fromEnvironment()

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
        val username = "mybatis-challenge-${UUID.randomUUID()}"
        val codec = MyBatisPasswordCodec.fromEnvironment()
        val users = MyBatisUserStore.fromEnvironment()
        users.save(username, codec.hash("secret"))
        val userId = users.find(username)!!.userId

        val challenges = MyBatisPlayerChallengeStore.fromEnvironment()
        val key = ChallengeKey("challenge-${UUID.randomUUID()}")
        val expiresAt = Instant.parse("2026-08-20T12:00:00Z")
        challenges.issue(userId, key, expiresAt)

        assertTrue(challenges.existsUnclaimed(key))
    }

    @Test
    fun revoke_marks_player_session_revoked() {
        assumeEnv()
        val username = "mybatis-session-${UUID.randomUUID()}"
        val codec = MyBatisPasswordCodec.fromEnvironment()
        val users = MyBatisUserStore.fromEnvironment()
        users.save(username, codec.hash("secret"))
        val userId = users.find(username)!!.userId

        val sessions = MyBatisPlayerSessionStore.fromEnvironment()
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
            "SEED_IDENTITY_DB_URL not set; skipping MyBatis auth adapter test",
        )
    }
}
