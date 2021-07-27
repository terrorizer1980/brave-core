/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_BRAVE_ACTIONS_BRAVE_ACTIONS_CONTAINER_H_
#define BRAVE_BROWSER_UI_VIEWS_BRAVE_ACTIONS_BRAVE_ACTIONS_CONTAINER_H_

#include <memory>
#include <string>

#include "base/scoped_observation.h"
#include "brave/browser/extensions/api/brave_action_api.h"
#include "brave/browser/ui/views/brave_actions/brave_rewards_panel_button.h"
#include "chrome/browser/extensions/api/extension_action/extension_action_api.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/toolbar/toolbar_action_view.h"
#include "components/prefs/pref_change_registrar.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_registry_observer.h"
#include "extensions/common/extension.h"
#include "ui/views/view.h"

class BraveActionViewController;

namespace extensions {
class ExtensionActionManager;
}

namespace views {
class Button;
}

class BraveActionsExtensionsContainer;

// This View contains all the built-in BraveActions such as Shields and Rewards.
// TODO(petemill): consider splitting to separate model, like
// |ToolbarActionsModel|.
class BraveActionsContainer : public views::View,
                              public extensions::BraveActionAPI::Observer,
                              public extensions::ExtensionActionAPI::Observer,
                              public extensions::ExtensionRegistryObserver,
                              public ToolbarActionView::Delegate {
 public:
  explicit BraveActionsContainer(Browser* browser);
  ~BraveActionsContainer() override;

  BraveActionsContainer(const BraveActionsContainer&) = delete;
  BraveActionsContainer& operator=(const BraveActionsContainer&) = delete;

  void Update();

  // views::View:
  void ChildPreferredSizeChanged(views::View* child) override;

  // ToolbarActionView::Delegate
  gfx::Size GetToolbarActionSize() override;

  content::WebContents* GetCurrentWebContents() override;

  views::LabelButton* GetOverflowReferenceView() const override;

  // views::DragController:
  void WriteDragDataForView(View* sender,
                            const gfx::Point& press_pt,
                            ui::OSExchangeData* data) override;

  int GetDragOperationsForView(View* sender, const gfx::Point& p) override;

  bool CanStartDragForView(View* sender,
                           const gfx::Point& press_pt,
                           const gfx::Point& p) override;

  // extensions::ExtensionRegistryObserver:
  void OnExtensionLoaded(content::BrowserContext* browser_context,
                         const extensions::Extension* extension) override;

  void OnExtensionUnloaded(content::BrowserContext* browser_context,
                           const extensions::Extension* extension,
                           extensions::UnloadedExtensionReason reason) override;

  // extensions::ExtensionActionAPI::Observer:
  void OnExtensionActionUpdated(
      extensions::ExtensionAction* extension_action,
      content::WebContents* web_contents,
      content::BrowserContext* browser_context) override;

  // extensions::BraveActionAPI::Observer
  void OnBraveActionShouldTrigger(
      const std::string& extension_id,
      std::unique_ptr<std::string> ui_relative_path) override;

 private:
  Profile* profile() const { return browser_->profile(); }

  BraveActionViewController* GetExtensionViewController(
      const std::string& extension_id);

  bool ShouldShowRewardsPanelButton();

  void MaybeAddRewardsPanelButton();

  void MaybeAddBraveExtensionAction();

  void OnExtensionSystemReady();

  void OnPreferencesChanged(const std::string& key);

  Browser* browser_ = nullptr;

  // Observer for profile preference changes
  PrefChangeRegistrar pref_change_registrar_;

  // A no-op |ExtensionContainer| provided to |BraveActionViewController|.
  std::unique_ptr<BraveActionsExtensionsContainer> extensions_container_;

  // The Brave shields extension action button controller.
  std::unique_ptr<BraveActionViewController> shields_controller_;

  // The Brave shields extension action button.
  std::unique_ptr<views::Button> shields_button_;

  // The Brave Rewards panel button.
  std::unique_ptr<BraveRewardsPanelButton> rewards_panel_button_;

  // Observer for extension load and unload events.
  base::ScopedObservation<extensions::ExtensionRegistry,
                          extensions::ExtensionRegistryObserver>
      extension_registry_observer_{this};

  // Observer for extension action updates.
  base::ScopedObservation<extensions::ExtensionActionAPI,
                          extensions::ExtensionActionAPI::Observer>
      extension_action_observer_{this};

  // Observer for requests to open an extension popup.
  base::ScopedObservation<extensions::BraveActionAPI,
                          extensions::BraveActionAPI::Observer>
      brave_action_observer_{this};

  base::WeakPtrFactory<BraveActionsContainer> weak_ptr_factory_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_BRAVE_ACTIONS_BRAVE_ACTIONS_CONTAINER_H_
