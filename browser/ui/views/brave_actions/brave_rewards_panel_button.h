// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_VIEWS_BRAVE_ACTIONS_BRAVE_REWARDS_PANEL_BUTTON_H_
#define BRAVE_BROWSER_UI_VIEWS_BRAVE_ACTIONS_BRAVE_REWARDS_PANEL_BUTTON_H_

#include <memory>
#include <string>
#include <utility>

#include "brave/browser/ui/webui/brave_rewards/rewards_panel_ui.h"
#include "brave/components/brave_rewards/browser/rewards_notification_service.h"
#include "brave/components/brave_rewards/browser/rewards_notification_service_observer.h"
#include "chrome/browser/ui/views/bubble/webui_bubble_manager.h"
#include "chrome/browser/ui/views/toolbar/toolbar_action_view.h"
#include "components/prefs/pref_change_registrar.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/menu_button_controller.h"
#include "ui/views/widget/widget_observer.h"

class Profile;

// A button that lives in the actions container and opens the Rewards panel.
class BraveRewardsPanelButton
    : public views::LabelButton,
      public views::WidgetObserver,
      public brave_rewards::RewardsNotificationServiceObserver {
 public:
  BraveRewardsPanelButton(Profile* profile,
                          ToolbarActionView::Delegate* delegate);

  ~BraveRewardsPanelButton() override;

  BraveRewardsPanelButton(const BraveRewardsPanelButton&) = delete;
  BraveRewardsPanelButton& operator=(const BraveRewardsPanelButton&) = delete;

  void UpdateImageAndText();

  // views::View:
  gfx::Size CalculatePreferredSize() const override;

  // views::LabelButton:
  std::unique_ptr<views::LabelButtonBorder> CreateDefaultBorder()
      const override;

  // views::WidgetObserver:
  void OnWidgetDestroying(views::Widget* widget) override;

  // brave_rewards::RewardsNotificationServiceObserver:
  void OnNotificationAdded(
      brave_rewards::RewardsNotificationService* service,
      const brave_rewards::RewardsNotificationService::RewardsNotification&
          notification) override;

  void OnNotificationDeleted(
      brave_rewards::RewardsNotificationService* service,
      const brave_rewards::RewardsNotificationService::RewardsNotification&
          notification) override;

 private:
  void OnPreferencesChanged(const std::string& key);
  void ToggleRewardsPanel();
  gfx::ImageSkia GetRewardsIcon();
  std::pair<std::string, SkColor> GetBadgeTextAndBackground();
  size_t GetRewardsNotificationCount();

  Profile* profile_ = nullptr;

  ToolbarActionView::Delegate* delegate_ = nullptr;

  PrefChangeRegistrar pref_change_registrar_;

  WebUIBubbleManagerT<RewardsPanelUI> bubble_manager_;

  std::pair<GURL, bool> publisher_verified_;

  std::unique_ptr<views::MenuButtonController::PressedLock> pressed_lock_;

  base::ScopedObservation<views::Widget, views::WidgetObserver>
      bubble_observation_{this};

  base::ScopedObservation<brave_rewards::RewardsNotificationService,
                          brave_rewards::RewardsNotificationServiceObserver>
      rewards_notification_observation_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_BRAVE_ACTIONS_BRAVE_REWARDS_PANEL_BUTTON_H_
