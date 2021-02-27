/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/rewards_wallet/rewards_wallet_manager.h"

#include <utility>

#include "bat/ledger/internal/rewards_wallet/create_wallet_endpoint.h"
#include "bat/ledger/internal/rewards_wallet/rewards_wallet_store.h"

namespace ledger {

class GenerateJob : public BATLedgerJob<optional<RewardsWallet>> {
 public:
  void Start() {
    context().Get<CreateWalletEndpoint>().CreateRewardsWallet().Then(
        ContinueWith(&GenerateJob::OnWalletCreated));
  }

 private:
  void OnWalletCreated(optional<RewardsWallet> wallet) {
    // TODO(zenparsing): Should this component be in charge of retries? For how
    // long? Do callers wait forever? Who are the callers?
    if (!wallet)
      return Complete({});

    // TODO(zenparsing): Error handling here?
    context().Get<RewardsWalletStore>().Write(*wallet);
    Complete(std::move(wallet));
  }
};

RewardsWalletManager::RewardsWalletManager() = default;

RewardsWalletManager::~RewardsWalletManager() = default;

const size_t RewardsWalletManager::kComponentKey =
    BATLedgerContext::ReserveComponentKey();

Future<bool> RewardsWalletManager::Initialize() {
  return Future<bool>::Create([this](auto resolver) {
    context().Get<RewardsWalletStore>().Read().Then(callback_(
        [this, resolver](RewardsWalletStore::ReadResult result) mutable {
          current_ = std::move(result.wallet);
          resolver.Complete(!result.corrupted);
        }));
  });
}

Future<optional<RewardsWallet>> RewardsWalletManager::GetRewardsWallet() {
  if (current_)
    return Future<optional<RewardsWallet>>::Completed(current_);

  return cache_.GetFuture([this]() {
    FuturePair<optional<RewardsWallet>> future_pair;
    auto resolver = future_pair.resolver;

    context().StartJob<GenerateJob>().Then(
        callback_([this, resolver](optional<RewardsWallet> wallet) mutable {
          // Upon completion, set the current value so that legacy callers can
          // access the data synchronously.
          current_ = wallet;
          resolver.Complete(std::move(wallet));
        }));

    return std::move(future_pair.future);
  });
}

}  // namespace ledger
