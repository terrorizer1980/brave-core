/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/brave_actions/brave_actions_container.h"

#include <memory>
#include <string>
#include <utility>

#include "base/one_shot_event.h"
#include "brave/browser/profiles/profile_util.h"
#include "brave/browser/ui/brave_actions/brave_action_view_controller.h"
#include "brave/browser/ui/views/brave_actions/brave_action_view.h"
#include "brave/browser/ui/views/rounded_separator.h"
#include "brave/components/brave_rewards/common/pref_names.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/extensions/extensions_container.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/toolbar/toolbar_action_view_controller.h"
#include "chrome/browser/ui/toolbar/toolbar_actions_bar_bubble_delegate.h"
#include "chrome/browser/ui/views/toolbar/toolbar_action_view.h"
#include "components/prefs/pref_service.h"
#include "extensions/browser/extension_action_manager.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_registry_observer.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "ui/base/dragdrop/drag_drop_types.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"

namespace {

constexpr SkColor kSeparatorColor = SkColorSetRGB(0xb2, 0xb5, 0xb7);
constexpr int kSeparatorMargin = 3;
constexpr int kSeparatorWidth = 1;

}  // namespace

class BraveActionsExtensionsContainer : public ExtensionsContainer {
 public:
  BraveActionsExtensionsContainer() = default;
  virtual ~BraveActionsExtensionsContainer() = default;

  BraveActionsExtensionsContainer(const BraveActionsExtensionsContainer&) =
      delete;

  BraveActionsExtensionsContainer& operator=(
      const BraveActionsExtensionsContainer&) = delete;

  ToolbarActionViewController* GetActionForId(
      const std::string& action_id) override { return nullptr; }

  ToolbarActionViewController* GetPoppedOutAction() const override {
    return nullptr;
  }

  bool IsActionVisibleOnToolbar(
      const ToolbarActionViewController* action) const override {
    return false;
  }

  extensions::ExtensionContextMenuModel::ButtonVisibility GetActionVisibility(
      const ToolbarActionViewController* action) const override {
    return extensions::ExtensionContextMenuModel::PINNED;
  }

  void UndoPopOut() override {}

  void SetPopupOwner(ToolbarActionViewController* popup_owner) override {}

  void HideActivePopup() override {}

  bool CloseOverflowMenuIfOpen() override { return false; }

  void PopOutAction(ToolbarActionViewController* action,
                    bool is_sticky,
                    base::OnceClosure closure) override {}

  bool ShowToolbarActionPopupForAPICall(const std::string& action_id) override {
    return false;
  }

  void ShowToolbarActionBubble(
      std::unique_ptr<ToolbarActionsBarBubbleDelegate> bubble) override {}

  void ShowToolbarActionBubbleAsync(
      std::unique_ptr<ToolbarActionsBarBubbleDelegate> bubble) override {}

  void ToggleExtensionsMenu() override {}

  bool HasAnyExtensions() const override { return false; }
};

BraveActionsContainer::BraveActionsContainer(Browser* browser)
    : browser_(browser),
      extensions_container_(new BraveActionsExtensionsContainer()) {
  DCHECK(browser_);

  auto layout = std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kHorizontal);
  layout->set_main_axis_alignment(views::BoxLayout::MainAxisAlignment::kCenter);
  layout->set_cross_axis_alignment(
      views::BoxLayout::CrossAxisAlignment::kCenter);
  SetLayoutManager(std::move(layout));

  auto separator = std::make_unique<RoundedSeparator>();
  separator->SetColor(kSeparatorColor);
  separator->SetPreferredSize(
      gfx::Size(kSeparatorWidth + kSeparatorMargin * 2,
                GetLayoutConstant(LOCATION_BAR_ICON_SIZE)));
  separator->SetBorder(
      views::CreateEmptyBorder(0, kSeparatorMargin, 0, kSeparatorMargin));

  AddChildViewAt(std::move(separator), 0);

  pref_change_registrar_.Init(profile()->GetPrefs());
  pref_change_registrar_.Add(
      brave_rewards::prefs::kHideButton,
      base::BindRepeating(&BraveActionsContainer::OnPreferencesChanged,
                          base::Unretained(this)));

  extensions::ExtensionSystem::Get(profile())->ready().Post(
      FROM_HERE, base::BindOnce(&BraveActionsContainer::OnExtensionSystemReady,
                                weak_ptr_factory_.GetWeakPtr()));
}

BraveActionsContainer::~BraveActionsContainer() = default;

void BraveActionsContainer::Update() {
  if (shields_controller_)
    shields_controller_->UpdateState();

  if (rewards_panel_button_)
    rewards_panel_button_->UpdateImageAndText();

  // Only show the separator if we're showing at least one button.
  DCHECK_GT(children().size(), 0ul);
  auto* separator = children()[0];
  separator->SetVisible(children().size() > 1);

  Layout();
}

void BraveActionsContainer::ChildPreferredSizeChanged(views::View* child) {
  PreferredSizeChanged();
}

gfx::Size BraveActionsContainer::GetToolbarActionSize() {
  // Width > Height to give space for a large bubble (especially for shields).
  // TODO(petemill): Generate based on toolbar size.
  return gfx::Size(34, 24);
}

content::WebContents* BraveActionsContainer::GetCurrentWebContents() {
  return browser_->tab_strip_model()->GetActiveWebContents();
}

views::LabelButton* BraveActionsContainer::GetOverflowReferenceView() const {
  // Our action views should always be visible; we should not need another view.
  NOTREACHED();
  return nullptr;
}

void BraveActionsContainer::WriteDragDataForView(View* sender,
                                                   const gfx::Point& press_pt,
                                                   OSExchangeData* data) {
  // Not supporting drag for action buttons inside this container
}

int BraveActionsContainer::GetDragOperationsForView(View* sender,
                                                      const gfx::Point& p) {
  return ui::DragDropTypes::DRAG_NONE;
}

bool BraveActionsContainer::CanStartDragForView(View* sender,
                                                  const gfx::Point& press_pt,
                                                  const gfx::Point& p) {
  return false;
}

void BraveActionsContainer::OnExtensionLoaded(
    content::BrowserContext* browser_context,
    const extensions::Extension* extension) {
  DCHECK(extension);
  if (extension->id() == brave_extension_id)
    MaybeAddBraveExtensionAction();
}

void BraveActionsContainer::OnExtensionUnloaded(
    content::BrowserContext* browser_context,
    const extensions::Extension* extension,
    extensions::UnloadedExtensionReason reason) {
  DCHECK(extension);
  if (extension->id() == brave_extension_id) {
    shields_button_.reset();
    shields_controller_.reset();
    Update();
    PreferredSizeChanged();
  }
}

void BraveActionsContainer::OnExtensionActionUpdated(
    extensions::ExtensionAction* extension_action,
    content::WebContents* web_contents,
    content::BrowserContext* browser_context) {
  DCHECK(extension_action);
  auto& id = extension_action->extension_id();
  if (auto* controller = GetExtensionViewController(id)) {
    controller->UpdateState();
  }
}

void BraveActionsContainer::OnBraveActionShouldTrigger(
    const std::string& extension_id,
    std::unique_ptr<std::string> ui_relative_path) {
  if (auto* controller = GetExtensionViewController(extension_id)) {
    if (ui_relative_path) {
      controller->ExecuteActionUI(*ui_relative_path);
    } else {
      controller->ExecuteAction(
          true, ToolbarActionViewController::InvocationSource::kApi);
    }
  }
}

BraveActionViewController* BraveActionsContainer::GetExtensionViewController(
    const std::string& extension_id) {
  if (extension_id == brave_extension_id)
    return shields_controller_.get();

  return nullptr;
}

bool BraveActionsContainer::ShouldShowRewardsPanelButton() {
  if (!brave::IsRegularProfile(profile()))
    return false;

  if (profile()->GetPrefs()->GetBoolean(brave_rewards::prefs::kHideButton))
    return false;

  return true;
}

void BraveActionsContainer::MaybeAddRewardsPanelButton() {
  if (!ShouldShowRewardsPanelButton() || rewards_panel_button_)
    return;

  rewards_panel_button_.reset(new BraveRewardsPanelButton(profile(), this));
  rewards_panel_button_->set_owned_by_client();
  AddChildView(rewards_panel_button_.get());
  Update();
  PreferredSizeChanged();
}

void BraveActionsContainer::MaybeAddBraveExtensionAction() {
  if (shields_button_)
    return;

  auto* registry = extensions::ExtensionRegistry::Get(profile());
  auto* extension = registry->enabled_extensions().GetByID(brave_extension_id);

  if (!extension)
    return;

  shields_controller_ = BraveActionViewController::Create(
      brave_extension_id, browser_, extensions_container_.get());

  shields_button_.reset(new BraveActionView(shields_controller_.get(), this));
  shields_button_->set_owned_by_client();

  AddChildViewAt(shields_button_.get(), 1);
  Update();
  PreferredSizeChanged();
}

void BraveActionsContainer::OnExtensionSystemReady() {
  extension_registry_observer_.Observe(
      extensions::ExtensionRegistry::Get(profile()));

  extension_action_observer_.Observe(
      extensions::ExtensionActionAPI::Get(profile()));

  brave_action_observer_.Observe(extensions::BraveActionAPI::Get(browser_));

  MaybeAddBraveExtensionAction();
  MaybeAddRewardsPanelButton();
}

void BraveActionsContainer::OnPreferencesChanged(const std::string& key) {
  if (ShouldShowRewardsPanelButton()) {
    MaybeAddRewardsPanelButton();
  } else {
    rewards_panel_button_.reset();
  }
}
