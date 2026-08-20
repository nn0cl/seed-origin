package com.seed.auth.adapter.http

data class CredentialsRequest(
    val username: String,
    val password: String,
)

data class LogoutRequest(
    val sessionKey: String,
)
