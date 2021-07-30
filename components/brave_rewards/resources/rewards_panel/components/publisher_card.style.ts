/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import styled from 'styled-components'

export const root = styled.div`
  background: var(--brave-palette-white);
  box-shadow: 0px 4px 16px rgba(27, 29, 47, 0.08);
  border-radius: 16px;
  margin-top: 16px;
  padding: 18px 35px 33px;

  a {
    color: var(--brave-color-brandBatInteracting);
    text-decoration: none;

    &:hover {
      text-decoration: underline;
    }
  }
`

export const heading = styled.div`
  display: flex;
  margin-bottom: 16px;
`

export const icon = styled.div`
  flex: 0 0 auto;

  img {
    height: 53px;
    width: auto;
  }
`

export const name = styled.div`
  flex: 1 1 auto;
  margin-top: 2px;
  margin-left: 7px;
  font-weight: 500;
  font-size: 18px;
  line-height: 22px;
  color: var(--brave-palette-neutral900);
`

export const status = styled.div`
  font-weight: normal;
  font-size: 12px;
  line-height: 14px;
  margin-top: 7px;
  color: var(--brave-palette-neutral700);

  .icon {
    height: 13px;
    width: auto;
    vertical-align: middle;
    margin-bottom: 2px;
    margin-right: 4px;
  }
`

export const refreshStatus = styled.span`
  display: inline-block;
  border-left: solid 1px var(--brave-palette-neutral200);
  padding-left: 4px;
  margin-left: 5px;
`

export const verified = styled.span`
  .icon {
    color: var(--brave-color-brandBatInteracting);
  }
`

export const unverified = styled.span`
  .icon {
    color: var(--brave-palette-grey200);
  }
`

export const unverifiedNote = styled.div`
  font-size: 12px;
  line-height: 20px;
  margin-bottom: 12px;
  color: var(--brave-palette-neutral700);
  background: var(--brave-palette-neutral000);
  border-radius: 8px;
  padding: 9px 11px;

  strong {
    font-weight: 600;
  }
`

export const attention = styled.div`
  border-bottom: solid 1px var(--brave-palette-neutral200);
  font-size: 14px;
  line-height: 26px;
  color: var(--brave-palette-neutral900);
  padding: 4px 0;
  display: flex;

  > * {
    flex: 1 1 auto;
  }

  .value {
    font-weight: 600;
    text-align: right;
  }
`

export const contribution = styled.div`
  border-bottom: solid 1px var(--brave-palette-neutral200);
  font-size: 14px;
  line-height: 26px;
  color: var(--brave-palette-neutral600);
  padding: 6px 0 12px 0;
`

export const autoContribution = styled.div`
  display: flex;

  > * {
    flex: 1 1 auto;
  }

  > :last-child {
    text-align: right;
  }
`

export const monthlyContribution = styled.div`
  display: flex;

  > * {
    flex: 1 1 auto;
  }

  > :last-child {
    text-align: right;
  }
`

export const tipAction = styled.div`
  text-align: center;
  margin-top: 30px;

  button {
    width: 100%;
    background: var(--brave-color-brandBatInteracting);
    color: var(--brave-palette-white);
    padding: 10px;
    margin: 0;
    border: none;
    border-radius: 48px;
    font-weight: 600;
    font-size: 13px;
    line-height: 20px;
    cursor: pointer;

    &:active {
      background: var(--brave-palette-blurple400);
    }
  }
`
