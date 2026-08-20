package com.seed.auth.application

import com.seed.auth.application.port.Clock
import com.seed.auth.application.port.PasswordCodec
import com.seed.auth.application.port.PlayerChallengeStore
import com.seed.auth.application.port.PlayerSessionStore
import com.seed.auth.application.port.UserStore
import com.seed.auth.domain.ChallengeKey
import com.seed.auth.domain.PlayerSessionKey
import java.time.Instant
import java.util.UUID

data class RegisterUserCommand(val username: String, val password: String)

data class NativeLoginCommand(val username: String, val password: String)

data class LogoutCommand(val sessionKey: PlayerSessionKey)

data class RegisterResult(
    val accepted: Boolean,
    val reason: String? = null,
)

data class LoginResult(
    val accepted: Boolean,
    val challengeKey: ChallengeKey? = null,
    val reason: String? = null,
    val lockedUntil: Instant? = null,
)

data class LogoutResult(
    val accepted: Boolean,
    val reason: String? = null,
)

data class LoginLockoutPolicy(
    val maxFailures: Int,
    val durationSeconds: Long,
)

class PlayerAuthenticationService(
    private val userStore: UserStore,
    private val playerChallengeStore: PlayerChallengeStore,
    private val playerSessionStore: PlayerSessionStore,
    private val clock: Clock,
    private val passwordCodec: PasswordCodec,
    private val lockoutPolicy: LoginLockoutPolicy,
) {
    private val failures = mutableMapOf<String, FailureState>()

    fun register(command: RegisterUserCommand): RegisterResult {
        if (userStore.exists(command.username)) {
            return RegisterResult(accepted = false, reason = "duplicate_username")
        }

        userStore.save(command.username, passwordCodec.hash(command.password))
        return RegisterResult(accepted = true)
    }

    fun login(command: NativeLoginCommand): LoginResult {
        val current = clock.now()
        val state = failures[command.username]
        if (state != null && current.isBefore(state.lockedUntil)) {
            return LoginResult(
                accepted = false,
                reason = "locked_out",
                lockedUntil = state.lockedUntil,
            )
        }

        val storedUser = userStore.find(command.username)
        if (storedUser == null || !passwordCodec.matches(command.password, storedUser.passwordHash)) {
            recordFailure(command.username, current)
            val updated = failures.getValue(command.username)
            return LoginResult(
                accepted = false,
                reason = if (updated.lockedUntil.isAfter(current)) "locked_out" else "invalid_credentials",
                lockedUntil = updated.lockedUntil.takeIf { it.isAfter(current) },
            )
        }

        failures.remove(command.username)
        val challengeKey = ChallengeKey(UUID.randomUUID().toString())
        playerChallengeStore.issue(
            userId = storedUser.userId,
            key = challengeKey,
            expiresAt = current.plusSeconds(120),
        )
        return LoginResult(accepted = true, challengeKey = challengeKey)
    }

    fun logout(command: LogoutCommand): LogoutResult {
        playerSessionStore.revoke(command.sessionKey)
        return LogoutResult(accepted = true)
    }

    private fun recordFailure(username: String, current: Instant) {
        val previous = failures[username]
        val count = (previous?.count ?: 0) + 1
        val lockedUntil = if (count >= lockoutPolicy.maxFailures) {
            current.plusSeconds(lockoutPolicy.durationSeconds)
        } else {
            Instant.MIN
        }
        failures[username] = FailureState(count, lockedUntil)
    }

    private data class FailureState(val count: Int, val lockedUntil: Instant)
}
