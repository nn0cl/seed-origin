#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <httplib.h>

#include "AdminAuthStore.h"
#include "AdminLoginLockout.h"
#include "AdminSessionStore.h"
#include "IdentityAliasStore.h"
#include "PostgresIdentityAliasStore.h"

namespace {

// ADR 0017 decision (2026-07-18): fixed 1-hour TTL, no idle timeout.
constexpr uint64_t kSessionTtlSeconds = 60 * 60;

// ADR 0017 decision (2026-07-18): lock after 3 consecutive failed logins for
// a username. Lockout duration itself was not specified by the Adjudicator;
// 15 minutes is an assumed default pending confirmation (see LISS-0144).
constexpr std::size_t kMaxLoginAttempts = 3;
constexpr uint64_t kLockoutSeconds = 15 * 60;

// Minimal escaping for the small, fixed-shape JSON this admin API emits.
// Values here are canonical claimed IDs (already restricted to
// alphanumeric/_/-/. by SessionRegistry::isValidClaimedId) plus numbers, so
// this only needs to guard against '"' and '\\' defensively.
std::string jsonEscape(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char c : value) {
        if (c == '"' || c == '\\') escaped.push_back('\\');
        escaped.push_back(c);
    }
    return escaped;
}

std::string reviewStatusName(session::AliasReviewStatus status) {
    switch (status) {
        case session::AliasReviewStatus::HumanConfirmed: return "confirmed";
        case session::AliasReviewStatus::HumanRejected: return "rejected";
        default: return "unreviewed";
    }
}

// Mirrors SessionRegistry::canonicalClaimedId (private to that class): the
// review-queue payload always carries the already-lowercased canonical id,
// but a human operator submitting the review form may type mixed case.
std::string canonicalClaimedId(const std::string& claimedId) {
    std::string canonical;
    canonical.reserve(claimedId.size());
    for (char c : claimedId) {
        canonical.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return canonical;
}

bool parseReviewStatus(const std::string& name, session::AliasReviewStatus& status) {
    if (name == "confirmed") {
        status = session::AliasReviewStatus::HumanConfirmed;
        return true;
    }
    if (name == "rejected") {
        status = session::AliasReviewStatus::HumanRejected;
        return true;
    }
    return false;
}

std::string recordsToJson(const std::vector<session::IdentityAliasRecord>& records) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < records.size(); ++i) {
        const session::IdentityAliasRecord& record = records[i];
        if (i > 0) json << ",";
        json << "{"
             << "\"aliasId\":" << record.aliasId << ","
             << "\"canonicalClaimedId\":\"" << jsonEscape(record.canonicalClaimedId) << "\","
             << "\"createdTick\":" << record.createdTick << ","
             << "\"lastUsedTick\":" << record.lastUsedTick << ","
             << "\"confidence\":" << record.confidence << ","
             << "\"reviewStatus\":\"" << reviewStatusName(record.reviewStatus) << "\""
             << "}";
    }
    json << "]";
    return json.str();
}

bool requireSession(const httplib::Request& req, admin::AdminSessionStore& sessions,
                    int64_t& adminUserId) {
    const std::string header = req.get_header_value("Authorization");
    const std::string prefix = "Bearer ";
    if (header.size() <= prefix.size() || header.compare(0, prefix.size(), prefix) != 0) {
        return false;
    }
    const std::string token = header.substr(prefix.size());
    return sessions.validate(token, static_cast<uint64_t>(std::time(nullptr)), adminUserId);
}

} // namespace

int main(int argc, char** argv) {
    uint16_t port = 40080;
    if (argc >= 2) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::unique_ptr<admin::AdminAuthStore> authStore = admin::AdminAuthStore::fromEnvironment();
    std::unique_ptr<session::PostgresIdentityAliasStore> aliasStore =
        session::PostgresIdentityAliasStore::fromEnvironment();
    if (!authStore || !aliasStore) {
        std::cerr << "seed_admin: SEED_IDENTITY_DB_URL must be set\n";
        return 1;
    }

    admin::AdminSessionStore sessions;
    admin::AdminLoginLockout lockout(kMaxLoginAttempts, kLockoutSeconds);
    httplib::Server server;

    server.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        const std::string username = req.get_param_value("username");
        const std::string password = req.get_param_value("password");
        const uint64_t now = static_cast<uint64_t>(std::time(nullptr));

        if (username.empty() || lockout.isLocked(username, now)) {
            res.status = 423;
            res.set_content("{\"error\":\"account locked, try again later\"}",
                            "application/json");
            return;
        }

        int64_t adminUserId = 0;
        if (password.empty() || !authStore->verifyCredentials(username, password, adminUserId)) {
            lockout.recordFailure(username, now);
            res.status = 401;
            res.set_content("{\"error\":\"invalid credentials\"}", "application/json");
            return;
        }

        lockout.recordSuccess(username);
        const std::string token = sessions.createSession(adminUserId, now, kSessionTtlSeconds);
        std::ostringstream body;
        body << "{\"token\":\"" << jsonEscape(token) << "\"}";
        res.set_content(body.str(), "application/json");
    });

    server.Post("/logout", [&](const httplib::Request& req, httplib::Response& res) {
        int64_t adminUserId = 0;
        if (!requireSession(req, sessions, adminUserId)) {
            res.status = 401;
            return;
        }
        const std::string header = req.get_header_value("Authorization");
        sessions.invalidate(header.substr(std::string("Bearer ").size()));
        res.status = 204;
    });

    server.Get("/aliases/export", [&](const httplib::Request& req, httplib::Response& res) {
        int64_t adminUserId = 0;
        if (!requireSession(req, sessions, adminUserId)) {
            res.status = 401;
            return;
        }
        res.set_content(recordsToJson(aliasStore->exportRecords()), "application/json");
    });

    server.Get("/aliases/review-queue", [&](const httplib::Request& req, httplib::Response& res) {
        int64_t adminUserId = 0;
        if (!requireSession(req, sessions, adminUserId)) {
            res.status = 401;
            return;
        }
        std::vector<session::IdentityAliasRecord> pending;
        for (const session::IdentityAliasRecord& record : aliasStore->exportRecords()) {
            if (record.reviewStatus == session::AliasReviewStatus::Unreviewed) {
                pending.push_back(record);
            }
        }
        res.set_content(recordsToJson(pending), "application/json");
    });

    server.Post("/aliases/review", [&](const httplib::Request& req, httplib::Response& res) {
        int64_t adminUserId = 0;
        if (!requireSession(req, sessions, adminUserId)) {
            res.status = 401;
            return;
        }
        const std::string claimedId = req.get_param_value("claimedId");
        const std::string statusName = req.get_param_value("status");
        session::AliasReviewStatus status = session::AliasReviewStatus::Unreviewed;
        if (claimedId.empty() || !parseReviewStatus(statusName, status)) {
            res.status = 400;
            res.set_content("{\"error\":\"claimedId and status(confirmed|rejected) are required\"}",
                            "application/json");
            return;
        }
        float confidence = 1.0f;
        if (req.has_param("confidence")) {
            confidence = std::strtof(req.get_param_value("confidence").c_str(), nullptr);
        }
        if (!aliasStore->reviewAlias(canonicalClaimedId(claimedId), status, confidence)) {
            res.status = 404;
            res.set_content("{\"error\":\"alias not found or confidence out of range\"}",
                            "application/json");
            return;
        }
        res.status = 204;
    });

    std::cout << "seed_admin: listening on port " << port << "\n";
    server.listen("127.0.0.1", port);
    return 0;
}
