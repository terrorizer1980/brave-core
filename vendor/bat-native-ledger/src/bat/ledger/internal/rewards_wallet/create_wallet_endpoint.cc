/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/rewards_wallet/create_wallet_endpoint.h"

#include <string>
#include <utility>
#include <vector>

#include "base/base64.h"
#include "base/json/json_reader.h"
#include "base/strings/string_number_conversions.h"
#include "bat/ledger/internal/core/url_fetcher.h"
#include "bat/ledger/internal/rewards_wallet/rewards_wallet_settings.h"
#include "net/http/http_status_code.h"

namespace ledger {

namespace {

const char kPath[] = "/v3/wallet/brave";

class RequestJob : public BATLedgerJob<optional<RewardsWallet>> {
 public:
  void Start() {
    auto& settings = context().GetSettings<RewardsWalletSettings>();
    std::string host(settings.grant_server_host);

    auto request = mojom::UrlRequest::New();
    request->method = mojom::UrlMethod::POST;
    request->url = "https://" + host + kPath;
    request->headers = GetHeaders();

    context()
        .Get<URLFetcher>()
        .Fetch(std::move(request))
        .Then(ContinueWith(&RequestJob::OnResponse));
  }

 private:
  std::vector<std::string> GetHeaders() {
    std::string key_id = base::HexEncode(wallet_.GenerateKeyPair().public_key);

    // Since the payload is empty, |digest| is the hash of the empty string.
    std::string digest = wallet_.CreateMessageDigest("");

    std::string signature = wallet_.SignHeaders(
        key_id, {{"digest", digest},
                 {"(request-target)", "post " + std::string(kPath)}});

    return {"digest: " + digest, "signature: " + signature,
            "accept: application/json"};
  }

  void OnResponse(mojom::UrlResponsePtr response) {
    DCHECK(response);

    if (!IsValidStatus(response->status_code))
      return Complete({});

    std::string payment_id = ParseResponse(response->body);
    if (payment_id.empty())
      return Complete({});

    RewardsWallet wallet(payment_id, wallet_.recovery_seed(),
                         base::Time::Now());

    Complete(std::move(wallet));
  }

  bool IsValidStatus(int status_code) {
    switch (status_code) {
      case net::HTTP_SERVICE_UNAVAILABLE:
        context().LogError(FROM_HERE)
            << "No conversion rate yet in ratios service";
        return false;
      case net::HTTP_CREATED:
        return true;
      default:
        context().LogError(FROM_HERE)
            << "Unexpected HTTP status: " << status_code;
        return false;
    }
  }

  std::string ParseResponse(const std::string& body) {
    auto root = base::JSONReader::Read(body);
    if (!root || !root->is_dict()) {
      context().LogError(FROM_HERE) << "Invalid JSON";
      return "";
    }

    auto* payment_id = root->FindStringKey("paymentId");
    if (!payment_id || payment_id->empty()) {
      context().LogError(FROM_HERE) << "Missing paymentId in response";
      return "";
    }

    return *payment_id;
  }

  RewardsWallet wallet_ = RewardsWallet::CreateWithEmptyPaymentId();
};

}  // namespace

const size_t CreateWalletEndpoint::kComponentKey =
    BATLedgerContext::ReserveComponentKey();

Future<optional<RewardsWallet>> CreateWalletEndpoint::CreateRewardsWallet() {
  return context().StartJob<RequestJob>();
}

}  // namespace ledger
