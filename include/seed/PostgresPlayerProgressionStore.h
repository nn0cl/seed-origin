#ifndef SEED_POSTGRES_PLAYER_PROGRESSION_STORE_H
#define SEED_POSTGRES_PLAYER_PROGRESSION_STORE_H

#include <cstdint>
#include <memory>
#include <string>

namespace pqxx {
class connection;
}

namespace server {

struct CharacterProgress {
    int64_t characterId;
    int64_t userId;
    int baseLevel;
    int64_t baseExp;
    int jobLevel;
    int64_t jobExp;
    int stamina;
    int jobClassId;
};

struct EquipmentInstanceSnapshot {
    std::string id;
    int64_t characterId;
    int64_t itemTemplateId;
    int durability;
    int64_t equipmentExp;
};

// LISS-0148: event-driven progression persistence (EXP, equipment, stamina).
class PostgresPlayerProgressionStore {
public:
    explicit PostgresPlayerProgressionStore(const std::string& connectionString);
    ~PostgresPlayerProgressionStore();

    static std::unique_ptr<PostgresPlayerProgressionStore> fromEnvironment();

    PostgresPlayerProgressionStore(const PostgresPlayerProgressionStore&) = delete;
    PostgresPlayerProgressionStore& operator=(
        const PostgresPlayerProgressionStore&) = delete;

    int64_t createCharacter(int64_t userId,
                            const std::string& name,
                            int jobClassId);
    CharacterProgress loadCharacter(int64_t characterId);

    void grantBaseExperience(int64_t characterId, int64_t delta);
    void grantJobExperience(int64_t characterId, int64_t delta);
    void applyStaminaChange(int64_t characterId, int stamina);

    void ensureMasterBaseExp(int level, int64_t expToNext);
    void ensureMasterJobExp(int level, int64_t expToNext);
    int64_t baseExpToNext(int level);

    void ensureItemTemplate(int64_t itemTemplateId,
                            int maxDurability,
                            int socketCount);
    std::string createEquipmentInstance(int64_t characterId,
                                        int64_t itemTemplateId,
                                        int durability,
                                        int64_t equipmentExp);
    bool equipmentExists(const std::string& equipmentId);
    EquipmentInstanceSnapshot loadEquipment(const std::string& equipmentId);
    void socketItem(const std::string& equipmentId,
                    int socketIndex,
                    const std::string& socketedItemInstanceId);
    std::string socketedItemId(const std::string& equipmentId, int socketIndex);

private:
    std::unique_ptr<pqxx::connection> connection;
};

} // namespace server

#endif
