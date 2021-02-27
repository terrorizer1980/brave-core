/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/rewards_wallet/rewards_wallet.h"

#include <array>
#include <sstream>

#include <openssl/digest.h>  // NOLINT
#include <openssl/hkdf.h>    // NOLINT

#include "base/base64.h"
#include "base/strings/string_util.h"
#include "crypto/random.h"
#include "crypto/sha2.h"

#include "tweetnacl.h"  // NOLINT

namespace ledger {

namespace {

constexpr std::array<uint8_t, 64> kHkdfSalt = {
    126, 244, 99,  158, 51,  68,  253, 80,  133, 183, 51,  180, 77,
    62,  74,  252, 62,  106, 96,  125, 241, 110, 134, 87,  190, 208,
    158, 84,  125, 69,  246, 207, 162, 247, 107, 172, 37,  34,  53,
    246, 105, 20,  215, 5,   248, 154, 179, 191, 46,  17,  6,   72,
    210, 91,  10,  169, 145, 248, 22,  147, 117, 24,  105, 12};

}  // namespace

RewardsWallet::KeyPair::KeyPair() = default;

RewardsWallet::KeyPair::KeyPair(const KeyPair& other) = default;

RewardsWallet::KeyPair::~KeyPair() = default;

RewardsWallet::RewardsWallet(const std::string& payment_id,
                             const std::vector<uint8_t>& recovery_seed,
                             base::Time created_at)
    : payment_id_(payment_id),
      recovery_seed_(recovery_seed),
      created_at_(created_at) {
  DCHECK(!payment_id_.empty());
  DCHECK(recovery_seed_.size() == kSeedLength);
}

RewardsWallet::RewardsWallet(const std::string& payment_id,
                             const std::string& recovery_seed,
                             base::Time created_at)
    : payment_id_(payment_id),
      recovery_seed_(recovery_seed.begin(), recovery_seed.end()),
      created_at_(created_at) {
  DCHECK(!payment_id_.empty());
  DCHECK(recovery_seed_.size() == kSeedLength);
}

RewardsWallet::RewardsWallet() = default;

RewardsWallet::RewardsWallet(const RewardsWallet& other) = default;

RewardsWallet::~RewardsWallet() = default;

RewardsWallet RewardsWallet::CreateWithEmptyPaymentId() {
  RewardsWallet wallet;
  wallet.recovery_seed_.resize(kSeedLength);
  crypto::RandBytes(wallet.recovery_seed_.data(), kSeedLength);
  return wallet;
}

RewardsWallet::KeyPair RewardsWallet::GenerateKeyPair() {
  DCHECK(!recovery_seed_.empty());

  std::vector<uint8_t> seed(kSeedLength);
  std::vector<uint8_t> info = {0};

  // We need to use openssl directly instead of |crypto::HkdfSha256| so that we
  // can specify SHA512.
  int result =
      HKDF(seed.data(), kSeedLength, EVP_sha512(), recovery_seed_.data(),
           recovery_seed_.size(), kHkdfSalt.data(), kHkdfSalt.size(),
           info.data(), info.size());

  DCHECK(result);

  KeyPair pair;
  pair.public_key.resize(crypto_sign_PUBLICKEYBYTES);
  pair.secret_key = std::move(seed);
  pair.secret_key.resize(crypto_sign_SECRETKEYBYTES);

  crypto_sign_keypair(pair.public_key.data(), pair.secret_key.data(), 1);

  return pair;
}

std::string RewardsWallet::CreateMessageDigest(const std::string& message) {
  std::string digest;
  base::Base64Encode(crypto::SHA256HashString(message), &digest);
  return "SHA-256=" + digest;
}

std::string RewardsWallet::SignHeaders(
    const std::string& key_id,
    const std::vector<std::pair<std::string, std::string>>& headers) {
  // Construct the message to be signed by concatenating the header pairs.
  std::vector<std::string> message_parts;
  std::vector<std::string> header_keys;
  for (auto& pair : headers) {
    message_parts.push_back(pair.first + ": " + pair.second);
    header_keys.push_back(pair.first);
  }
  std::string message = base::JoinString(message_parts, "\n");

  KeyPair keypair = GenerateKeyPair();

  // Sign the headers using the wallet's secret key - |signed_message| will
  // contain the signature followed by a copy of the plain text message.
  uint64_t signed_size = 0;
  std::vector<uint8_t> signed_message(crypto_sign_BYTES + message.size());
  crypto_sign(signed_message.data(), &signed_size,
              reinterpret_cast<const uint8_t*>(message.data()), message.size(),
              keypair.secret_key.data());

  // Discard the plain text portion of the message.
  signed_message.resize(crypto_sign_BYTES);

  std::ostringstream stream;
  stream << "keyId=\"" << key_id << "\","
         << "algorithm=\"ed25519\","
         << "headers=\"" << base::JoinString(header_keys, " ") << "\","
         << "signature=\"" << base::Base64Encode(signed_message) << "\"";

  return stream.str();
}

}  // namespace ledger
