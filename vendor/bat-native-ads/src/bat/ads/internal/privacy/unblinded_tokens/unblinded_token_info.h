/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_PRIVACY_UNBLINDED_TOKENS_UNBLINDED_TOKEN_INFO_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_PRIVACY_UNBLINDED_TOKENS_UNBLINDED_TOKEN_INFO_H_

#include <vector>

#include "bat/ads/ad_type.h"
#include "bat/ads/confirmation_type.h"
#include "wrapper.hpp"

namespace ads {
namespace privacy {

using challenge_bypass_ristretto::PublicKey;
using challenge_bypass_ristretto::UnblindedToken;

struct UnblindedTokenInfo {
  UnblindedTokenInfo();
  UnblindedTokenInfo(const UnblindedTokenInfo& info);
  ~UnblindedTokenInfo();

  bool operator==(const UnblindedTokenInfo& rhs) const;
  bool operator!=(const UnblindedTokenInfo& rhs) const;

  UnblindedToken value;
  PublicKey public_key;
  ConfirmationType confirmation_type;
  AdType ad_type;
};

using UnblindedTokenList = std::vector<UnblindedTokenInfo>;

}  // namespace privacy
}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_PRIVACY_UNBLINDED_TOKENS_UNBLINDED_TOKEN_INFO_H_
