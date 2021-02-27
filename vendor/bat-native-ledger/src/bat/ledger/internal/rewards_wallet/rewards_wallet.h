/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_H_
#define BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_H_

#include <string>
#include <utility>
#include <vector>

#include "base/time/time.h"

namespace ledger {

class RewardsWallet {
 public:
  static constexpr int kSeedLength = 32;

  RewardsWallet(const std::string& payment_id,
                const std::vector<uint8_t>& recovery_seed,
                base::Time created_at);

  RewardsWallet(const std::string& payment_id,
                const std::string& recovery_seed,
                base::Time created_at);

  RewardsWallet(const RewardsWallet& other);

  ~RewardsWallet();

  static RewardsWallet CreateWithEmptyPaymentId();

  const std::string& payment_id() const { return payment_id_; }

  const std::vector<uint8_t>& recovery_seed() const { return recovery_seed_; }

  const base::Time created_at() const { return created_at_; }

  struct KeyPair {
    KeyPair();
    KeyPair(const KeyPair& other);
    ~KeyPair();

    std::vector<uint8_t> public_key;
    std::vector<uint8_t> secret_key;
  };

  KeyPair GenerateKeyPair();

  std::string CreateMessageDigest(const std::string& message);

  std::string SignHeaders(
      const std::string& key_kd,
      const std::vector<std::pair<std::string, std::string>>& headers);

 private:
  RewardsWallet();

  std::string payment_id_;
  std::vector<uint8_t> recovery_seed_;
  base::Time created_at_;
};

}  // namespace ledger

#endif  // BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_REWARDS_WALLET_REWARDS_WALLET_H_
