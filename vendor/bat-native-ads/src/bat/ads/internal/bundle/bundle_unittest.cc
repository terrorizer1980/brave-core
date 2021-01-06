/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/bundle/bundle.h"

#include "bat/ads/internal/catalog/catalog.h"
#include "bat/ads/internal/database/tables/conversions_database_table.h"
#include "bat/ads/internal/database/tables/creative_ad_notifications_database_table.h"
#include "bat/ads/internal/database/tables/creative_new_tab_page_ads_database_table.h"
#include "bat/ads/internal/unittest_base.h"
#include "bat/ads/internal/unittest_util.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {

namespace {

const char kEmptyCatalog[] = "empty_catalog.json";

const char kCatalogWithSingleCampaign[] =
    "catalog_with_single_campaign.json";

// const char kCatalogWithMultipleCampaigns[] =
//     "catalog_with_multiple_campaigns.json";

}  // namespace

class BatAdsBundleTest : public UnitTestBase {
 protected:
  BatAdsBundleTest() = default;

  ~BatAdsBundleTest() override = default;

  CreativeAdNotificationList BuildCreativeAdNotificationsForCampaigns(
      const CatalogCampaignList& catalog_campaigns) {
    CreativeAdNotificationList creative_ad_notifications;

    for (const auto& catalog_campaign : catalog_campaigns) {
      CreativeAdNotificationInfo creative_ad_notification;

      creative_ad_notification.campaign_id = catalog_campaign.campaign_id;
      creative_ad_notification.daily_cap = catalog_campaign.daily_cap;
      creative_ad_notification.advertiser_id = catalog_campaign.advertiser_id;
      creative_ad_notification.priority = catalog_campaign.priority;
      creative_ad_notification.ptr = catalog_campaign.ptr;

      for (const auto& catalog_creative_set : catalog_campaign.creative_sets) {
        creative_ad_notification.creative_set_id =
            catalog_creative_set.creative_set_id;
        creative_ad_notification.conversion =
            !catalog_creative_set.conversions.empty();
        creative_ad_notification.per_day = catalog_creative_set.per_day;
        creative_ad_notification.total_max = catalog_creative_set.total_max;

        for (const auto& catalog_creative_ad_notification :
            catalog_creative_set.creative_ad_notifications) {
          creative_ad_notification.body =
              catalog_creative_ad_notification.payload.body;
          creative_ad_notification.title =
              catalog_creative_ad_notification.payload.title;
          creative_ad_notification.creative_instance_id =
              catalog_creative_ad_notification.creative_instance_id;
          creative_ad_notification.target_url =
              catalog_creative_ad_notification.payload.target_url;

          for (const auto& segment : catalog_creative_set.segments) {
            creative_ad_notification.segment = segment.name;

            for (const auto& geo_target : catalog_campaign.geo_targets) {
              creative_ad_notification.start_at_timestamp = 0;
              creative_ad_notification.end_at_timestamp = 0;
              creative_ad_notification.geo_targets.push_back(geo_target.code);

              for (const auto& daypart : catalog_campaign.dayparts) {
                CreativeDaypartInfo creative_daypart;
                creative_daypart.dow = daypart.dow;
                creative_daypart.start_minute = daypart.start_minute;
                creative_daypart.end_minute = daypart.end_minute;
                creative_ad_notification.dayparts.push_back(creative_daypart);

                creative_ad_notifications.push_back(creative_ad_notification);
              }
            }
          }
        }
      }
    }

    return creative_ad_notifications;
  }

  CreativeNewTabPageAdList BuildCreativeNewTabPageAdsForCampaigns(
      const CatalogCampaignList& catalog_campaigns) {
    CreativeNewTabPageAdList creative_new_tab_page_ads;

    for (const auto& catalog_campaign : catalog_campaigns) {
      CreativeNewTabPageAdInfo creative_new_tab_page_ad;

      creative_new_tab_page_ad.campaign_id = catalog_campaign.campaign_id;
      creative_new_tab_page_ad.daily_cap = catalog_campaign.daily_cap;
      creative_new_tab_page_ad.advertiser_id = catalog_campaign.advertiser_id;
      creative_new_tab_page_ad.priority = catalog_campaign.priority;
      creative_new_tab_page_ad.ptr = catalog_campaign.ptr;

      for (const auto& catalog_creative_set : catalog_campaign.creative_sets) {
        creative_new_tab_page_ad.creative_set_id =
            catalog_creative_set.creative_set_id;
        creative_new_tab_page_ad.conversion =
            !catalog_creative_set.conversions.empty();
        creative_new_tab_page_ad.per_day = catalog_creative_set.per_day;
        creative_new_tab_page_ad.total_max = catalog_creative_set.total_max;

        for (const auto& catalog_creative_new_tab_page_ad :
            catalog_creative_set.creative_new_tab_page_ads) {
          creative_new_tab_page_ad.company_name =
              catalog_creative_new_tab_page_ad.payload.company_name;
          creative_new_tab_page_ad.alt =
              catalog_creative_new_tab_page_ad.payload.alt;
          creative_new_tab_page_ad.creative_instance_id =
              catalog_creative_new_tab_page_ad.creative_instance_id;
          creative_new_tab_page_ad.target_url =
              catalog_creative_new_tab_page_ad.payload.target_url;

          for (const auto& segment : catalog_creative_set.segments) {
            creative_new_tab_page_ad.segment = segment.name;

            for (const auto& geo_target : catalog_campaign.geo_targets) {
              creative_new_tab_page_ad.start_at_timestamp = 0;
              creative_new_tab_page_ad.end_at_timestamp = 0;
              creative_new_tab_page_ad.geo_targets.push_back(geo_target.code);

              for (const auto& daypart : catalog_campaign.dayparts) {
                CreativeDaypartInfo creative_daypart;
                creative_daypart.dow = daypart.dow;
                creative_daypart.start_minute = daypart.start_minute;
                creative_daypart.end_minute = daypart.end_minute;
                creative_new_tab_page_ad.dayparts.push_back(creative_daypart);

                creative_new_tab_page_ads.push_back(creative_new_tab_page_ad);
              }
            }
          }
        }
      }
    }

    return creative_new_tab_page_ads;
  }
};

TEST_F(BatAdsBundleTest,
    BuildFromEmptyCatalog) {
  // Arrange
  const base::Optional<std::string> json =
      ReadFileFromTestPathToString(kEmptyCatalog);
  ASSERT_NE(base::nullopt, json);

  Catalog catalog;
  ASSERT_TRUE(catalog.FromJson(*json));

  // Act
  Bundle bundle;
  bundle.BuildFromCatalog(catalog);

  // Assert
  database::table::CreativeAdNotifications
      creative_ad_notifications_database_table;
  creative_ad_notifications_database_table.GetAll([](
      const Result result,
      const SegmentList& segments,
      const CreativeAdNotificationList& creative_ad_notifications) {
    ASSERT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(creative_ad_notifications.empty());
  });

  database::table::CreativeNewTabPageAds
      creative_new_tab_page_ads_database_table;
  creative_new_tab_page_ads_database_table.GetAll([](
      const Result result,
      const SegmentList& segments,
      const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
    ASSERT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(creative_new_tab_page_ads.empty());
  });

  database::table::Conversions conversions_database_table;
  conversions_database_table.GetAll([](
      const Result result,
      const ConversionList& conversions) {
    ASSERT_EQ(Result::SUCCESS, result);
    EXPECT_TRUE(conversions.empty());
  });
}

TEST_F(BatAdsBundleTest,
    BuildFromCatalogWithSingleCampaign) {
  // Arrange
  const base::Optional<std::string> json =
      ReadFileFromTestPathToString(kCatalogWithSingleCampaign);
  ASSERT_NE(base::nullopt, json);

  Catalog catalog;
  ASSERT_TRUE(catalog.FromJson(*json));

  // Act
  Bundle bundle;
  bundle.BuildFromCatalog(catalog);

  // Assert
  const CatalogCampaignList catalog_campaigns = catalog.GetCampaigns();

  CreativeAdNotificationList expected_creative_ad_notifications =
      BuildCreativeAdNotificationsForCampaigns(catalog_campaigns);

  database::table::CreativeAdNotifications database_table;
  database_table.GetAll([&expected_creative_ad_notifications](
      const Result result,
      const SegmentList& segments,
      const CreativeAdNotificationList& creative_ad_notifications) {
    ASSERT_EQ(Result::SUCCESS, result);

    std::cout << "FOOBAR: " << expected_creative_ad_notifications.size() << std::endl;
    std::cout << "FOOBAR: " << creative_ad_notifications.size() << std::endl;

    EXPECT_EQ(expected_creative_ad_notifications, creative_ad_notifications);
  });

  // database::table::CreativeNewTabPageAds
  //     creative_new_tab_page_ads_database_table;
  // creative_new_tab_page_ads_database_table.GetAll([&campaigns](
  //     const Result result,
  //     const SegmentList& segments,
  //     const CreativeNewTabPageAdList& creative_new_tab_page_ads) {
  //   ASSERT_EQ(Result::SUCCESS, result);

  //   CreativeNewTabPageAdList expected_creative_new_tab_page_ads;
  //   for (const auto& campaign : campaigns) {
  //     for (const auto& creative_set : campaign.creative_sets) {
  //       expected_creative_new_tab_page_ads.insert(
  //           expected_creative_new_tab_page_ads.end(),
  //               creative_set.creative_new_tab_page_ads.begin(),
  //                   creative_set.creative_new_tab_page_ads.end());
  //     }
  //   }

  //   EXPECT_EQ(expected_creative_new_tab_page_ads, creative_new_tab_page_ads);
  // });

  // database::table::Conversions conversions_database_table;
  // conversions_database_table.GetAll([&campaigns](
  //     const Result result,
  //     const ConversionList& conversions) {
  //   ASSERT_EQ(Result::SUCCESS, result);

  //   ConversionList expected_conversions;
  //   for (const auto& campaign : campaigns) {
  //     for (const auto& creative_set : campaign.creative_sets) {
  //       expected_conversions.insert(expected_conversions.end(),
  //           creative_set.conversions.begin(), creative_set.conversions.end());
  //     }
  //   }

  //   EXPECT_EQ(expected_conversions, conversions);
  // });
}

// TEST_F(BatAdsBundleTest,
//     BuildFromCatalogWithMultipleCampaigns) {
//   // Arrange
//   const base::Optional<std::string> json =
//       ReadFileFromTestPathToString(kCatalogWithMultipleCampaigns);
//   ASSERT_NE(base::nullopt, json);

//   Catalog catalog;
//   ASSERT_TRUE(catalog.FromJson(*json));

//   // Act

//   // Assert
//   CatalogCampaignList expected_catalog_campaigns;
//   CatalogCampaignInfo catalog_campaign_1 = BuildCatalogCampaign1();
//   expected_catalog_campaigns.push_back(catalog_campaign_1);
//   CatalogCampaignInfo catalog_campaign_2 = BuildCatalogCampaign2();
//   expected_catalog_campaigns.push_back(catalog_campaign_2);

//   EXPECT_EQ(expected_catalog_campaigns, catalog_campaigns);
// }

}  // namespace ads
