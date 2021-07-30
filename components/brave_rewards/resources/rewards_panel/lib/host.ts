/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { loadTimeData } from '../../../../common/loadTimeData'

import { Host } from './interfaces'

export function createHost (): Host {
  document.addEventListener('visibilitychange', () => {
    if (document.visibilityState === 'visible') {
      // The WebUI bubble manager can cause the visibility to transition from
      // "visible" to "hidden" immediately (within the same turn of the event
      // loop) as the bubble is added to the view heirarchy and then hidden.
      chrome.send('showPanel')
    }
  })

  chrome.send('pageReady')

  return {
    getString (key: string) {
      return loadTimeData.getString(key)
    }
  }
}
