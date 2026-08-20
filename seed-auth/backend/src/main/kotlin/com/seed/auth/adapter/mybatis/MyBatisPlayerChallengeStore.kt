package com.seed.auth.adapter.mybatis

import com.seed.auth.application.port.PlayerChallengeStore
import com.seed.auth.domain.ChallengeKey
import java.time.Instant

class MyBatisPlayerChallengeStore(
    private val challenges: PlayerChallengeMapper,
) : PlayerChallengeStore {
    override fun issue(userId: Long, key: ChallengeKey, expiresAt: Instant) {
        check(challenges.insert(key.value, userId, expiresAt) == 1) {
            "failed to insert challenge"
        }
    }

    fun existsUnclaimed(key: ChallengeKey): Boolean =
        challenges.existsUnclaimedRow(key.value) != null

    companion object {
        fun fromEnvironment(): MyBatisPlayerChallengeStore =
            MyBatisPlayerChallengeStore(
                MyBatisSupport.sessionScopedMapper(PlayerChallengeMapper::class.java),
            )
    }
}
