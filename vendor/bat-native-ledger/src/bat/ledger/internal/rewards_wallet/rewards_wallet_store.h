/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_STORE_H_
#define BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_STORE_H_

#include "bat/ledger/internal/core/bat_ledger_context.h"
#include "bat/ledger/internal/core/future.h"
#include "bat/ledger/internal/core/optional.h"
#include "bat/ledger/internal/rewards_wallet/rewards_wallet.h"

namespace ledger {

class RewardsWalletStore : public BATLedgerContext::Object {
 public:
  static const size_t kComponentKey;

  struct ReadResult {
    ReadResult();
    ~ReadResult();

    ReadResult(const ReadResult& other);

    optional<RewardsWallet> wallet;
    bool corrupted = false;
  };

  Future<ReadResult> Read();

  Future<bool> Write(const RewardsWallet& wallet);
};

}  // namespace ledger

#endif  // BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_STORE_H_
