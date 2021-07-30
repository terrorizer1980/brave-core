/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import { Host, HostState } from '../lib/interfaces'
import { localeStrings } from './locale_strings'

import { App } from '../components/app'

export default {
  title: 'Rewards'
}

function createHost (): Host {
  const state: HostState = {
    balance: 10.2,
    exchangeInfo: {
      rate: 0.75,
      currency: 'USD'
    },
    earningsInfo: {
      earningsThisMonth: 1.2,
      earningsLastMonth: 2.4,
      nextPaymentDate: new Date(Date.now() + 1000 * 60 * 60 * 24 * 3)
    },
    publisherInfo: {
      name: 'brave.com',
      icon: 'https://brave.com/static-assets/images/brave-favicon.png',
      status: 'registered',
      attentionScore: 0.17,
      autoContributeEnabled: true,
      monthlyContribution: 5
    },
    externalWallet: {
      provider: 'uphold',
      username: 'brave123',
      status: 'verified'
    },
    summaryData: {
      grantClaims: 10,
      adEarnings: 10,
      autoContributions: 10,
      oneTimeTips: -2,
      monthlyTips: -19
    }
  }

  return {
    state,

    addListener (callback) {
      Promise.resolve().then(() => callback(state))
      return () => {
        // No-op
      }
    },

    getString (key: string) {
      return localeStrings[key] || 'MISSING'
    },

    refreshPublisherStatus () {
      console.log('refreshPublisherStatus')
    },

    openRewardsSettings () {
      console.log('openRewardsSettings')
    }
  }
}

export function Panel () {
  return (
    <App host={createHost()} />
  )
}
