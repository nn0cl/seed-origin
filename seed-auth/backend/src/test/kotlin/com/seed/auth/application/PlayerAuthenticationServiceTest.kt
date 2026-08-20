package com.seed.auth.application

import com.seed.auth.application.port.Clock
import com.seed.auth.application.port.PasswordCodec
import com.seed.auth.application.port.PlayerChallengeStore
import com.seed.auth.application.port.PlayerSessionStore
import com.seed.auth.application.port.StoredUser
import com.seed.auth.application.port.UserStore
import com.seed.auth.domain.ChallengeKey
import com.seed.auth.domain.PlayerSessionKey
import java.time.Instant
import org.junit.jupiter.api.Test
import kotlin.test.assertEquals
import kotlin.test.assertFalse
import kotlin.test.assertNotEquals
import kotlin.test.assertNull
import kotlin.test.assertTrue

/**
 * LISS-0146 acceptance tests carried forward from the Phase 1 Red review.
 *
 * Ports are supplied as fakes so the scenarios remain focused on the
 * application boundary and do not require PostgreSQL, HTTP, Docker, or a
 * real clock.
 */
class PlayerAuthenticationServiceTest {
    private val now = Instant.parse("2026-07-23T00:00:00Z")
    private val clock = FixedClock(now)
    private val users = FakeUserStore()
    private val challenges = FakePlayerChallengeStore()
    private val sessions = FakePlayerSessionStore()
    private val service = PlayerAuthenticationService(
        userStore = users,
        playerChallengeStore = challenges,
        playerSessionStore = sessions,
        clock = clock,
        passwordCodec = FakePasswordCodec(),
        lockoutPolicy = LoginLockoutPolicy(maxFailures = 3, durationSeconds = 15 * 60),
    )

    @Test
    fun registers_user_with_hashed_password() {
        val result = service.register(
            RegisterUserCommand(username = "alice", password = "secret"),
        )

        assertTrue(result.accepted)
        assertEquals("alice", users.savedUsername)
        assertNotEquals("secret", users.savedPasswordHash)
    }

    @Test
    fun rejects_duplicate_username_without_creating_a_second_user() {
        users.existingUsernames += "alice"

        val result = service.register(
            RegisterUserCommand(username = "alice", password = "secret"),
        )

        assertFalse(result.accepted)
        assertEquals("duplicate_username", result.reason)
        assertNull(users.savedUsername)
    }

    @Test
    fun issues_a_single_use_challenge_for_valid_native_login() {
        users.addUser(username = "alice", password = "secret")

        val result = service.login(
            NativeLoginCommand(username = "alice", password = "secret"),
        )

        assertTrue(result.accepted)
        assertEquals(challenges.issuedKey, result.challengeKey)
        assertEquals(now.plusSeconds(120), challenges.issuedExpiresAt)
        assertNull(sessions.createdSession)
    }

    @Test
    fun rejects_invalid_login_without_issuing_a_challenge() {
        users.addUser(username = "alice", password = "secret")

        val result = service.login(
            NativeLoginCommand(username = "alice", password = "wrong"),
        )

        assertFalse(result.accepted)
        assertNull(result.challengeKey)
        assertNull(challenges.issuedKey)
    }

    @Test
    fun locks_login_after_three_consecutive_failures_for_fifteen_minutes() {
        users.addUser(username = "alice", password = "secret")

        repeat(3) {
            service.login(NativeLoginCommand(username = "alice", password = "wrong"))
        }

        val result = service.login(
            NativeLoginCommand(username = "alice", password = "secret"),
        )

        assertFalse(result.accepted)
        assertEquals("locked_out", result.reason)
        assertEquals(now.plusSeconds(15 * 60), result.lockedUntil)
        assertNull(challenges.issuedKey)
    }

    @Test
    fun logout_revokes_the_player_session() {
        val sessionKey = PlayerSessionKey("session-token")
        sessions.activeKeys += sessionKey

        val result = service.logout(LogoutCommand(sessionKey))

        assertTrue(result.accepted)
        assertTrue(sessions.revokedKeys.contains(sessionKey))
    }
}

private class FixedClock(private val current: Instant) : Clock {
    override fun now(): Instant = current
}

private class FakeUserStore : UserStore {
    val existingUsernames = mutableSetOf<String>()
    var savedUsername: String? = null
    var savedPasswordHash: String? = null

    fun addUser(username: String, password: String) {
        existingUsernames += username
        savedPasswordHash = "hash:$password"
    }

    override fun exists(username: String): Boolean = username in existingUsernames

    override fun save(username: String, passwordHash: String) {
        existingUsernames += username
        savedUsername = username
        savedPasswordHash = passwordHash
    }

    override fun find(username: String): StoredUser? =
        if (username in existingUsernames) {
            StoredUser(1L, username, savedPasswordHash ?: "")
        } else {
            null
        }
}

private class FakePlayerChallengeStore : PlayerChallengeStore {
    var issuedKey: ChallengeKey? = null
    var issuedExpiresAt: Instant? = null

    override fun issue(userId: Long, key: ChallengeKey, expiresAt: Instant) {
        issuedKey = key
        issuedExpiresAt = expiresAt
    }
}

private class FakePlayerSessionStore : PlayerSessionStore {
    val activeKeys = mutableSetOf<PlayerSessionKey>()
    val revokedKeys = mutableSetOf<PlayerSessionKey>()
    var createdSession: PlayerSessionKey? = null

    override fun revoke(key: PlayerSessionKey) {
        revokedKeys += key
    }
}

private class FakePasswordCodec : PasswordCodec {
    override fun hash(password: String): String = "hash:$password"

    override fun matches(password: String, passwordHash: String): Boolean =
        hash(password) == passwordHash
}
