package com.seed.auth.adapter.postgres

import java.net.URI
import java.sql.Connection
import java.sql.DriverManager
import java.sql.PreparedStatement
import java.sql.ResultSet
import java.sql.Timestamp
import java.time.Instant
import java.util.Properties

/**
 * Shared JDBC helpers for seed_auth Postgres adapters.
 *
 * Accepts the same `SEED_IDENTITY_DB_URL` shape as C++ adapters
 * (`postgresql://user:pass@host:port/db`) and opens a JDBC connection.
 *
 * MyBatis is deferred to the Spring HTTP adapter slice (ADR 0019 / 0023);
 * this adapter keeps a minimal DriverManager boundary until then.
 */
internal object PostgresConnections {
    fun openFromEnvironment(): Connection {
        val raw = System.getenv("SEED_IDENTITY_DB_URL")
            ?: error("SEED_IDENTITY_DB_URL is not set")
        require(raw.isNotBlank()) { "SEED_IDENTITY_DB_URL is blank" }
        return open(raw)
    }

    internal fun open(url: String): Connection {
        val parsed = parseLibpqStyleUrl(url)
        return DriverManager.getConnection(parsed.jdbcUrl, parsed.properties)
    }

    internal fun parseLibpqStyleUrl(url: String): ParsedJdbc {
        if (url.startsWith("jdbc:")) {
            return ParsedJdbc(jdbcUrl = url, properties = Properties())
        }

        val normalized = when {
            url.startsWith("postgresql://") -> url
            url.startsWith("postgres://") ->
                "postgresql://${url.removePrefix("postgres://")}"
            else -> error("unsupported SEED_IDENTITY_DB_URL scheme: $url")
        }

        // URI needs a scheme Java understands; keep host/user/path semantics.
        val uri = URI(normalized.replaceFirst("postgresql://", "http://"))
        val host = uri.host ?: error("SEED_IDENTITY_DB_URL missing host")
        val port = if (uri.port >= 0) uri.port else 5432
        val database = uri.path.trimStart('/').ifBlank {
            error("SEED_IDENTITY_DB_URL missing database")
        }
        val props = Properties()
        val userInfo = uri.userInfo
        if (!userInfo.isNullOrBlank()) {
            val parts = userInfo.split(":", limit = 2)
            props.setProperty("user", parts[0])
            if (parts.size > 1) {
                props.setProperty("password", parts[1])
            }
        }
        return ParsedJdbc(
            jdbcUrl = "jdbc:postgresql://$host:$port/$database",
            properties = props,
        )
    }

    internal data class ParsedJdbc(
        val jdbcUrl: String,
        val properties: Properties,
    )
}

internal fun Instant.toSqlTimestamp(): Timestamp = Timestamp.from(this)

internal inline fun <T> (() -> Connection).query(
    sql: String,
    bind: PreparedStatement.() -> Unit = {},
    map: (ResultSet) -> T,
): T {
    invoke().use { connection ->
        connection.prepareStatement(sql).use { statement ->
            statement.bind()
            statement.executeQuery().use { result ->
                return map(result)
            }
        }
    }
}

internal inline fun (() -> Connection).update(
    sql: String,
    bind: PreparedStatement.() -> Unit = {},
): Int {
    invoke().use { connection ->
        connection.prepareStatement(sql).use { statement ->
            statement.bind()
            return statement.executeUpdate()
        }
    }
}

internal fun ResultSet.hasRow(): Boolean = next()
