/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/wallet/wallet_create.h"

#include <utility>
#include <vector>

#include "bat/ledger/internal/common/time_util.h"
#include "bat/ledger/internal/constants.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/rewards_wallet/rewards_wallet_manager.h"

using std::placeholders::_1;

namespace ledger {
namespace wallet {

WalletCreate::WalletCreate(LedgerImpl* ledger) : ledger_(ledger) {
  DCHECK(ledger_);
}

WalletCreate::~WalletCreate() = default;

void WalletCreate::Start(ledger::ResultCallback callback) {
  auto wallet = ledger_->wallet()->GetWallet();
  if (wallet && !wallet->payment_id.empty()) {
    BLOG(1, "Wallet already exists");
    callback(type::Result::WALLET_CREATED);
    return;
  }

  ledger_->context().Get<RewardsWalletManager>().GetRewardsWallet().Then(
      base::BindOnce(&WalletCreate::OnCreate, base::Unretained(this),
                     callback));
}

void WalletCreate::OnCreate(ledger::ResultCallback callback,
                            optional<RewardsWallet> wallet) {
  if (!wallet) {
    callback(type::Result::LEDGER_ERROR);
    return;
  }

  ledger_->state()->ResetReconcileStamp();
  if (!ledger::is_testing) {
    ledger_->state()->SetFetchOldBalanceEnabled(false);
    ledger_->state()->SetEmptyBalanceChecked(true);
    ledger_->state()->SetPromotionCorruptedMigrated(true);
  }
  ledger_->state()->SetCreationStamp(util::GetCurrentTimeStamp());
  callback(type::Result::WALLET_CREATED);
}

}  // namespace wallet
}  // namespace ledger
