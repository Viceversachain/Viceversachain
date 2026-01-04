// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2025 The Viceversachain Core developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chain.h>
#include <tinyformat.h>
#include <util/time.h>

std::string CBlockFileInfo::ToString() const
{
    return strprintf("CBlockFileInfo(blocks=%u, size=%u, heights=%u...%u, time=%s...%s)", nBlocks, nSize, nHeightFirst, nHeightLast, FormatISO8601Date(nTimeFirst), FormatISO8601Date(nTimeLast));
}

std::string CBlockIndex::ToString() const
{
    return strprintf("CBlockIndex(pprev=%p, nHeight=%d, merkle=%s, hashBlock=%s)",
                     pprev, nHeight, hashMerkleRoot.ToString(), GetBlockHash().ToString());
}

void CChain::SetTip(CBlockIndex& block)
{
    CBlockIndex* pindex = &block;
    // ViceversaChain: Use offset mapping (genesis at 100M = index 0)
    // Array index = (100000000 - height)
    const int GENESIS_HEIGHT = 100000000;
    int chainDepth = GENESIS_HEIGHT - pindex->nHeight + 1;
    // Safety check: ensure valid chain depth
    if (chainDepth <= 0 || chainDepth > GENESIS_HEIGHT + 1) {
        throw std::runtime_error(strprintf("Invalid chain depth %d for height %d", chainDepth, pindex->nHeight));
    }
    vChain.resize(chainDepth);
    while (pindex) {
        int idx = GENESIS_HEIGHT - pindex->nHeight;
        if (vChain[idx] != pindex) {
            vChain[idx] = pindex;
            pindex = pindex->pprev;
        } else {
            break;
        }
    }
}

std::vector<uint256> LocatorEntries(const CBlockIndex* index)
{
    int step = 1;
    std::vector<uint256> have;
    if (index == nullptr) return have;

    have.reserve(32);
    // ViceversaChain: Genesis is at height 100M (oldest block)
    const int GENESIS_HEIGHT = 100000000;
    while (index) {
        have.emplace_back(index->GetBlockHash());
        if (index->nHeight == GENESIS_HEIGHT) break;
        // Exponentially larger steps back (toward genesis = higher heights)
        int height = std::min(index->nHeight + step, GENESIS_HEIGHT);
        // Use skiplist.
        index = index->GetAncestor(height);
        if (have.size() > 10) step *= 2;
    }
    return have;
}

CBlockLocator GetLocator(const CBlockIndex* index)
{
    return CBlockLocator{LocatorEntries(index)};
}

CBlockLocator CChain::GetLocator() const
{
    return ::GetLocator(Tip());
}

const CBlockIndex *CChain::FindFork(const CBlockIndex *pindex) const {
    if (pindex == nullptr) {
        return nullptr;
    }
    // ViceversaChain: Lower height = more recent.
    // If fork block is newer (lower height), get its ancestor at our tip height
    if (pindex->nHeight < Height())
        pindex = pindex->GetAncestor(Height());
    while (pindex && !Contains(pindex))
        pindex = pindex->pprev;
    return pindex;
}

CBlockIndex* CChain::FindEarliestAtLeast(int64_t nTime, int height) const
{
    // ViceversaChain: 'earliest' means highest height (closest to genesis at 100M)
    // We want the first block with time >= nTime AND height >= height
    std::pair<int64_t, int> blockparams = std::make_pair(nTime, height);
    std::vector<CBlockIndex*>::const_iterator lower = std::lower_bound(vChain.begin(), vChain.end(), blockparams,
        [](CBlockIndex* pBlock, const std::pair<int64_t, int>& blockparams) -> bool { 
            // ViceversaChain: inverted - earlier blocks have higher heights
            // OLD BUG: pBlock->nHeight > blockparams.second (incorrect - finds older blocks)
            return pBlock->GetBlockTimeMax() < blockparams.first || pBlock->nHeight < blockparams.second; 
        });
    return (lower == vChain.end() ? nullptr : *lower);
}

/** Turn the lowest '1' bit in the binary representation of a number into a '0'. */
int static inline InvertLowestOne(int n) { return n & (n - 1); }

/** Compute what height to jump back to with the CBlockIndex::pskip pointer. */
int static inline GetSkipHeight(int height) {
    // ViceversaChain: ancestors have HIGHER heights (toward genesis at 100M)
    const int GENESIS_HEIGHT = 100000000;
    if (height >= GENESIS_HEIGHT - 1)
        return GENESIS_HEIGHT;

    // Calculate distance from genesis
    int distance = GENESIS_HEIGHT - height;
    if (distance < 2)
        return GENESIS_HEIGHT;

    // Jump toward genesis (higher heights)
    int skip_distance = (distance & 1) ? InvertLowestOne(InvertLowestOne(distance - 1)) + 1 : InvertLowestOne(distance);
    return GENESIS_HEIGHT - skip_distance;
}

const CBlockIndex* CBlockIndex::GetAncestor(int height) const
{
    // ViceversaChain: Ancestors have HIGHER heights (toward genesis at 100M)
    const int GENESIS_HEIGHT = 100000000;
    if (height < nHeight || height > GENESIS_HEIGHT) {
        return nullptr;
    }

    const CBlockIndex* pindexWalk = this;
    int heightWalk = nHeight;
    while (heightWalk < height) {
        int heightSkip = GetSkipHeight(heightWalk);
        int heightSkipPrev = GetSkipHeight(heightWalk + 1);
        if (pindexWalk->pskip != nullptr &&
            (heightSkip == height ||
             (heightSkip < height && !(heightSkipPrev > heightSkip + 2 &&
                                       heightSkipPrev <= height)))) {
            // Only follow pskip if pprev->pskip isn't better than pskip->pprev.
            pindexWalk = pindexWalk->pskip;
            heightWalk = heightSkip;
        } else {
            assert(pindexWalk->pprev);
            pindexWalk = pindexWalk->pprev;
            heightWalk++;
        }
    }
    return pindexWalk;
}

CBlockIndex* CBlockIndex::GetAncestor(int height)
{
    return const_cast<CBlockIndex*>(static_cast<const CBlockIndex*>(this)->GetAncestor(height));
}

void CBlockIndex::BuildSkip()
{
    if (pprev)
        pskip = pprev->GetAncestor(GetSkipHeight(nHeight));
}

arith_uint256 GetBlockProof(const CBlockIndex& block)
{
    arith_uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for an arith_uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (bnTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params& params)
{
    arith_uint256 r;
    int sign = 1;
    if (to.nChainWork > from.nChainWork) {
        r = to.nChainWork - from.nChainWork;
    } else {
        r = from.nChainWork - to.nChainWork;
        sign = -1;
    }
    r = r * arith_uint256(params.nPowTargetSpacing) / GetBlockProof(tip);
    if (r.bits() > 63) {
        return sign * std::numeric_limits<int64_t>::max();
    }
    return sign * int64_t(r.GetLow64());
}

/** Find the last common ancestor two blocks have.
 *  Both pa and pb must be non-nullptr. */
const CBlockIndex* LastCommonAncestor(const CBlockIndex* pa, const CBlockIndex* pb) {
    // ViceversaChain: Higher height = older block (toward genesis at 100M)
    if (pa->nHeight > pb->nHeight) {
        // pa is older, move pb backward to match
        pb = pb->GetAncestor(pa->nHeight);
    } else if (pb->nHeight > pa->nHeight) {
        // pb is older, move pa backward to match
        pa = pa->GetAncestor(pb->nHeight);
    }

    while (pa != pb && pa && pb) {
        pa = pa->pprev;
        pb = pb->pprev;
    }

    // Eventually all chain branches meet at the genesis block.
    assert(pa == pb);
    return pa;
}
