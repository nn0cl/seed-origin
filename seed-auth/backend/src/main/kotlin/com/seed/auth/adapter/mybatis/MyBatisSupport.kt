package com.seed.auth.adapter.mybatis

import org.apache.ibatis.mapping.Environment
import org.apache.ibatis.session.Configuration
import org.apache.ibatis.session.SqlSession
import org.apache.ibatis.session.SqlSessionFactory
import org.apache.ibatis.session.SqlSessionFactoryBuilder
import org.apache.ibatis.transaction.jdbc.JdbcTransactionFactory
import org.springframework.jdbc.datasource.DriverManagerDataSource
import java.lang.reflect.Proxy
import java.net.URI
import java.util.Properties
import javax.sql.DataSource

/**
 * MyBatis session / DataSource helpers from SEED_IDENTITY_DB_URL (libpq-style).
 */
internal object MyBatisSupport {
    fun dataSourceFromEnvironment(): DataSource {
        val raw = System.getenv("SEED_IDENTITY_DB_URL")
            ?: error("SEED_IDENTITY_DB_URL is not set")
        require(raw.isNotBlank()) { "SEED_IDENTITY_DB_URL is blank" }
        val parsed = parseLibpqStyleUrl(raw)
        val dataSource = DriverManagerDataSource()
        dataSource.setUrl(parsed.jdbcUrl)
        dataSource.setUsername(parsed.properties.getProperty("user"))
        dataSource.setPassword(parsed.properties.getProperty("password"))
        return dataSource
    }

    fun sqlSessionFactory(dataSource: DataSource = dataSourceFromEnvironment()): SqlSessionFactory {
        val environment = Environment("seed-auth", JdbcTransactionFactory(), dataSource)
        val configuration = Configuration(environment)
        configuration.isMapUnderscoreToCamelCase = true
        configuration.addMapper(UserMapper::class.java)
        configuration.addMapper(PasswordMapper::class.java)
        configuration.addMapper(PlayerChallengeMapper::class.java)
        configuration.addMapper(PlayerSessionMapper::class.java)
        return SqlSessionFactoryBuilder().build(configuration)
    }

    fun openSessionFromEnvironment(): SqlSession =
        sqlSessionFactory().openSession(true)

    /**
     * Mapper proxy that opens a short-lived SqlSession for each call.
     * Enables `fromEnvironment()` without a Spring container.
     */
    fun <T : Any> sessionScopedMapper(mapperType: Class<T>): T {
        val factory = sqlSessionFactory()
        @Suppress("UNCHECKED_CAST")
        return Proxy.newProxyInstance(
            mapperType.classLoader,
            arrayOf(mapperType),
        ) { _, method, args ->
            factory.openSession(true).use { session ->
                val mapper = session.getMapper(mapperType)
                method.invoke(mapper, *(args ?: emptyArray()))
            }
        } as T
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
