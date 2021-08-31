/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_TOKENS_ISSUERS_ISSUERS_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_TOKENS_ISSUERS_ISSUERS_H_

#include <functional>
#include <map>
#include <set>
#include <string>

#include "base/callback.h"
#include "bat/ads/internal/tokens/issuers/issuers_delegate.h"
#include "bat/ads/public/interfaces/ads.mojom.h"

namespace ads {

// TODO(tmancey): Issuers should be fetched and persisted. We then need to add a
// frequency cap to make sure we have issuers before serving ads
using ConfirmationCallback = std::function<void()>;

class Issuers {
 public:
  Issuers();

  ~Issuers();

  void set_delegate(IssuersDelegate* delegate);

  bool IsInitialized();

  void GetIssuers(const ConfirmationCallback& callback);

  bool PublicKeyExists(const std::string& issuer_name,
                       const std::string& public_key);

 private:
  void OnGetIssuers(const mojom::UrlResponse& url_response,
                    const ConfirmationCallback& callback);

  bool ParseResponseBody(const mojom::UrlResponse& url_response);

  void OnDidGetIssuers();
  void OnFailedToGetIssuers();

  std::map<std::string, std::set<std::string>> public_keys_;
  bool initialized_ = false;

  IssuersDelegate* delegate_ = nullptr;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_TOKENS_ISSUERS_ISSUERS_H_
