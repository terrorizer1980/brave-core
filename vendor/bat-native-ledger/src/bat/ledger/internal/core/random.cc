/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/core/random.h"

#include <vector>

#include "crypto/random.h"

namespace ledger {

const size_t Random::kComponentKey = BATLedgerContext::ReserveComponentKey();

std::vector<uint8_t> Random::GetRandomBytes(size_t length) {
  return GetRandomBytesImpl(length);
}

std::vector<uint8_t> Random::GetRandomBytesImpl(size_t length) {
  std::vector<uint8_t> buffer(length);
  crypto::RandBytes(buffer);
  return buffer;
}

}  // namespace ledger
