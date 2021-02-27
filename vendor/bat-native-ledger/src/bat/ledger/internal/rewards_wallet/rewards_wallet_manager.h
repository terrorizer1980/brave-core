/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_MANAGER_H_
#define BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_MANAGER_H_

#include "bat/ledger/internal/core/bat_ledger_context.h"
#include "bat/ledger/internal/core/callback_scope.h"
#include "bat/ledger/internal/core/future.h"
#include "bat/ledger/internal/core/future_cache.h"
#include "bat/ledger/internal/core/optional.h"
#include "bat/ledger/internal/rewards_wallet/rewards_wallet.h"

namespace ledger {

class RewardsWalletManager : public BATLedgerContext::Object {
 public:
  static const size_t kComponentKey;

  RewardsWalletManager();
  ~RewardsWalletManager() override;

  Future<bool> Initialize();

  Future<optional<RewardsWallet>> GetRewardsWallet();

  const optional<RewardsWallet>& GetCurrent() const { return current_; }

 private:
  FutureCache<optional<RewardsWallet>> cache_;
  optional<RewardsWallet> current_;
  CallbackScope callback_;
};

}  // namespace ledger

#endif  // BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_MANAGER_H_
