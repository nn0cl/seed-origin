package com.seed.auth.adapter.mybatis

import com.seed.auth.application.port.StoredUser
import com.seed.auth.application.port.UserStore

class MyBatisUserStore(
    private val users: UserMapper,
) : UserStore {
    override fun exists(username: String): Boolean =
        users.existsRow(username) != null

    override fun save(username: String, passwordHash: String) {
        check(users.insert(username, passwordHash) == 1) { "failed to insert user" }
    }

    override fun find(username: String): StoredUser? = users.find(username)

    companion object {
        fun fromEnvironment(): MyBatisUserStore =
            MyBatisUserStore(MyBatisSupport.sessionScopedMapper(UserMapper::class.java))
    }
}
