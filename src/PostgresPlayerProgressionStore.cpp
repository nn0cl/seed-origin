#include "PostgresPlayerProgressionStore.h"

#include <cstdlib>
#include <stdexcept>

#include <pqxx/pqxx>

namespace server {

namespace {

bool hasConnectionString(const char* connectionString) {
    return connectionString != nullptr && connectionString[0] != '\0';
}

CharacterProgress readCharacter(const pqxx::result::reference row) {
    CharacterProgress progress;
    progress.characterId = row["id"].as<int64_t>();
    progress.userId = row["user_id"].as<int64_t>();
    progress.baseLevel = row["base_level"].as<int>();
    progress.baseExp = row["base_exp"].as<int64_t>();
    progress.jobLevel = row["job_level"].as<int>();
    progress.jobExp = row["job_exp"].as<int64_t>();
    progress.stamina = row["stamina"].as<int>();
    progress.jobClassId = row["job_class_id"].as<int>();
    return progress;
}

} // namespace

PostgresPlayerProgressionStore::PostgresPlayerProgressionStore(
    const std::string& connectionString)
    : connection(std::make_unique<pqxx::connection>(connectionString)) {}

PostgresPlayerProgressionStore::~PostgresPlayerProgressionStore() = default;

std::unique_ptr<PostgresPlayerProgressionStore>
PostgresPlayerProgressionStore::fromEnvironment() {
    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    if (!hasConnectionString(connectionString)) {
        return nullptr;
    }
    return std::make_unique<PostgresPlayerProgressionStore>(
        std::string(connectionString));
}

int64_t PostgresPlayerProgressionStore::createCharacter(
    int64_t userId,
    const std::string& name,
    int jobClassId) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "INSERT INTO player_characters "
        "(user_id, name, job_class_id) "
        "VALUES ($1, $2, $3) "
        "RETURNING id",
        pqxx::params{userId, name, jobClassId});
    tx.commit();
    return rows[0]["id"].as<int64_t>();
}

CharacterProgress PostgresPlayerProgressionStore::loadCharacter(
    int64_t characterId) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT id, user_id, base_level, base_exp, job_level, job_exp, "
        "       stamina, job_class_id "
        "  FROM player_characters "
        " WHERE id = $1",
        pqxx::params{characterId});
    tx.commit();
    if (rows.empty()) {
        throw std::runtime_error("player character not found");
    }
    return readCharacter(rows[0]);
}

void PostgresPlayerProgressionStore::grantBaseExperience(int64_t characterId,
                                                         int64_t delta) {
    pqxx::work tx(*connection);
    tx.exec(
        "UPDATE player_characters "
        "   SET base_exp = base_exp + $2 "
        " WHERE id = $1",
        pqxx::params{characterId, delta});
    tx.commit();
}

void PostgresPlayerProgressionStore::grantJobExperience(int64_t characterId,
                                                        int64_t delta) {
    pqxx::work tx(*connection);
    tx.exec(
        "UPDATE player_characters "
        "   SET job_exp = job_exp + $2 "
        " WHERE id = $1",
        pqxx::params{characterId, delta});
    tx.commit();
}

void PostgresPlayerProgressionStore::applyStaminaChange(int64_t characterId,
                                                       int stamina) {
    pqxx::work tx(*connection);
    tx.exec(
        "UPDATE player_characters "
        "   SET stamina = $2 "
        " WHERE id = $1",
        pqxx::params{characterId, stamina});
    tx.commit();
}

void PostgresPlayerProgressionStore::ensureMasterBaseExp(int level,
                                                        int64_t expToNext) {
    pqxx::work tx(*connection);
    tx.exec(
        "INSERT INTO master_base_exp_curve (level, exp_to_next) "
        "VALUES ($1, $2) "
        "ON CONFLICT (level) DO UPDATE SET exp_to_next = EXCLUDED.exp_to_next",
        pqxx::params{level, expToNext});
    tx.commit();
}

void PostgresPlayerProgressionStore::ensureMasterJobExp(int level,
                                                       int64_t expToNext) {
    pqxx::work tx(*connection);
    tx.exec(
        "INSERT INTO master_job_exp_curve (level, exp_to_next) "
        "VALUES ($1, $2) "
        "ON CONFLICT (level) DO UPDATE SET exp_to_next = EXCLUDED.exp_to_next",
        pqxx::params{level, expToNext});
    tx.commit();
}

int64_t PostgresPlayerProgressionStore::baseExpToNext(int level) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT exp_to_next FROM master_base_exp_curve WHERE level = $1",
        pqxx::params{level});
    tx.commit();
    if (rows.empty()) {
        throw std::runtime_error("master base exp curve row missing");
    }
    return rows[0]["exp_to_next"].as<int64_t>();
}

void PostgresPlayerProgressionStore::ensureItemTemplate(int64_t itemTemplateId,
                                                       int maxDurability,
                                                       int socketCount) {
    pqxx::work tx(*connection);
    tx.exec(
        "INSERT INTO master_item_templates "
        "(id, max_durability, socket_count) "
        "VALUES ($1, $2, $3) "
        "ON CONFLICT (id) DO UPDATE SET "
        "  max_durability = EXCLUDED.max_durability, "
        "  socket_count = EXCLUDED.socket_count",
        pqxx::params{itemTemplateId, maxDurability, socketCount});
    tx.commit();
}

std::string PostgresPlayerProgressionStore::createEquipmentInstance(
    int64_t characterId,
    int64_t itemTemplateId,
    int durability,
    int64_t equipmentExp) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "INSERT INTO equipment_instances "
        "(character_id, item_template_id, durability, equipment_exp) "
        "VALUES ($1, $2, $3, $4) "
        "RETURNING id::text",
        pqxx::params{characterId, itemTemplateId, durability, equipmentExp});
    tx.commit();
    return rows[0]["id"].as<std::string>();
}

bool PostgresPlayerProgressionStore::equipmentExists(
    const std::string& equipmentId) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT 1 FROM equipment_instances WHERE id = $1::uuid",
        pqxx::params{equipmentId});
    tx.commit();
    return !rows.empty();
}

EquipmentInstanceSnapshot PostgresPlayerProgressionStore::loadEquipment(
    const std::string& equipmentId) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT id::text AS id, character_id, item_template_id, "
        "       durability, equipment_exp "
        "  FROM equipment_instances "
        " WHERE id = $1::uuid",
        pqxx::params{equipmentId});
    tx.commit();
    if (rows.empty()) {
        throw std::runtime_error("equipment instance not found");
    }
    EquipmentInstanceSnapshot snapshot;
    snapshot.id = rows[0]["id"].as<std::string>();
    snapshot.characterId = rows[0]["character_id"].as<int64_t>();
    snapshot.itemTemplateId = rows[0]["item_template_id"].as<int64_t>();
    snapshot.durability = rows[0]["durability"].as<int>();
    snapshot.equipmentExp = rows[0]["equipment_exp"].as<int64_t>();
    return snapshot;
}

void PostgresPlayerProgressionStore::socketItem(
    const std::string& equipmentId,
    int socketIndex,
    const std::string& socketedItemInstanceId) {
    pqxx::work tx(*connection);
    tx.exec(
        "INSERT INTO equipment_sockets "
        "(equipment_instance_id, socket_index, socketed_item_instance_id) "
        "VALUES ($1::uuid, $2, $3::uuid) "
        "ON CONFLICT (equipment_instance_id, socket_index) DO UPDATE SET "
        "  socketed_item_instance_id = EXCLUDED.socketed_item_instance_id",
        pqxx::params{equipmentId, socketIndex, socketedItemInstanceId});
    tx.commit();
}

std::string PostgresPlayerProgressionStore::socketedItemId(
    const std::string& equipmentId,
    int socketIndex) {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT socketed_item_instance_id::text AS socketed_id "
        "  FROM equipment_sockets "
        " WHERE equipment_instance_id = $1::uuid "
        "   AND socket_index = $2",
        pqxx::params{equipmentId, socketIndex});
    tx.commit();
    if (rows.empty() || rows[0]["socketed_id"].is_null()) {
        return std::string();
    }
    return rows[0]["socketed_id"].as<std::string>();
}

} // namespace server
