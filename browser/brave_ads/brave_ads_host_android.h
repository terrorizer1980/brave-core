// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_BRAVE_ADS_BRAVE_ADS_HOST_ANDROID_H_
#define BRAVE_BROWSER_BRAVE_ADS_BRAVE_ADS_HOST_ANDROID_H_

#include "brave/components/brave_ads/common/brave_ads_host.mojom.h"

class Profile;

namespace content {
class WebContents;
}

namespace brave_ads {

class BraveAdsHostAndroid : public brave_ads::mojom::BraveAdsHost {
 public:
  BraveAdsHostAndroid(Profile* profile, content::WebContents* web_contents);
  BraveAdsHostAndroid(const BraveAdsHostAndroid&) = delete;
  BraveAdsHostAndroid& operator=(const BraveAdsHostAndroid&) = delete;
  ~BraveAdsHostAndroid() override;

  // brave_ads::mojom::BraveAdsHost
  void RequestAdsEnabled(RequestAdsEnabledCallback callback) override;

 private:
  Profile* profile_;
};

}  // namespace brave_ads

#endif  // BRAVE_BROWSER_BRAVE_ADS_BRAVE_ADS_HOST_ANDROID_H_
