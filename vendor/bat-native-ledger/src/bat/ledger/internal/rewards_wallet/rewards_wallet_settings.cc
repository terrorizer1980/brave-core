/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/rewards_wallet/rewards_wallet_settings.h"

namespace ledger {

const RewardsWalletSettings RewardsWalletSettings::kDevelopment = {
    .grant_server_host = "grant.rewards.brave.software",
};

const RewardsWalletSettings RewardsWalletSettings::kStaging = {
    .grant_server_host = "grant.rewards.bravesoftware.com",
};

const RewardsWalletSettings RewardsWalletSettings::kProduction = {
    .grant_server_host = "grant.rewards.brave.com",
};

}  // namespace ledger
