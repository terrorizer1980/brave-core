/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/exclusion_rules/dislike_frequency_cap.h"

#include "bat/ads/internal/unittest_base.h"
#include "bat/ads/internal/unittest_util.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {

namespace {
const char kCreativeInstanceId[] = "72a8d5ce-b785-4764-b3cd-e2e310d71778";
const char kCreativeSetId[] = "654f10df-fbc4-4a92-8d43-2edf73734a60";
}  // namespace

class BatAdsDislikeFrequencyCapTest : public UnitTestBase {
 protected:
  BatAdsDislikeFrequencyCapTest() = default;

  ~BatAdsDislikeFrequencyCapTest() override = default;
};

TEST_F(BatAdsDislikeFrequencyCapTest, AllowAd) {
  // Arrange
  CreativeAdInfo ad;
  ad.creative_set_id = kCreativeSetId;

  // Act
  DislikeFrequencyCap frequency_cap;
  const bool should_exclude = frequency_cap.ShouldExclude(ad);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BatAdsDislikeFrequencyCapTest, DoNotAllowAd) {
  // Arrange
  CreativeAdInfo ad;
  ad.creative_set_id = kCreativeSetId;

  AdContentInfo ad_content;
  ad_content.creative_instance_id = kCreativeInstanceId;
  ad_content.creative_set_id = kCreativeSetId;
  ad_content.like_action = AdContentInfo::LikeAction::kNeutral;
  Client::Get()->ToggleAdThumbDown(ad_content);

  // Act
  DislikeFrequencyCap frequency_cap;
  const bool should_exclude = frequency_cap.ShouldExclude(ad);

  // Assert
  EXPECT_TRUE(should_exclude);
}

}  // namespace ads
