/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/bundle/creative_ad_unittest_util.h"

#include "base/guid.h"

namespace ads {

namespace {

int64_t kDistantPastAsTimestamp = 0;
int64_t kDistantFutureAsTimestamp = 4102444799;

}  // namespace

CreativeAdInfo GetCreativeAd() {
  CreativeAdInfo creative_ad;

  creative_ad.creative_instance_id = base::GenerateGUID();
  creative_ad.creative_set_id = base::GenerateGUID();
  creative_ad.campaign_id = base::GenerateGUID();
  creative_ad.start_at_timestamp = kDistantPastAsTimestamp;  // todo(Moritz Haller): use TimestampFromDateString
  creative_ad.end_at_timestamp = kDistantFutureAsTimestamp;
  creative_ad.daily_cap = 1;
  creative_ad.advertiser_id = base::GenerateGUID();
  creative_ad.priority = 1;
  creative_ad.ptr = 1.0;
  creative_ad.per_day = 1;
  creative_ad.per_week = 1;
  creative_ad.per_month = 1;
  creative_ad.total_max = 1;
  creative_ad.segment = "untargeted";
  creative_ad.geo_targets = {"US"};
  creative_ad.target_url = "https://brave.com";
  CreativeDaypartInfo daypart;
  creative_ad.dayparts = {daypart};

  return creative_ad;
}

}  // namespace ads
