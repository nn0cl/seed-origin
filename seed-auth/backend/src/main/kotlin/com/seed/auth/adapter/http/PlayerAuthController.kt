package com.seed.auth.adapter.http

import com.seed.auth.application.LoginResult
import com.seed.auth.application.LogoutCommand
import com.seed.auth.application.NativeLoginCommand
import com.seed.auth.application.PlayerAuthenticationService
import com.seed.auth.application.RegisterUserCommand
import com.seed.auth.domain.PlayerSessionKey
import org.springframework.http.HttpStatus
import org.springframework.http.ResponseEntity
import org.springframework.web.bind.annotation.PostMapping
import org.springframework.web.bind.annotation.RequestBody
import org.springframework.web.bind.annotation.RestController

/**
 * HTTP delivery adapter for player authentication UseCase.
 * Maps status codes and JSON shapes only — no auth policy here.
 */
@RestController
class PlayerAuthController(
    private val authService: PlayerAuthenticationService,
) {
    @PostMapping("/register")
    fun register(@RequestBody body: CredentialsRequest): ResponseEntity<Map<String, Any?>> {
        val result = authService.register(
            RegisterUserCommand(username = body.username, password = body.password),
        )
        if (result.accepted) {
            return ResponseEntity.status(HttpStatus.CREATED).body(acceptedBody())
        }
        return ResponseEntity.status(HttpStatus.CONFLICT).body(reasonBody(result.reason))
    }

    @PostMapping("/login")
    fun login(@RequestBody body: CredentialsRequest): ResponseEntity<Map<String, Any?>> {
        val result = authService.login(
            NativeLoginCommand(username = body.username, password = body.password),
        )
        return when {
            result.accepted -> ResponseEntity.ok(
                mapOf(
                    "accepted" to true,
                    "challengeKey" to result.challengeKey!!.value,
                ),
            )
            result.reason == "locked_out" ->
                ResponseEntity.status(HttpStatus.LOCKED).body(lockedOutBody(result))
            else ->
                ResponseEntity.status(HttpStatus.UNAUTHORIZED).body(reasonBody(result.reason))
        }
    }

    @PostMapping("/logout")
    fun logout(@RequestBody body: LogoutRequest): ResponseEntity<Map<String, Any?>> {
        authService.logout(LogoutCommand(sessionKey = PlayerSessionKey(body.sessionKey)))
        return ResponseEntity.ok(acceptedBody())
    }

    private fun acceptedBody(): Map<String, Any?> = mapOf("accepted" to true)

    private fun reasonBody(reason: String?): Map<String, Any?> = mapOf("reason" to reason)

    private fun lockedOutBody(result: LoginResult): Map<String, Any?> {
        val body = linkedMapOf<String, Any?>("reason" to result.reason)
        result.lockedUntil?.let { body["lockedUntil"] = it.toString() }
        return body
    }
}
