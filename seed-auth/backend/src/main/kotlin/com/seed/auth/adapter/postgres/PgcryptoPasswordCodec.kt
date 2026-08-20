package com.seed.auth.adapter.postgres

import com.seed.auth.application.port.PasswordCodec
import java.sql.Connection

class PgcryptoPasswordCodec(
    private val openConnection: () -> Connection = PostgresConnections::openFromEnvironment,
) : PasswordCodec {
    override fun hash(password: String): String =
        openConnection.query(
            sql = "SELECT crypt(?, gen_salt('bf'))",
            bind = { setString(1, password) },
        ) { result ->
            check(result.next()) { "pgcrypto hash returned no row" }
            result.getString(1)
        }

    override fun matches(password: String, passwordHash: String): Boolean =
        openConnection.query(
            sql = "SELECT crypt(?, ?) = ?",
            bind = {
                setString(1, password)
                setString(2, passwordHash)
                setString(3, passwordHash)
            },
        ) { result ->
            check(result.next()) { "pgcrypto match returned no row" }
            result.getBoolean(1)
        }

    companion object {
        fun fromEnvironment(): PgcryptoPasswordCodec = PgcryptoPasswordCodec()
    }
}
