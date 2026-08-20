package com.seed.auth.adapter.mybatis

import com.seed.auth.application.port.PasswordCodec

class MyBatisPasswordCodec(
    private val passwords: PasswordMapper,
) : PasswordCodec {
    override fun hash(password: String): String = passwords.hash(password)

    override fun matches(password: String, passwordHash: String): Boolean =
        passwords.matches(password, passwordHash)

    companion object {
        fun fromEnvironment(): MyBatisPasswordCodec =
            MyBatisPasswordCodec(MyBatisSupport.sessionScopedMapper(PasswordMapper::class.java))
    }
}
