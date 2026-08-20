package com.seed.auth.adapter.mybatis

import com.seed.auth.application.port.StoredUser
import org.apache.ibatis.annotations.Insert
import org.apache.ibatis.annotations.Mapper
import org.apache.ibatis.annotations.Param
import org.apache.ibatis.annotations.Select
import org.apache.ibatis.annotations.Update
import java.time.Instant

@Mapper
interface UserMapper {
    @Select("SELECT 1 FROM users WHERE username = #{username}")
    fun existsRow(@Param("username") username: String): Int?

    @Insert(
        """
        INSERT INTO users (username, password_hash)
        VALUES (#{username}, #{passwordHash})
        """,
    )
    fun insert(
        @Param("username") username: String,
        @Param("passwordHash") passwordHash: String,
    ): Int

    @Select(
        """
        SELECT id AS userId, username, password_hash AS passwordHash
          FROM users
         WHERE username = #{username}
        """,
    )
    fun find(@Param("username") username: String): StoredUser?
}

@Mapper
interface PasswordMapper {
    @Select("SELECT crypt(#{password}, gen_salt('bf'))")
    fun hash(@Param("password") password: String): String

    @Select("SELECT crypt(#{password}, #{passwordHash}) = #{passwordHash}")
    fun matches(
        @Param("password") password: String,
        @Param("passwordHash") passwordHash: String,
    ): Boolean
}

@Mapper
interface PlayerChallengeMapper {
    @Insert(
        """
        INSERT INTO player_challenges (challenge_key, user_id, expires_at)
        VALUES (#{challengeKey}, #{userId}, #{expiresAt})
        """,
    )
    fun insert(
        @Param("challengeKey") challengeKey: String,
        @Param("userId") userId: Long,
        @Param("expiresAt") expiresAt: Instant,
    ): Int

    @Select(
        """
        SELECT 1 FROM player_challenges
         WHERE challenge_key = #{challengeKey}
           AND claimed_at IS NULL
        """,
    )
    fun existsUnclaimedRow(@Param("challengeKey") challengeKey: String): Int?
}

@Mapper
interface PlayerSessionMapper {
    @Update(
        """
        UPDATE player_sessions
           SET revoked_at = now()
         WHERE session_token = #{sessionToken}
           AND revoked_at IS NULL
        """,
    )
    fun revoke(@Param("sessionToken") sessionToken: String): Int

    @Insert(
        """
        INSERT INTO player_sessions (session_token, user_id, expires_at)
        VALUES (#{sessionToken}, #{userId}, #{expiresAt})
        """,
    )
    fun insert(
        @Param("sessionToken") sessionToken: String,
        @Param("userId") userId: Long,
        @Param("expiresAt") expiresAt: Instant,
    ): Int

    @Select(
        """
        SELECT 1 FROM player_sessions
         WHERE session_token = #{sessionToken}
           AND revoked_at IS NULL
           AND expires_at > #{now}
        """,
    )
    fun isActiveRow(
        @Param("sessionToken") sessionToken: String,
        @Param("now") now: Instant,
    ): Int?
}
