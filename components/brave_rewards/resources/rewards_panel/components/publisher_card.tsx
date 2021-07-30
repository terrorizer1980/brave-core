/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import { LocaleContext, formatMessage } from '../../shared/lib/locale_context'
import { HostContext } from '../lib/host_context'
import { Toggle } from './toggle'
import { NewTabLink } from '../../shared/components/new_tab_link'
import { VerifiedIcon } from './icons/verified_icon'

import * as styles from './publisher_card.style'

const unverifiedLearnMoreURL = ''

export function PublisherCard () {
  const { getString } = React.useContext(LocaleContext)
  const host = React.useContext(HostContext)

  const [publisherInfo, setPublisherInfo] =
    React.useState(host.state.publisherInfo)

  React.useEffect(() => {
    return host.addListener((state) => {
      setPublisherInfo(state.publisherInfo)
    })
  }, [])

  if (!publisherInfo) {
    return null
  }

  function renderStatusMessage (): React.ReactNode {
    if (!publisherInfo) {
      return null
    }

    switch (publisherInfo.status) {
      case 'not-registered':
      case 'registered': // TODO(zenparsing): This case doesn't make sense
        return (
          <styles.unverified>
            <VerifiedIcon />{getString('unverifiedCreator')}
          </styles.unverified>
        )
      case 'verified':
        return (
          <styles.verified>
            <VerifiedIcon />{getString('verifiedCreator')}
          </styles.verified>
        )
    }
  }

  function renderUnverifiedNote (): React.ReactNode {
    if (!publisherInfo) {
      return null
    }

    if (publisherInfo.status === 'verified') {
      return null
    }

    return (
      <styles.unverifiedNote>
        <strong>{getString('note')}:</strong>&nbsp;
        {getString('unverifiedNote')}&nbsp;
        {
          formatMessage(getString('unverifiedLinks'), {
            tags: {
              $1: (content) =>
                <NewTabLink href={unverifiedLearnMoreURL} key='learn-more'>
                  {content}
                </NewTabLink>,
              $3: (content) =>
                <a href='#' key='hide' onClick={hideVerifiedNote}>
                  {content}
                </a>
            }
          })
        }
      </styles.unverifiedNote>
    )
  }

  function hideVerifiedNote (evt: React.UIEvent) {
    evt.preventDefault()
    // TODO(zenparsing): Send to host
  }

  function onRefreshClick (evt: React.UIEvent) {
    evt.preventDefault()
    host.refreshPublisherStatus()
  }

  function onAutoContributeToggle (enabled: boolean) {
    // TODO
  }

  return (
    <styles.root>
      <styles.heading>
        <styles.icon>
          <img src={publisherInfo.icon} />
        </styles.icon>
        <styles.name>
          {publisherInfo.name}
          <styles.status>
            {renderStatusMessage()}
            <styles.refreshStatus>
              <a href='#' onClick={onRefreshClick}>
                {getString('refreshStatus')}
              </a>
            </styles.refreshStatus>
          </styles.status>
        </styles.name>
      </styles.heading>
      {renderUnverifiedNote()}
      <styles.attention>
        <span>{getString('attention')}</span>
        <span className='value'>
          {(publisherInfo.attentionScore * 100).toFixed(0)}%
        </span>
      </styles.attention>
      <styles.contribution>
        <styles.autoContribution>
          <span>{getString('includeInAutoContribute')}</span>
          <span>
            <Toggle
              checked={publisherInfo.autoContributeEnabled}
              onChange={onAutoContributeToggle}
            />
          </span>
        </styles.autoContribution>
        <styles.monthlyContribution>
          <span>{getString('monthlyContribution')}</span>
          <span></span>
        </styles.monthlyContribution>
      </styles.contribution>
      <styles.tipAction>
        <button>
          {getString('sendTip')}
        </button>
      </styles.tipAction>
    </styles.root>
  )
}
