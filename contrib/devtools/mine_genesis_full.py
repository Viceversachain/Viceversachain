#!/usr/bin/env python3
"""
ViceversaChain Genesis Block Miner
Mine the genesis block and output the correct parameters
"""

import hashlib
import struct
import time
import binascii

def hash256(data):
    """Double SHA256"""
    return hashlib.sha256(hashlib.sha256(data).digest()).digest()

def reverse_bytes(data):
    """Reverse byte order"""
    return data[::-1]

def compact_to_target(compact):
    """Convert compact difficulty to target"""
    size = compact >> 24
    word = compact & 0x007fffff
    if size <= 3:
        word >>= 8 * (3 - size)
        return word
    else:
        return word << (8 * (size - 3))

def create_genesis_tx():
    """Create the genesis coinbase transaction"""
    tx = b""

    # nVersion
    tx += struct.pack("<I", 1)

    # vin count
    tx += b"\x01"

    # vin[0] prevout hash (null)
    tx += b"\x00" * 32

    # vin[0] prevout n
    tx += struct.pack("<I", 0xffffffff)

    # vin[0] scriptSig
    timestamp = b"ViceversaChain 18/Dec/2024 Blockchain that counts backwards from 100M to 0"
    script_sig = struct.pack("<I", 486604799) + struct.pack("B", 4) + struct.pack("B", len(timestamp)) + timestamp
    tx += struct.pack("B", len(script_sig)) + script_sig

    # vin[0] sequence
    tx += struct.pack("<I", 0xffffffff)

    # vout count
    tx += b"\x01"

    # vout[0] value (50 BTC in satoshis)
    tx += struct.pack("<Q", 50 * 100000000)

    # vout[0] scriptPubKey
    pubkey = binascii.unhexlify("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f")
    script_pubkey = struct.pack("B", len(pubkey)) + pubkey + b"\xac"  # OP_CHECKSIG
    tx += struct.pack("B", len(script_pubkey)) + script_pubkey

    # nLockTime
    tx += struct.pack("<I", 0)

    return tx

def mine_genesis():
    """Mine the genesis block"""
    print("ViceversaChain Genesis Block Miner")
    print("=" * 50)
    print()

    # Genesis parameters
    nVersion = 1
    nTime = 1734523200
    nBits = 0x1e00ffff  # Lower difficulty for faster mining

    # Create genesis transaction
    genesis_tx = create_genesis_tx()
    merkle_root = hash256(genesis_tx)

    print(f"Genesis Transaction Hash (Merkle Root):")
    print(f"  {reverse_bytes(merkle_root).hex()}")
    print()

    # Calculate target
    target = compact_to_target(nBits)
    print(f"Target: {hex(target)}")
    print(f"Difficulty bits: {hex(nBits)}")
    print()

    print("Mining genesis block...")
    print("(This may take a few minutes)")
    print()

    nNonce = 0
    start_time = time.time()

    while True:
        # Build block header
        header = b""
        header += struct.pack("<I", nVersion)
        header += b"\x00" * 32  # prev block (null for genesis)
        header += merkle_root
        header += struct.pack("<I", nTime)
        header += struct.pack("<I", nBits)
        header += struct.pack("<I", nNonce)

        # Hash the header
        block_hash = hash256(header)
        hash_int = int.from_bytes(block_hash[::-1], 'big')

        # Check if valid
        if hash_int <= target:
            print("\n✓ Genesis block mined successfully!")
            print()
            print("=" * 50)
            print("ViceversaChain Genesis Block Parameters:")
            print("=" * 50)
            print()
            print(f"nTime = {nTime}")
            print(f"nNonce = {nNonce}")
            print(f"nBits = 0x{nBits:08x}")
            print()
            print(f"Genesis Hash:")
            print(f"  {reverse_bytes(block_hash).hex()}")
            print()
            print(f"Merkle Root:")
            print(f"  {reverse_bytes(merkle_root).hex()}")
            print()
            print("=" * 50)
            print()

            elapsed = time.time() - start_time
            print(f"Mining time: {elapsed:.2f} seconds")
            print(f"Hash rate: {nNonce/elapsed:,.0f} H/s")
            print()

            print("Copy these values into src/kernel/chainparams.cpp:")
            print()
            print(f'genesis = CreateGenesisBlock({nTime}, {nNonce}, 0x{nBits:08x}, {nVersion}, 50 * COIN);')
            print('consensus.hashGenesisBlock = genesis.GetHash();')
            print(f'assert(consensus.hashGenesisBlock == uint256S("0x{reverse_bytes(block_hash).hex()}"));')
            print(f'assert(genesis.hashMerkleRoot == uint256S("0x{reverse_bytes(merkle_root).hex()}"));')
            print()

            break

        nNonce += 1

        if nNonce % 100000 == 0:
            elapsed = time.time() - start_time
            if elapsed > 0:
                hashrate = nNonce / elapsed
                print(f"\rTried {nNonce:,} nonces... ({hashrate:,.0f} H/s)", end='', flush=True)

if __name__ == "__main__":
    mine_genesis()
