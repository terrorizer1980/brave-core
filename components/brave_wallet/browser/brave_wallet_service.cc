/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/brave_wallet_service.h"

#include "components/prefs/pref_service.h"

//
// {
//    "0x1": {
//      known_tokens: ["eth", "0x0D8775F648430679A709E98d2b0Cb6250d2887EF"],
//      custom_tokens: [
//        {
//          "contract_address": "0x4729c2017edD1BaDf768595378c668955b537197",
//          "name": "MOR",
//          "symbol": "MOR",
//          "is_erc20": true,
//          "is_erc721": false,
//          "decimals": 18
//        }
//      ]
//    },
//    "0x2": {
//      known_tokens: [ ... ],
//      custom_tokens: [ ... ]
//    },
//    ...
//    }
// }
//

namespace brave_wallet {

BraveWalletService::BraveWalletService(PrefService* prefs) : prefs_(prefs) {
  DCHECK(prefs_);
}

BraveWalletService::~BraveWalletService() = default;

mojo::PendingRemote<mojom::BraveWalletService>
BraveWalletService::MakeRemote() {
  mojo::PendingRemote<mojom::BraveWalletService> remote;
  receivers_.Add(this, remote.InitWithNewPipeAndPassReceiver());
  return remote;
}

void BraveWalletService::Bind(
    mojo::PendingReceiver<mojom::BraveWalletService> receiver) {
  receivers_.Add(this, std::move(receiver));
}

void BraveWalletService::GetAllVisibleTokens(
    mojom::Network network,
    GetAllVisibleTokensCallback callback) {
  // TODO(jocelyn): Implement it.
  std::move(callback).Run(std::vector<mojom::ERCTokenPtr>());
}

void BraveWalletService::AddVisibleKnownToken(
    const std::string& contract_address,
    mojom::Network network,
    AddVisibleKnownTokenCallback callback) {
  // TODO(jocelyn): Implement it.
  std::move(callback).Run(false);
}

void BraveWalletService::AddVisibleCustomToken(
    mojom::ERCTokenPtr token,
    mojom::Network network,
    AddVisibleCustomTokenCallback callback) {
  // TODO(jocelyn): Implement it.
  std::move(callback).Run(false);
}

}  // namespace brave_wallet
