/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/tokens/issuers/issuers.h"

#include "base/json/json_reader.h"
#include "base/values.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/tokens/issuers/get_issuers_url_request_builder.h"
#include "net/http/http_status_code.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace ads {

Issuers::Issuers() = default;

Issuers::~Issuers() = default;

void Issuers::set_delegate(IssuersDelegate* delegate) {
  delegate_ = delegate;
}

// TODO(tmancey): Change to IssuersExist once issuers are persisted
bool Issuers::IsInitialized() {
  return initialized_;
}

// TODO(tmancey): How often should we fetch the latest issuers?
void Issuers::GetIssuers(const ConfirmationCallback& callback) {
  initialized_ = false;

  BLOG(1, "GetIssuers");
  BLOG(2, "GET /v1/issuers/");

  GetIssuersUrlRequestBuilder url_request_builder;
  mojom::UrlRequestPtr url_request = url_request_builder.Build();
  BLOG(6, UrlRequestToString(url_request));
  BLOG(7, UrlRequestHeadersToString(url_request));

  // TODO(tmancey): Rename to callback once ConfirmationCallback has been
  // removed
  auto request_callback =
      std::bind(&Issuers::OnGetIssuers, this, std::placeholders::_1, callback);
  AdsClientHelper::Get()->UrlRequest(std::move(url_request), request_callback);
}

bool Issuers::PublicKeyExists(const std::string& issuer_name,
                              const std::string& public_key) {
  if (!IsInitialized()) {
    // TODO(tmancey): Is this check needed as if not initialized the object
    // would be empty and would return false
    return false;
  }

  const auto iter = public_keys_.find(issuer_name);
  if (iter == public_keys_.end()) {
    return false;
  }

  // TODO(tmancey): Improve business logic
  return static_cast<bool>(iter->second.count(public_key));
}

//////////////////////////////////////////////////////////////////////////////

void Issuers::OnGetIssuers(const mojom::UrlResponse& url_response,
                           const ConfirmationCallback& callback) {
  BLOG(1, "OnGetIssuers");

  BLOG(6, UrlResponseToString(url_response));
  BLOG(7, UrlResponseHeadersToString(url_response));

  if (url_response.status_code != net::HTTP_OK) {
    BLOG(0, "Failed to get issuers");
    OnFailedToGetIssuers();

    if (callback) {
      callback();
    }

    return;
  }

  const bool success = ParseResponseBody(url_response);
  if (!success) {
    BLOG(3, "Failed to parse response: " << url_response.body);
    OnFailedToGetIssuers();

    if (callback) {
      callback();
    }

    return;
  }

  OnDidGetIssuers();

  // TODO(tmancey): Move callback to OnDidGetIssuers and OnFailedToGetIssuers
  if (callback) {
    callback();
  }
}

bool Issuers::ParseResponseBody(const mojom::UrlResponse& url_response) {
  public_keys_.clear();

  // TODO(tmancey): Decouple JSONReader to issuers_json_reader.h/.cc
  absl::optional<base::Value> issuer_list =
      base::JSONReader::Read(url_response.body);

  if (!issuer_list || !issuer_list->is_list()) {
    return false;
  }

  for (const auto& value : issuer_list->GetList()) {
    if (!value.is_dict()) {
      return false;
    }

    // TODO(tmancey): Why empty path?
    const base::Value* public_key_dict = value.FindPath("");
    const std::string* public_key_name = public_key_dict->FindStringKey("name");
    if (!public_key_name) {
      continue;
    }

    std::set<std::string> current_public_keys;
    const base::Value* public_key_list =
        public_key_dict->FindListKey("publicKeys");
    for (const auto& public_key : public_key_list->GetList()) {
      if (!public_key.is_string()) {
        continue;
      }
      current_public_keys.insert(public_key.GetString());
    }

    public_keys_[*public_key_name] = current_public_keys;
  }

  return true;
}

void Issuers::OnDidGetIssuers() {
  initialized_ = true;

  if (!delegate_) {
    return;
  }

  delegate_->OnDidGetIssuers();
}

void Issuers::OnFailedToGetIssuers() {
  // TODO(tmancey): not required as if we fail we should keep the old issuers
  initialized_ = false;

  if (!delegate_) {
    return;
  }

  delegate_->OnFailedToGetIssuers();
}

}  // namespace ads
