/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

 import {Router} from '../router.js';

(function() {
'use strict';

/**
 * 'settings-brave-default-extensions-page' is the settings page containing
 * brave's default extensions.
 */
Polymer({
  is: 'settings-brave-wallet-page',

  behaviors: [
    WebUIListenerBehavior,
  ],

  properties: {
    isNativeWalletEnabled_: Boolean
  },

  /** @private {?settings.BraveWalletBrowserProxy} */
  browserProxy_: null,

  /** @override */
  created: function() {
    this.browserProxy_ = settings.BraveWalletBrowserProxyImpl.getInstance();
  },

  /** @override */
  ready: function() {
    this.onBraveWalletEnabledChange_ = this.onBraveWalletEnabledChange_.bind(this)
    this.browserProxy_.getWeb3ProviderList().then(list => {
      this.braveWeb3Providers_ = JSON.parse(list)
    });
    this.browserProxy_.isNativeWalletEnabled().then(val => {
      this.isNativeWalletEnabled_ = val;
    });
    this.onWalletNetworksEditorClick_ = this.onWalletNetworksEditorClick_.bind(this)
  },

  onBraveWalletEnabledChange_: function() {
    this.browserProxy_.setBraveWalletEnabled(this.$.braveWalletEnabled.checked);
  },

  onWalletNetworksEditorClick_: function() {
    const router = Router.getInstance();
    router.navigateTo(router.getRoutes().BRAVE_WALLET_NETWORKS);
  },

});
})();
