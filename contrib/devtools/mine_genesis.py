#!/usr/bin/env python3
# Mine ViceversaChain genesis block

import hashlib
import struct
import time

def dblsha(data):
    return hashlib.sha256(hashlib.sha256(data).digest()).digest()

def mine_genesis():
    # Genesis parameters
    nTime = 1734523200
    nBits = 0x1d00ffff
    nVersion = 1

    # Calculate target from nBits
    nBits_compact = nBits
    nSize = nBits_compact >> 24
    nWord = nBits_compact & 0x007fffff
    if nSize <= 3:
        target = nWord >> (8 * (3 - nSize))
    else:
        target = nWord << (8 * (nSize - 3))

    print(f"Mining genesis block for ViceversaChain...")
    print(f"Timestamp: {nTime}")
    print(f"Target: {hex(target)}")
    print()

    # Start mining
    nNonce = 0
    start_time = time.time()

    while True:
        # Build block header (simplified - we'll get the real merkle root from the daemon)
        # This just finds A valid nonce, the real one needs the correct merkle root
        header = struct.pack("<I", nVersion)
        header += b'\x00' * 32  # prev block hash
        header += b'\x00' * 32  # merkle root placeholder (will be calculated by daemon)
        header += struct.pack("<I", nTime)
        header += struct.pack("<I", nBits)
        header += struct.pack("<I", nNonce)

        hash_result = dblsha(header)
        hash_int = int.from_bytes(hash_result[::-1], 'big')

        if hash_int <= target:
            print(f"\n✓ Found valid nonce!")
            print(f"nNonce = {nNonce}")
            print(f"Hash = {hash_result[::-1].hex()}")
            elapsed = time.time() - start_time
            print(f"Time elapsed: {elapsed:.2f}s")
            print(f"Hashrate: {nNonce/elapsed:.0f} H/s")
            break

        nNonce += 1
        if nNonce % 100000 == 0:
            print(f"Tried {nNonce} nonces... ({nNonce/(time.time()-start_time):.0f} H/s)", end='\r')

if __name__ == "__main__":
    mine_genesis()
