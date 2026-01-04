// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2025 The Viceversachain Core developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>

// ViceversaChain: DarkGravityWave v3 - Difficulty retargeting algorithm
// Based on Dash implementation, adapted for reverse blockchain (decreasing heights)
unsigned int DarkGravityWave(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    /* DarkGravityWave v3 retargets difficulty every single block based on the last 24 blocks.
     * This provides rapid adjustment to hashrate changes and prevents long vulnerability windows.
     *
     * Key differences from Bitcoin:
     * - Retargets EVERY block (not every 2016/10080 blocks)
     * - Uses last 24 blocks average (not entire period)
     * - Adapts quickly to hashrate spikes/drops
     * - Prevents difficulty manipulation via timestamp attacks
     */

    const CBlockIndex *pindex = pindexLast;
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);

    // Genesis block or not enough blocks for averaging
    if (pindexLast == nullptr || pindexLast->nHeight < params.nMinerConfirmationWindow) {
        return bnPowLimit.GetCompact();
    }

    // ViceversaChain: Use last N blocks for averaging (default 24)
    const int64_t nBlocksToAverage = params.nMinerConfirmationWindow;

    arith_uint256 bnAvg;
    bnAvg.SetCompact(pindexLast->nBits);

    // ViceversaChain: Walk backwards through chain (heights INCREASE going back)
    // In reverse blockchain: current=99999800, previous=99999801 (height increases)
    for (int64_t i = 1; pindex && i < nBlocksToAverage; i++) {
        // Move to previous block (higher height in ViceversaChain)
        pindex = pindex->pprev;
        if (!pindex) break;

        arith_uint256 bnTmp;
        bnTmp.SetCompact(pindex->nBits);
        bnAvg += bnTmp;
    }

    // Calculate average difficulty
    bnAvg /= nBlocksToAverage;

    // Get actual timespan of the last N blocks
    // ViceversaChain: First block in window has HIGHER height than last
    const CBlockIndex* pindexFirst = pindexLast;
    for (int64_t i = 0; pindexFirst && i < nBlocksToAverage - 1; i++) {
        pindexFirst = pindexFirst->pprev;
    }

    if (!pindexFirst) {
        return bnPowLimit.GetCompact();
    }

    // Calculate actual time taken for these blocks
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    int64_t nTargetTimespan = nBlocksToAverage * params.nPowTargetSpacing;

    // Limit adjustment to prevent wild swings
    // Don't allow more than 3x change in either direction
    if (nActualTimespan < nTargetTimespan / 3)
        nActualTimespan = nTargetTimespan / 3;
    if (nActualTimespan > nTargetTimespan * 3)
        nActualTimespan = nTargetTimespan * 3;

    // Retarget
    arith_uint256 bnNew = bnAvg;
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    // Never go above powLimit
    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // ViceversaChain: Use DarkGravityWave v3 for all difficulty adjustments
    // This provides per-block retargeting instead of periodic retargeting
    return DarkGravityWave(pindexLast, pblock, params);

    /* ORIGINAL BITCOIN LOGIC - DISABLED for ViceversaChain
     *
     * Bitcoin retargets only every 2016 blocks (2 weeks), creating:
     * - Long vulnerability windows (10080 blocks = 2 weeks for VVC)
     * - Chain gets stuck if difficulty spikes then hashrate drops
     * - Easy 51% attacks during retarget period
     * - No protection against rapid hashrate changes
     *
     * ViceversaChain experienced multiple 51% attacks due to:
     * - Attacker mining 8 blocks/minute (480 blocks/hour)
     * - Difficulty staying low for entire 10080 block window
     * - Could rewrite 500+ blocks before difficulty adjusted
     *
     * DarkGravityWave solves this by retargeting every block.
     *
    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
    */
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

// Check that on difficulty adjustments, the new difficulty does not increase
// or decrease beyond the permitted limits.
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

    if (height % params.DifficultyAdjustmentInterval() == 0) {
        int64_t smallest_timespan = params.nPowTargetTimespan/4;
        int64_t largest_timespan = params.nPowTargetTimespan*4;

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target;
        observed_new_target.SetCompact(new_nbits);

        // Calculate the largest difficulty value possible:
        arith_uint256 largest_difficulty_target;
        largest_difficulty_target.SetCompact(old_nbits);
        largest_difficulty_target *= largest_timespan;
        largest_difficulty_target /= params.nPowTargetTimespan;

        if (largest_difficulty_target > pow_limit) {
            largest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 maximum_new_target;
        maximum_new_target.SetCompact(largest_difficulty_target.GetCompact());
        if (maximum_new_target < observed_new_target) return false;

        // Calculate the smallest difficulty value possible:
        arith_uint256 smallest_difficulty_target;
        smallest_difficulty_target.SetCompact(old_nbits);
        smallest_difficulty_target *= smallest_timespan;
        smallest_difficulty_target /= params.nPowTargetTimespan;

        if (smallest_difficulty_target > pow_limit) {
            smallest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 minimum_new_target;
        minimum_new_target.SetCompact(smallest_difficulty_target.GetCompact());
        if (minimum_new_target > observed_new_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
    return true;
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
