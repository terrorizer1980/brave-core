/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_CORE_RANDOM_H_
#define BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_CORE_RANDOM_H_

#include <vector>

#include "base/containers/span.h"
#include "bat/ledger/internal/core/bat_ledger_context.h"

namespace ledger {

class Random : public BATLedgerContext::Object {
 public:
  static const size_t kComponentKey;

  std::vector<uint8_t> GetRandomBytes(size_t length);

 protected:
  virtual std::vector<uint8_t> GetRandomBytesImpl(size_t length);
};

}  // namespace ledger

#endif  // BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_CORE_RANDOM_H_
