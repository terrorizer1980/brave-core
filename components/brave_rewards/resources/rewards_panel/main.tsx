/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import * as ReactDOM from 'react-dom'

import { LocaleContext } from '../shared/lib/locale_context'

import { createHost } from './lib/host'
import { HostContext } from './lib/host_context'
import { App } from './components/app'

document.addEventListener('DOMContentLoaded', () => {

  document.addEventListener('visibilitychange', () => {
    if (document.visibilityState === 'visible') {
      // The WebUI bubble manager can cause the visibility to transition from
      // "visible" to "hidden" immediately (within the same turn of the event
      // loop) as the bubble is added to the view heirarchy and then hidden.
      chrome.send('showPanel')
    }
  })

  const host = createHost()

  function Root () {
    return (
      <HostContext.Provider value={host}>
        <LocaleContext.Provider value={host}>
          <App />
        </LocaleContext.Provider>
      </HostContext.Provider>
    )
  }

  ReactDOM.render(<Root />, document.getElementById('root'))

  chrome.send('pageReady')
})
