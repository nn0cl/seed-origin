package com.seed.auth.adapter.http

import com.seed.auth.application.LoginResult
import com.seed.auth.application.LogoutResult
import com.seed.auth.application.NativeLoginCommand
import com.seed.auth.application.PlayerAuthenticationService
import com.seed.auth.application.RegisterResult
import com.seed.auth.application.RegisterUserCommand
import com.seed.auth.application.LogoutCommand
import com.seed.auth.domain.ChallengeKey
import java.time.Instant
import org.springframework.boot.webmvc.test.autoconfigure.WebMvcTest
import org.springframework.http.MediaType
import org.springframework.test.context.bean.override.mockito.MockitoBean
import org.springframework.test.web.servlet.MockMvc
import org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post
import org.springframework.test.web.servlet.result.MockMvcResultMatchers.jsonPath
import org.springframework.test.web.servlet.result.MockMvcResultMatchers.status
import org.springframework.beans.factory.annotation.Autowired
import org.junit.jupiter.api.Test
import org.mockito.kotlin.any
import org.mockito.kotlin.whenever

/**
 * LISS-0155 Phase 1/2 — Spring Boot HTTP contract for seed_auth.
 *
 * Assertions match LISS-0146 API scenarios and Adjudicator-approved JSON shape
 * `{ "reason": "..." }` (`lockedUntil` on login lockout only).
 */
@WebMvcTest(controllers = [PlayerAuthController::class])
class PlayerAuthHttpAdapterTest {
    @Autowired
    private lateinit var mockMvc: MockMvc

    @MockitoBean
    private lateinit var authService: PlayerAuthenticationService

    @Test
    fun register_returns_created_when_accepted() {
        whenever(authService.register(any<RegisterUserCommand>()))
            .thenReturn(RegisterResult(accepted = true))

        mockMvc.perform(
            post("/register")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""{"username":"alice","password":"secret-password"}"""),
        )
            .andExpect(status().isCreated)
            .andExpect(jsonPath("$.accepted").value(true))
    }

    @Test
    fun register_returns_conflict_for_duplicate_username() {
        whenever(authService.register(any<RegisterUserCommand>()))
            .thenReturn(RegisterResult(accepted = false, reason = "duplicate_username"))

        mockMvc.perform(
            post("/register")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""{"username":"alice","password":"secret-password"}"""),
        )
            .andExpect(status().isConflict)
            .andExpect(jsonPath("$.reason").value("duplicate_username"))
    }

    @Test
    fun login_returns_challenge_key_without_session_key() {
        whenever(authService.login(any<NativeLoginCommand>()))
            .thenReturn(
                LoginResult(
                    accepted = true,
                    challengeKey = ChallengeKey("challenge-1"),
                ),
            )

        mockMvc.perform(
            post("/login")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""{"username":"alice","password":"secret-password"}"""),
        )
            .andExpect(status().isOk)
            .andExpect(jsonPath("$.accepted").value(true))
            .andExpect(jsonPath("$.challengeKey").value("challenge-1"))
            .andExpect(jsonPath("$.sessionKey").doesNotExist())
    }

    @Test
    fun login_returns_unauthorized_for_invalid_credentials() {
        whenever(authService.login(any<NativeLoginCommand>()))
            .thenReturn(LoginResult(accepted = false, reason = "invalid_credentials"))

        mockMvc.perform(
            post("/login")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""{"username":"alice","password":"wrong"}"""),
        )
            .andExpect(status().isUnauthorized)
            .andExpect(jsonPath("$.reason").value("invalid_credentials"))
    }

    @Test
    fun login_returns_locked_out_with_locked_until() {
        val lockedUntil = Instant.parse("2026-08-20T12:15:00Z")
        whenever(authService.login(any<NativeLoginCommand>()))
            .thenReturn(
                LoginResult(
                    accepted = false,
                    reason = "locked_out",
                    lockedUntil = lockedUntil,
                ),
            )

        mockMvc.perform(
            post("/login")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""{"username":"alice","password":"wrong"}"""),
        )
            .andExpect(status().isLocked)
            .andExpect(jsonPath("$.reason").value("locked_out"))
            .andExpect(jsonPath("$.lockedUntil").value("2026-08-20T12:15:00Z"))
    }

    @Test
    fun logout_revokes_session() {
        whenever(authService.logout(any<LogoutCommand>()))
            .thenReturn(LogoutResult(accepted = true))

        mockMvc.perform(
            post("/logout")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""{"sessionKey":"session-1"}"""),
        )
            .andExpect(status().isOk)
            .andExpect(jsonPath("$.accepted").value(true))
    }
}
