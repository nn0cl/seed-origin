package com.seed.auth

import org.mybatis.spring.boot.autoconfigure.MybatisAutoConfiguration
import org.springframework.boot.autoconfigure.SpringBootApplication
import org.springframework.boot.runApplication

/**
 * seed_auth Spring Boot entry.
 *
 * MyBatis autoconfig is excluded while JDBC adapters (LISS-0151) remain the
 * wired port implementations; MyBatis starter is on the classpath for gradual
 * mapper adoption (LISS-0155 decision 3).
 */
@SpringBootApplication(exclude = [MybatisAutoConfiguration::class])
class SeedAuthApplication

fun main(args: Array<String>) {
    runApplication<SeedAuthApplication>(*args)
}
