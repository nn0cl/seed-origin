#ifndef SEED_LOGIN_FIELD_SPAWN_SETTINGS_H
#define SEED_LOGIN_FIELD_SPAWN_SETTINGS_H

#include <string>

namespace server {

inline constexpr float LOGIN_FIELD_SPAWN_DEFAULT_X = 0.0f;
inline constexpr float LOGIN_FIELD_SPAWN_DEFAULT_Y = 0.0f;
inline constexpr float LOGIN_FIELD_SPAWN_DEFAULT_Z = 0.0f;
inline constexpr long LOGIN_FIELD_SPAWN_DEFAULT_HP = 10;
inline constexpr long LOGIN_FIELD_SPAWN_DEFAULT_MP = 10;
inline constexpr long LOGIN_FIELD_SPAWN_MAX_HP = 1024;
inline constexpr long LOGIN_FIELD_SPAWN_MAX_MP = 1024;

inline long clampSpawnResource(long value, long maxValue) {
    const long floored = value < 0 ? 0 : value;
    const long cap = maxValue < 0 ? 0 : maxValue;
    return floored > cap ? cap : floored;
}

struct LoginFieldSpawnSettings {
    float x;
    float y;
    float z;
    long hp;
    long mp;
    long maxHp;
    long maxMp;
    std::string playerName;

    LoginFieldSpawnSettings()
        : x(LOGIN_FIELD_SPAWN_DEFAULT_X),
          y(LOGIN_FIELD_SPAWN_DEFAULT_Y),
          z(LOGIN_FIELD_SPAWN_DEFAULT_Z),
          hp(LOGIN_FIELD_SPAWN_DEFAULT_HP),
          mp(LOGIN_FIELD_SPAWN_DEFAULT_MP),
          maxHp(LOGIN_FIELD_SPAWN_MAX_HP),
          maxMp(LOGIN_FIELD_SPAWN_MAX_MP),
          playerName() {}

    LoginFieldSpawnSettings(float spawnX, float spawnY, float spawnZ, long spawnHp,
                            long spawnMp, long spawnMaxHp, long spawnMaxMp)
        : x(spawnX), y(spawnY), z(spawnZ), hp(spawnHp), mp(spawnMp),
          maxHp(spawnMaxHp), maxMp(spawnMaxMp), playerName() {}

    long clampedHp() const { return clampSpawnResource(hp, maxHp); }
    long clampedMp() const { return clampSpawnResource(mp, maxMp); }
};

class LoginFieldSpawnPolicy {
public:
    virtual ~LoginFieldSpawnPolicy() {}
    virtual LoginFieldSpawnSettings settings() const = 0;
};

class DefaultLoginFieldSpawnPolicy : public LoginFieldSpawnPolicy {
public:
    LoginFieldSpawnSettings settings() const { return LoginFieldSpawnSettings(); }
};

}

#endif
