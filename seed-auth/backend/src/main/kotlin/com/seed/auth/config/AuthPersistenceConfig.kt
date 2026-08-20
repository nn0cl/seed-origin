package com.seed.auth.config

import com.seed.auth.adapter.mybatis.MyBatisPasswordCodec
import com.seed.auth.adapter.mybatis.MyBatisPlayerChallengeStore
import com.seed.auth.adapter.mybatis.MyBatisPlayerSessionStore
import com.seed.auth.adapter.mybatis.MyBatisSupport
import com.seed.auth.adapter.mybatis.MyBatisUserStore
import com.seed.auth.adapter.mybatis.PasswordMapper
import com.seed.auth.adapter.mybatis.PlayerChallengeMapper
import com.seed.auth.adapter.mybatis.PlayerSessionMapper
import com.seed.auth.adapter.mybatis.UserMapper
import com.seed.auth.application.LoginLockoutPolicy
import com.seed.auth.application.PlayerAuthenticationService
import com.seed.auth.application.port.Clock
import com.seed.auth.application.port.PasswordCodec
import com.seed.auth.application.port.PlayerChallengeStore
import com.seed.auth.application.port.PlayerSessionStore
import com.seed.auth.application.port.UserStore
import java.time.Instant
import javax.sql.DataSource
import org.mybatis.spring.annotation.MapperScan
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty
import org.springframework.context.annotation.Bean
import org.springframework.context.annotation.Configuration

/**
 * Wires MyBatis mappers (LISS-0156) into the UseCase for runtime.
 * Activated when SEED_IDENTITY_DB_URL is present.
 */
@Configuration
@ConditionalOnProperty(name = ["SEED_IDENTITY_DB_URL"])
@MapperScan("com.seed.auth.adapter.mybatis")
class AuthPersistenceConfig {
    @Bean
    fun dataSource(): DataSource = MyBatisSupport.dataSourceFromEnvironment()

    @Bean
    fun userStore(users: UserMapper): UserStore = MyBatisUserStore(users)

    @Bean
    fun playerChallengeStore(challenges: PlayerChallengeMapper): PlayerChallengeStore =
        MyBatisPlayerChallengeStore(challenges)

    @Bean
    fun playerSessionStore(sessions: PlayerSessionMapper): PlayerSessionStore =
        MyBatisPlayerSessionStore(sessions)

    @Bean
    fun passwordCodec(passwords: PasswordMapper): PasswordCodec =
        MyBatisPasswordCodec(passwords)

    @Bean
    fun systemClock(): Clock = object : Clock {
        override fun now(): Instant = Instant.now()
    }

    @Bean
    fun loginLockoutPolicy(): LoginLockoutPolicy =
        LoginLockoutPolicy(maxFailures = 3, durationSeconds = 15 * 60)

    @Bean
    fun playerAuthenticationService(
        userStore: UserStore,
        playerChallengeStore: PlayerChallengeStore,
        playerSessionStore: PlayerSessionStore,
        clock: Clock,
        passwordCodec: PasswordCodec,
        loginLockoutPolicy: LoginLockoutPolicy,
    ): PlayerAuthenticationService =
        PlayerAuthenticationService(
            userStore = userStore,
            playerChallengeStore = playerChallengeStore,
            playerSessionStore = playerSessionStore,
            clock = clock,
            passwordCodec = passwordCodec,
            lockoutPolicy = loginLockoutPolicy,
        )
}
