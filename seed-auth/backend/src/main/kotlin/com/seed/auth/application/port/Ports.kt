package com.seed.auth.application.port

import com.seed.auth.domain.ChallengeKey
import com.seed.auth.domain.PlayerSessionKey
import java.time.Instant

interface Clock {
    fun now(): Instant
}

interface PasswordCodec {
    fun hash(password: String): String
    fun matches(password: String, passwordHash: String): Boolean
}

interface UserStore {
    fun exists(username: String): Boolean
    fun save(username: String, passwordHash: String)
    fun find(username: String): StoredUser?
}

data class StoredUser(
    val userId: Long,
    val username: String,
    val passwordHash: String,
)

interface PlayerChallengeStore {
    fun issue(userId: Long, key: ChallengeKey, expiresAt: Instant)
}

interface PlayerSessionStore {
    fun revoke(key: PlayerSessionKey)
}
