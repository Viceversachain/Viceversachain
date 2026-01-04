// Copyright (c) 2016-2019 The Viceversachain Core developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/rbf.h>

#include <primitives/transaction.h>

bool SignalsOptInRBF(const CTransaction &tx)
{
    for (const CTxIn &txin : tx.vin) {
        if (txin.nSequence <= MAX_BIP125_RBF_SEQUENCE) {
            return true;
        }
    }
    return false;
}
