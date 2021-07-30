/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { ExternalWallet } from '../../shared/lib/external_wallet'
import { RewardsSummaryData } from '../../shared/components/wallet_card'

interface ExchangeInfo {
  currency: string
  rate: number
}

interface EarningsInfo {
  nextPaymentDate: Date
  earningsThisMonth: number
  earningsLastMonth: number
}

type PublisherStatus = 'not-registered' | 'registered' | 'verified'

interface PublisherInfo {
  name: string
  icon: string
  status: PublisherStatus
  attentionScore: number
  autoContributeEnabled: boolean
  monthlyContribution: number
}

export interface HostState {
  balance: number
  exchangeInfo: ExchangeInfo
  earningsInfo: EarningsInfo
  publisherInfo: PublisherInfo | null
  externalWallet: ExternalWallet | null
  summaryData: RewardsSummaryData
}

export type HostListener = (state: HostState) => void

export interface Host {
  state: HostState
  addListener: (callback: HostListener) => () => void
  getString: (key: string) => string
  openRewardsSettings: () => void
  refreshPublisherStatus: () => void
}
