/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import styled from 'styled-components'

export const root = styled.div`
  --toggle-handle-size: 16px;

  button {
    position: relative;
    height: var(--toggle-handle-size);
    width: 28px;
    border: none;
    background: none;
    margin: 0;
    padding: 0;
    cursor: pointer;
  }
`

export const bar = styled.div`
  background: var(--brave-palette-neutral500);
  height: 6px;
  width: 100%;
  margin: 5px 0;
  border-radius: 3px;
`

export const handle = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  right: initial;
  background-color: var(--brave-palette-neutral400);
  height: var(--toggle-handle-size);
  width: var(--toggle-handle-size);
  box-shadow: 0 3px 3px rgba(0, 0, 0, .05);
  border-radius: 50%;
  transition: all .3s ease;

  &.checked {
    right: 0;
    left: calc(100% - var(--toggle-handle-size));
    background-color: var(--brave-palette-blurple400);
  }
`
