package com.seed.auth.config

import com.seed.auth.adapter.postgres.PgcryptoPasswordCodec
import com.seed.auth.adapter.postgres.PostgresPlayerChallengeStore
import com.seed.auth.adapter.postgres.PostgresPlayerSessionStore
import com.seed.auth.adapter.postgres.PostgresUserStore
import com.seed.auth.application.LoginLockoutPolicy
import com.seed.auth.application.PlayerAuthenticationService
import com.seed.auth.application.port.Clock
import com.seed.auth.application.port.PasswordCodec
import com.seed.auth.application.port.PlayerChallengeStore
import com.seed.auth.application.port.PlayerSessionStore
import com.seed.auth.application.port.UserStore
import java.time.Instant
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty
import org.springframework.context.annotation.Bean
import org.springframework.context.annotation.Configuration

/**
 * Wires LISS-0151 JDBC/pgcrypto adapters into the UseCase for runtime.
 * Activated when SEED_IDENTITY_DB_URL is present (same env as C++/adapter tests).
 */
@Configuration
@ConditionalOnProperty(name = ["SEED_IDENTITY_DB_URL"])
class AuthPersistenceConfig {
    @Bean
    fun userStore(): UserStore = PostgresUserStore.fromEnvironment()

    @Bean
    fun playerChallengeStore(): PlayerChallengeStore =
        PostgresPlayerChallengeStore.fromEnvironment()

    @Bean
    fun playerSessionStore(): PlayerSessionStore =
        PostgresPlayerSessionStore.fromEnvironment()

    @Bean
    fun passwordCodec(): PasswordCodec = PgcryptoPasswordCodec.fromEnvironment()

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
