package com.seed.auth

import org.springframework.boot.autoconfigure.SpringBootApplication
import org.springframework.boot.runApplication

/**
 * seed_auth Spring Boot entry (LISS-0156: MyBatis enabled via AuthPersistenceConfig).
 */
@SpringBootApplication
class SeedAuthApplication

fun main(args: Array<String>) {
    runApplication<SeedAuthApplication>(*args)
}
