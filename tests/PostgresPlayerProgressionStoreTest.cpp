#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <pqxx/pqxx>

#include "PostgresPlayerProgressionStore.h"

/**
 * LISS-0148 Phase 1/2 — Postgres player progression adapter acceptance.
 *
 * Skips when SEED_IDENTITY_DB_URL is unset (same convention as other
 * seed_postgres tests).
 */

namespace {

const char* kSkipMessage =
    "SEED_IDENTITY_DB_URL not set; skipping Postgres player progression test\n";

std::unique_ptr<server::PostgresPlayerProgressionStore> openStoreOrSkip() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        server::PostgresPlayerProgressionStore::fromEnvironment();
    if (!store) {
        std::cerr << kSkipMessage;
    }
    return store;
}

int64_t ensureTestUser(pqxx::connection& connection, const std::string& username) {
    pqxx::work tx(connection);
    const pqxx::result inserted = tx.exec(
        "INSERT INTO users (username, password_hash) "
        "VALUES ($1, crypt('test-only', gen_salt('bf'))) "
        "ON CONFLICT (username) DO UPDATE SET username = EXCLUDED.username "
        "RETURNING id",
        pqxx::params{username});
    tx.commit();
    return inserted[0]["id"].as<int64_t>();
}

} // namespace

namespace postgres_player_progression_store_tests {

void grants_base_experience_independently_of_job() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* url = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(url);
    const int64_t userId = ensureTestUser(connection, "prog-base-exp-user");
    const int64_t characterId = store->createCharacter(userId, "Hero", 1);
    store->ensureMasterBaseExp(1, 100);
    store->ensureMasterJobExp(1, 100);

    const server::CharacterProgress before = store->loadCharacter(characterId);
    store->grantBaseExperience(characterId, 40);
    const server::CharacterProgress after = store->loadCharacter(characterId);

    assert(after.baseExp == before.baseExp + 40);
    assert(after.jobExp == before.jobExp);
    assert(after.jobLevel == before.jobLevel);
}

void grants_job_experience_independently_of_base() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* url = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(url);
    const int64_t userId = ensureTestUser(connection, "prog-job-exp-user");
    const int64_t characterId = store->createCharacter(userId, "Mage", 2);
    store->ensureMasterBaseExp(1, 100);
    store->ensureMasterJobExp(1, 100);

    const server::CharacterProgress before = store->loadCharacter(characterId);
    store->grantJobExperience(characterId, 25);
    const server::CharacterProgress after = store->loadCharacter(characterId);

    assert(after.jobExp == before.jobExp + 25);
    assert(after.baseExp == before.baseExp);
    assert(after.baseLevel == before.baseLevel);
}

void creates_equipment_instance_with_durability_and_exp() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* url = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(url);
    const int64_t userId = ensureTestUser(connection, "prog-equip-user");
    const int64_t characterId = store->createCharacter(userId, "Knight", 1);
    store->ensureItemTemplate(10, 50, 2);

    const std::string equipmentId =
        store->createEquipmentInstance(characterId, 10, 50, 0);
    assert(!equipmentId.empty());
    assert(store->equipmentExists(equipmentId));

    const server::EquipmentInstanceSnapshot equip =
        store->loadEquipment(equipmentId);
    assert(equip.durability == 50);
    assert(equip.equipmentExp == 0);
    assert(equip.itemTemplateId == 10);
}

void sockets_item_into_equipment() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* url = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(url);
    const int64_t userId = ensureTestUser(connection, "prog-socket-user");
    const int64_t characterId = store->createCharacter(userId, "Ranger", 1);
    store->ensureItemTemplate(20, 40, 1);
    store->ensureItemTemplate(21, 1, 0);

    const std::string equipmentId =
        store->createEquipmentInstance(characterId, 20, 40, 0);
    const std::string gemId =
        store->createEquipmentInstance(characterId, 21, 1, 0);
    store->socketItem(equipmentId, 0, gemId);
    assert(store->socketedItemId(equipmentId, 0) == gemId);
}

void persists_stamina_on_stamina_event() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* url = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(url);
    const int64_t userId = ensureTestUser(connection, "prog-stamina-user");
    const int64_t characterId = store->createCharacter(userId, "Wanderer", 1);

    store->applyStaminaChange(characterId, 70);
    const server::CharacterProgress after = store->loadCharacter(characterId);
    assert(after.stamina == 70);
}

void reads_base_exp_threshold_from_master_data() {
    std::unique_ptr<server::PostgresPlayerProgressionStore> store =
        openStoreOrSkip();
    if (!store) {
        return;
    }

    store->ensureMasterBaseExp(3, 450);
    assert(store->baseExpToNext(3) == 450);
}

} // namespace postgres_player_progression_store_tests
