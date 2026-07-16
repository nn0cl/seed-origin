#include <cassert>
#include <cstring>

#include "FileManager.h"
#include "SeedBinary.h"

namespace seed_binary_tests {

void rejects_out_of_range_input() {
    SeedBinary binary;
    char data[STANDARD_BINARY_SIZE]{};
    assert(binary.setBinary(data, STANDARD_BINARY_SIZE + 1, 0) == 0);
    assert(binary.setBinary(nullptr, 0, 0) == 0);
    assert(binary.setBinary(data, 0, -1) == 0);
}

void zero_fills_unused_block_bytes() {
    SeedBinary binary;
    char input[2] = {'a', 'b'};
    char output[STANDARD_BINARY_SIZE];
    size_t size = 0;
    bool valid = false;
    assert(binary.setBinary(input, sizeof(input), 0) == 1);
    assert(binary.getBinary(output, size, 0, valid) == 1);
    assert(valid && size == sizeof(input));
    assert(std::memcmp(output, input, sizeof(input)) == 0);
    for (size_t i = sizeof(input); i < STANDARD_BINARY_SIZE; ++i) {
        assert(output[i] == 0);
    }
}

void rejects_missing_block_on_save() {
    SeedBinary binary;
    char data[1] = {'x'};
    assert(binary.setBinary(data, sizeof(data), 1) == 1);
    io::FileManager manager;
    assert(manager.save(binary, "/tmp/seed-missing-block.bin") == 0);
}

} // namespace seed_binary_tests
