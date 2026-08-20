#ifndef SEED_PLAYER_NAME_H
#define SEED_PLAYER_NAME_H

#include <string>

inline bool isPlayerNamePadding(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' ||
           ch == '\f';
}

inline std::string trimPlayerName(const std::string& raw) {
    std::string::size_type begin = 0;
    while (begin < raw.size() && isPlayerNamePadding(raw[begin])) {
        ++begin;
    }
    std::string::size_type end = raw.size();
    while (end > begin && isPlayerNamePadding(raw[end - 1])) {
        --end;
    }
    return raw.substr(begin, end - begin);
}

inline bool isEmptyPlayerName(const std::string& raw) {
    return trimPlayerName(raw).empty();
}

#endif
