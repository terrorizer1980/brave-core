/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/rewards_wallet/rewards_wallet_store.h"

#include <string>
#include <utility>

#include "bat/ledger/internal/core/bat_ledger_job.h"
#include "bat/ledger/internal/core/sql_store.h"
#include "bat/ledger/internal/core/user_encryption.h"

namespace ledger {

namespace {

class ReadJob : public BATLedgerJob<RewardsWalletStore::ReadResult> {
 public:
  void Start() {
    context()
        .Get<SQLStore>()
        .Query(
            "SELECT payment_id, recovery_seed, created_at FROM rewards_wallet")
        .Then(ContinueWith(&ReadJob::OnReadComplete));
  }

 private:
  using ReadResult = RewardsWalletStore::ReadResult;

  ReadResult NotFound() { return ReadResult(); }

  ReadResult Corrupted() {
    ReadResult result;
    result.corrupted = true;
    return result;
  }

  ReadResult Success(RewardsWallet wallet) {
    ReadResult result;
    result.wallet = std::move(wallet);
    return result;
  }

  void OnReadComplete(SQLReader reader) {
    if (!reader.Step())
      return Complete(NotFound());

    std::string payment_id = reader.ColumnString(0);
    if (payment_id.empty())
      return Complete(Corrupted());

    auto seed = context().Get<UserEncryption>().Base64DecryptString(
        reader.ColumnString(1));

    if (!seed) {
      context().LogError(FROM_HERE) << "Unable to decrypt recovery seed";
      return Complete(Corrupted());
    }

    if ((*seed).size() != RewardsWallet::kSeedLength) {
      context().LogError(FROM_HERE) << "Invalid seed length";
      return Complete(Corrupted());
    }

    auto created_at = base::Time::FromJsTime(reader.ColumnDouble(2));

    RewardsWallet wallet(payment_id, *seed, created_at);

    Complete(Success(RewardsWallet(payment_id, *seed, created_at)));
  }
};

class WriteJob : public BATLedgerJob<bool> {
 public:
  void Start(const RewardsWallet& wallet) {
    DCHECK(!wallet.payment_id().empty());
    DCHECK(!wallet.recovery_seed().empty());
    DCHECK(!wallet.created_at().is_null());

    std::string seed_string(wallet.recovery_seed().begin(),
                            wallet.recovery_seed().end());

    auto encrypted_seed =
        context().Get<UserEncryption>().Base64EncryptString(seed_string);

    if (!encrypted_seed) {
      context().LogError(FROM_HERE) << "Unable to encrypt recovery seed";
      return Complete(false);
    }

    double created_at = wallet.created_at().ToJsTimeIgnoringNull();

    context()
        .Get<SQLStore>()
        .Execute(
            "INSERT INTO rewards_wallet (payment_id, recovery_seed, "
            "created_at) VALUES (?, ?, ?)",
            wallet.payment_id(), *encrypted_seed, created_at)
        .Then(ContinueWith(&WriteJob::OnInsertComplete));
  }

 private:
  void OnInsertComplete(SQLReader reader) {
    if (!reader.Succeeded())
      context().LogError(FROM_HERE) << "Error saving wallet to database";

    Complete(reader.Succeeded());
  }
};

}  // namespace

RewardsWalletStore::ReadResult::ReadResult() = default;

RewardsWalletStore::ReadResult::~ReadResult() = default;

RewardsWalletStore::ReadResult::ReadResult(const ReadResult& other) = default;

const size_t RewardsWalletStore::kComponentKey =
    BATLedgerContext::ReserveComponentKey();

Future<RewardsWalletStore::ReadResult> RewardsWalletStore::Read() {
  return context().StartJob<ReadJob>();
}

Future<bool> RewardsWalletStore::Write(const RewardsWallet& wallet) {
  return context().StartJob<WriteJob>(wallet);
}

}  // namespace ledger
