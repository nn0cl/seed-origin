package com.seed.auth.adapter.postgres

import com.seed.auth.application.port.PlayerSessionStore
import com.seed.auth.domain.PlayerSessionKey
import java.sql.Connection
import java.time.Instant

class PostgresPlayerSessionStore(
    private val openConnection: () -> Connection = PostgresConnections::openFromEnvironment,
) : PlayerSessionStore {
    override fun revoke(key: PlayerSessionKey) {
        openConnection.update(
            sql = """
                UPDATE player_sessions
                   SET revoked_at = now()
                 WHERE session_token = ?
                   AND revoked_at IS NULL
                """.trimIndent(),
            bind = { setString(1, key.value) },
        )
    }

    fun insertActiveForTest(userId: Long, key: PlayerSessionKey, expiresAt: Instant) {
        val updated = openConnection.update(
            sql = """
                INSERT INTO player_sessions (session_token, user_id, expires_at)
                VALUES (?, ?, ?)
                """.trimIndent(),
            bind = {
                setString(1, key.value)
                setLong(2, userId)
                setTimestamp(3, expiresAt.toSqlTimestamp())
            },
        )
        check(updated == 1) { "failed to insert session" }
    }

    fun isActive(key: PlayerSessionKey, now: Instant): Boolean =
        openConnection.query(
            sql = """
                SELECT 1 FROM player_sessions
                WHERE session_token = ?
                  AND revoked_at IS NULL
                  AND expires_at > ?
                """.trimIndent(),
            bind = {
                setString(1, key.value)
                setTimestamp(2, now.toSqlTimestamp())
            },
        ) { it.hasRow() }

    companion object {
        fun fromEnvironment(): PostgresPlayerSessionStore =
            PostgresPlayerSessionStore()
    }
}
