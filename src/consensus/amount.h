// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Viceversachain Core developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_AMOUNT_H
#define BITCOIN_CONSENSUS_AMOUNT_H

#include <cstdint>

/** Amount in satoshis (Can be negative) */
typedef int64_t CAmount;

/** The amount of satoshis in one VIVE. */
static constexpr CAmount COIN = 100000000;

/** No amount larger than this (in satoshi) is valid.
 *
 * ViceversaChain total supply: 194,415,500 VIVE
 * - Blocks 100M→98,686,000 (1,314,000 blocks): 0.25 VIVE = 328,500 VIVE
 * - Blocks 98,686,000→97,372,000 (1,314,000 blocks): 0.5 VIVE = 657,000 VIVE
 * - Blocks 97,372,000→96,058,000 (1,314,000 blocks): 1.0 VIVE = 1,314,000 VIVE
 * - Blocks 96,058,000→0 (96,058,000 blocks): 2.0 VIVE = 192,116,000 VIVE
 * Total: 194,415,500 VIVE
 * */
static constexpr CAmount MAX_MONEY = 194415500 * COIN;
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif // BITCOIN_CONSENSUS_AMOUNT_H
