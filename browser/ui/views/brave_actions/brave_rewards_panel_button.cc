// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/brave_actions/brave_rewards_panel_button.h"

#include <memory>
#include <string>
#include <utility>

#include "base/strings/string_number_conversions.h"
#include "brave/app/vector_icons/vector_icons.h"
#include "brave/browser/brave_rewards/rewards_service_factory.h"
#include "brave/browser/ui/brave_actions/brave_action_icon_with_badge_image_source.h"
#include "brave/common/webui_url_constants.h"
#include "brave/components/brave_rewards/common/pref_names.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/toolbar/toolbar_action_view.h"
#include "chrome/browser/ui/views/toolbar/toolbar_ink_drop_util.h"
#include "components/grit/brave_components_strings.h"
#include "components/prefs/pref_service.h"
#include "ui/base/theme_provider.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/gfx/skia_util.h"
#include "ui/views/animation/ink_drop_impl.h"
#include "ui/views/controls/button/label_button_border.h"
#include "ui/views/controls/highlight_path_generator.h"
#include "ui/views/view_class_properties.h"

namespace {

using brave_rewards::RewardsNotificationService;
using brave_rewards::RewardsServiceFactory;

constexpr SkColor kBadgeTextColor = SK_ColorWHITE;
constexpr SkColor kBadgeNotificationBG = SkColorSetRGB(0xfb, 0x54, 0x2b);
constexpr SkColor kBadgeVerifiedBG = SkColorSetRGB(0x4c, 0x54, 0xd2);
const char kVerifiedCheck[] = "\u2713";

class ButtonHighlightPathGenerator : public views::HighlightPathGenerator {
 public:
  // views::HighlightPathGenerator:
  SkPath GetHighlightPath(const views::View* view) override {
    // Set the highlight path for the toolbar button, making it inset so that
    // the badge can show outside it in the fake margin on the right that we are
    // creating.
    DCHECK(view);
    gfx::Rect rect(view->GetPreferredSize());
    rect.Inset(gfx::Insets(0, 0, 0, kBraveActionRightMargin));

    auto* layout_provider = ChromeLayoutProvider::Get();
    DCHECK(layout_provider);

    int radius = layout_provider->GetCornerRadiusMetric(
        views::Emphasis::kMaximum, rect.size());

    SkPath path;
    path.addRoundRect(gfx::RectToSkRect(rect), radius, radius);
    return path;
  }
};

RewardsNotificationService* GetRewardsNotificationService(Profile* profile) {
  DCHECK(profile);
  auto* rewards_service = RewardsServiceFactory::GetForProfile(profile);
  return rewards_service ? rewards_service->GetNotificationService() : nullptr;
}

}  // namespace

BraveRewardsPanelButton::BraveRewardsPanelButton(
    Profile* profile,
    ToolbarActionView::Delegate* delegate)
    : profile_(profile),
      delegate_(delegate),
      bubble_manager_(this,
                      profile_,
                      GURL(kBraveRewardsPanelURL),
                      IDS_ACCNAME_BRAVE_REWARDS_BUBBLE) {
  SetButtonController(std::make_unique<views::MenuButtonController>(
      this,
      base::BindRepeating(&BraveRewardsPanelButton::ToggleRewardsPanel,
                          base::Unretained(this)),
      std::make_unique<views::Button::DefaultButtonControllerDelegate>(this)));

  views::HighlightPathGenerator::Install(
      this, std::make_unique<ButtonHighlightPathGenerator>());

  ink_drop()->SetMode(views::InkDropHost::InkDropMode::ON);
  ink_drop()->SetVisibleOpacity(kToolbarInkDropVisibleOpacity);
  ink_drop()->SetBaseColorCallback(base::BindRepeating(
      [](views::View* host) { return GetToolbarInkDropBaseColor(host); },
      this));

  SetHasInkDropActionOnClick(true);
  SetHorizontalAlignment(gfx::ALIGN_CENTER);

  pref_change_registrar_.Init(profile_->GetPrefs());
  pref_change_registrar_.Add(
      brave_rewards::prefs::kBadgeText,
      base::BindRepeating(&BraveRewardsPanelButton::OnPreferencesChanged,
                          base::Unretained(this)));

  rewards_notification_observation_.Observe(
      GetRewardsNotificationService(profile_));
}

BraveRewardsPanelButton::~BraveRewardsPanelButton() = default;

void BraveRewardsPanelButton::UpdateImageAndText() {
  auto badge_info = GetBadgeTextAndBackground();

  auto badge = std::make_unique<IconWithBadgeImageSource::Badge>(
      std::get<std::string>(badge_info), kBadgeTextColor,
      std::get<SkColor>(badge_info));

  gfx::Size size = GetPreferredSize();

  auto image_source =
      std::make_unique<BraveActionIconWithBadgeImageSource>(size);
  image_source->SetIcon(gfx::Image(GetRewardsIcon()));
  image_source->SetBadge(std::move(badge));

  gfx::ImageSkia icon_image(std::move(image_source), size);

  SetImage(views::Button::STATE_NORMAL, icon_image);
}

gfx::Size BraveRewardsPanelButton::CalculatePreferredSize() const {
  return delegate_->GetToolbarActionSize();
}

std::unique_ptr<views::LabelButtonBorder>
BraveRewardsPanelButton::CreateDefaultBorder() const {
  auto border = LabelButton::CreateDefaultBorder();
  border->set_insets(gfx::Insets(0, 0, 0, 0));
  return border;
}

void BraveRewardsPanelButton::OnWidgetDestroying(views::Widget* widget) {
  auto* bubble_widget = bubble_manager_.GetBubbleWidget();
  DCHECK_EQ(bubble_widget, widget);
  DCHECK(bubble_observation_.IsObservingSource(bubble_widget));

  bubble_observation_.Reset();
  pressed_lock_.reset();
}

void BraveRewardsPanelButton::OnNotificationAdded(
    RewardsNotificationService* service,
    const RewardsNotificationService::RewardsNotification& notification) {
  UpdateImageAndText();
}

void BraveRewardsPanelButton::OnNotificationDeleted(
    RewardsNotificationService* service,
    const RewardsNotificationService::RewardsNotification& notification) {
  UpdateImageAndText();
}

void BraveRewardsPanelButton::OnPreferencesChanged(const std::string& key) {
  UpdateImageAndText();
}

void BraveRewardsPanelButton::ToggleRewardsPanel() {
  if (bubble_manager_.GetBubbleWidget()) {
    bubble_manager_.CloseBubble();
    return;
  }

  // Clear the default-on-start badge text when the user opens the panel.
  profile_->GetPrefs()->SetString(brave_rewards::prefs::kBadgeText, "");

  bubble_manager_.ShowBubble();

  DCHECK(!bubble_observation_.IsObserving());
  bubble_observation_.Observe(bubble_manager_.GetBubbleWidget());

  auto* controller =
      static_cast<views::MenuButtonController*>(button_controller());
  pressed_lock_ = controller->TakeLock();
}

gfx::ImageSkia BraveRewardsPanelButton::GetRewardsIcon() {
  // Since the BAT icon has color the actual color value here is not relevant,
  // but |CreateVectorIcon| requires one.
  auto* theme_provider = GetThemeProvider();
  DCHECK(theme_provider);
  SkColor icon_color =
      theme_provider->GetColor(ThemeProperties::COLOR_TOOLBAR_BUTTON_ICON);

  return gfx::CreateVectorIcon(kBatIcon, kBraveActionGraphicSize, icon_color);
}

std::pair<std::string, SkColor>
BraveRewardsPanelButton::GetBadgeTextAndBackground() {
  // 1. Display the default-on-start Rewards badge text, if specified.
  std::string text_pref =
      profile_->GetPrefs()->GetString(brave_rewards::prefs::kBadgeText);
  if (!text_pref.empty())
    return {text_pref, kBadgeNotificationBG};

  // 2. Display the number of current notifications, if non-zero.
  size_t notification_count = GetRewardsNotificationCount();
  if (notification_count > 0) {
    std::string count_text = notification_count > 99
                                 ? "99+"
                                 : base::NumberToString(notification_count);

    return {count_text, kBadgeNotificationBG};
  }

  // 3. Display a verified checkmark for verified publishers.
  if (auto* web_contents = delegate_->GetCurrentWebContents()) {
    auto& url = web_contents->GetLastCommittedURL();
    if (url == std::get<GURL>(publisher_verified_)) {
      if (std::get<bool>(publisher_verified_))
        return {kVerifiedCheck, kBadgeVerifiedBG};
    } else {
      // TODO(zenparsing): Query the rewards service for verified status on
      // this URL. Unfortunately, the system doesn't really allow this right
      // now, especially for "media" publishers. For non-media publishers, we
      // just use the TLD+1 to get the publisher ID. For media publishers, we
      // rely on content-level scripts (or "legacy" code in bat-native-ledger)
      // to figure it out. Basically, we don't store the mapping from URL to
      // publisher ID anywhere. Instead, we store a mapping from tab ID to
      // publisher ID in the extension. No wonder this thing is so flaky!
    }
  }

  return {"", kBadgeNotificationBG};
}

size_t BraveRewardsPanelButton::GetRewardsNotificationCount() {
  auto* service = GetRewardsNotificationService(profile_);
  return service ? service->GetAllNotifications().size() : 0;
}
