// Copyright (c) 2009-2022 The Bitcoin Core developers

// ViceversaChain Genesis Block Miner
// Compile: g++ -o mine_genesis mine_genesis.cpp -lcrypto -std=c++11
// Run: ./mine_genesis

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <openssl/sha.h>

// Reverse bytes (little-endian <-> big-endian)
void reverse_bytes(unsigned char* data, size_t len) {
    for (size_t i = 0; i < len / 2; i++) {
        unsigned char tmp = data[i];
        data[i] = data[len - 1 - i];
        data[len - 1 - i] = tmp;
    }
}

// Double SHA256
void double_sha256(const unsigned char* data, size_t len, unsigned char* hash) {
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    SHA256(data, len, hash1);
    SHA256(hash1, SHA256_DIGEST_LENGTH, hash);
}

// Serialize uint32 little-endian
void write_uint32_le(unsigned char* buf, uint32_t value) {
    buf[0] = value & 0xff;
    buf[1] = (value >> 8) & 0xff;
    buf[2] = (value >> 16) & 0xff;
    buf[3] = (value >> 24) & 0xff;
}

int main() {
    // ViceversaChain Genesis Block Parameters
    const char* pszTimestamp = "ViceversaChain 18/Dec/2024 Blockchain that counts backwards from 100M to 0";
    const uint32_t nTime = 1734523200;
    const uint32_t nBits = 0x1d00ffff;
    const int32_t nVersion = 1;

    // Calculate Merkle Root (from genesis transaction)
    // This is the merkle root for the coinbase tx with our timestamp
    unsigned char merkle_root[32];
    // You'll need to calculate this from the actual genesis transaction
    // For now, using placeholder - MUST BE CALCULATED CORRECTLY
    std::cout << "WARNING: This is a simplified miner." << std::endl;
    std::cout << "You need to get the correct merkle root from the actual genesis transaction!" << std::endl;
    std::cout << std::endl;

    // Genesis transaction merkle root (will be different due to new timestamp)
    // Format: txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>(pszTimestamp...)
    // This creates a unique merkle root

    // For now, just print the info needed
    std::cout << "ViceversaChain Genesis Block Mining Tool" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Timestamp: \"" << pszTimestamp << "\"" << std::endl;
    std::cout << "nTime: " << nTime << std::endl;
    std::cout << "nBits: 0x" << std::hex << nBits << std::dec << std::endl;
    std::cout << "nVersion: " << nVersion << std::endl;
    std::cout << std::endl;

    std::cout << "To mine the genesis block:" << std::endl;
    std::cout << "1. Compile ViceversaChain with the new timestamp" << std::endl;
    std::cout << "2. Run: ./viceversachaind -printtoconsole" << std::endl;
    std::cout << "3. It will calculate and print the merkle root" << std::endl;
    std::cout << "4. OR use the Python script which can calculate it" << std::endl;
    std::cout << std::endl;

    std::cout << "For quick mining, use the Python script instead:" << std::endl;
    std::cout << "  python3 contrib/devtools/mine_genesis_full.py" << std::endl;

    return 0;
}
