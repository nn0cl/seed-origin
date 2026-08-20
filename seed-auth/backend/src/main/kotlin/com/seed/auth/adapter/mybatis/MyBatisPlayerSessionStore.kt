package com.seed.auth.adapter.mybatis

import com.seed.auth.application.port.PlayerSessionStore
import com.seed.auth.domain.PlayerSessionKey
import java.time.Instant

class MyBatisPlayerSessionStore(
    private val sessions: PlayerSessionMapper,
) : PlayerSessionStore {
    override fun revoke(key: PlayerSessionKey) {
        sessions.revoke(key.value)
    }

    fun insertActiveForTest(userId: Long, key: PlayerSessionKey, expiresAt: Instant) {
        check(sessions.insert(key.value, userId, expiresAt) == 1) {
            "failed to insert session"
        }
    }

    fun isActive(key: PlayerSessionKey, now: Instant): Boolean =
        sessions.isActiveRow(key.value, now) != null

    companion object {
        fun fromEnvironment(): MyBatisPlayerSessionStore =
            MyBatisPlayerSessionStore(
                MyBatisSupport.sessionScopedMapper(PlayerSessionMapper::class.java),
            )
    }
}
