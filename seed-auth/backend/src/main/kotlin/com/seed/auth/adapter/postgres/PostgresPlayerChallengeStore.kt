package com.seed.auth.adapter.postgres

import com.seed.auth.application.port.PlayerChallengeStore
import com.seed.auth.domain.ChallengeKey
import java.sql.Connection
import java.time.Instant

class PostgresPlayerChallengeStore(
    private val openConnection: () -> Connection = PostgresConnections::openFromEnvironment,
) : PlayerChallengeStore {
    override fun issue(userId: Long, key: ChallengeKey, expiresAt: Instant) {
        val updated = openConnection.update(
            sql = """
                INSERT INTO player_challenges (challenge_key, user_id, expires_at)
                VALUES (?, ?, ?)
                """.trimIndent(),
            bind = {
                setString(1, key.value)
                setLong(2, userId)
                setTimestamp(3, expiresAt.toSqlTimestamp())
            },
        )
        check(updated == 1) { "failed to insert challenge" }
    }

    fun existsUnclaimed(key: ChallengeKey): Boolean =
        openConnection.query(
            sql = """
                SELECT 1 FROM player_challenges
                WHERE challenge_key = ?
                  AND claimed_at IS NULL
                """.trimIndent(),
            bind = { setString(1, key.value) },
        ) { it.hasRow() }

    companion object {
        fun fromEnvironment(): PostgresPlayerChallengeStore =
            PostgresPlayerChallengeStore()
    }
}
