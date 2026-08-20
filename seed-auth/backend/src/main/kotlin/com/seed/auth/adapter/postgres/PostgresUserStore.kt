package com.seed.auth.adapter.postgres

import com.seed.auth.application.port.StoredUser
import com.seed.auth.application.port.UserStore
import java.sql.Connection

class PostgresUserStore(
    private val openConnection: () -> Connection = PostgresConnections::openFromEnvironment,
) : UserStore {
    override fun exists(username: String): Boolean =
        openConnection.query(
            sql = "SELECT 1 FROM users WHERE username = ?",
            bind = { setString(1, username) },
        ) { it.hasRow() }

    override fun save(username: String, passwordHash: String) {
        val updated = openConnection.update(
            sql = "INSERT INTO users (username, password_hash) VALUES (?, ?)",
            bind = {
                setString(1, username)
                setString(2, passwordHash)
            },
        )
        check(updated == 1) { "failed to insert user" }
    }

    override fun find(username: String): StoredUser? =
        openConnection.query(
            sql = "SELECT id, username, password_hash FROM users WHERE username = ?",
            bind = { setString(1, username) },
        ) { result ->
            if (!result.next()) {
                return@query null
            }
            StoredUser(
                userId = result.getLong("id"),
                username = result.getString("username"),
                passwordHash = result.getString("password_hash"),
            )
        }

    companion object {
        fun fromEnvironment(): PostgresUserStore = PostgresUserStore()
    }
}
