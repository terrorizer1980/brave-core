/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_SETTINGS_H_
#define BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_SETTINGS_H_

namespace ledger {

struct RewardsWalletSettings {
  static const RewardsWalletSettings kDevelopment;
  static const RewardsWalletSettings kStaging;
  static const RewardsWalletSettings kProduction;

  const char* grant_server_host;
};

}  // namespace ledger

#endif  // BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_SETTINGS_H_
