/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser.onboarding;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Build;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.PopupWindow;
import org.chromium.base.SysUtils;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.BraveRewardsHelper;
import org.chromium.chrome.browser.ChromeTabbedActivity;


public class BraveTalkOptInPopup {
    private final View mAnchor;
    private final BraveTalkOptInPopupListener mListener;
    private final LayoutInflater mInflater;
    private final ChromeTabbedActivity mActivity;
    private final PopupWindow mWindow;

    public BraveTalkOptInPopup(View anchor,
                               BraveTalkOptInPopupListener listener,
                               boolean showRewardsOptIn) {
        mAnchor = anchor;
        mListener = listener;
        mInflater = (LayoutInflater) mAnchor.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mActivity = BraveRewardsHelper.getChromeTabbedActivity();
        mWindow = createPopupWindow();

        initPopupContent();
    }

    public void showLikePopDownMenu() {
        mWindow.setAnimationStyle(R.style.OverflowMenuAnim);

        if (SysUtils.isLowEndDevice()) {
            mWindow.setAnimationStyle(0);
        }

        mWindow.showAsDropDown(mAnchor, /* xOffset */ 0, /* yOffset */ 0);
    }

    private PopupWindow createPopupWindow() {
        PopupWindow window = new PopupWindow(mAnchor.getContext());
        window.setWidth(ViewGroup.LayoutParams.WRAP_CONTENT);
        window.setHeight(ViewGroup.LayoutParams.WRAP_CONTENT);
        window.setBackgroundDrawable(new ColorDrawable(Color.WHITE));
        window.setTouchable(true);
        window.setFocusable(true);
        window.setOutsideTouchable(true);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            window.setElevation(20);
        }

        window.setTouchInterceptor(new View.OnTouchListener() {
            @SuppressLint("ClickableViewAccessibility")
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_OUTSIDE) {
                    dismissPopup();
                    return true;
                }
                return false;
            }
        });
        window.setOnDismissListener(new PopupWindow.OnDismissListener() {
            @Override
            public void onDismiss() {
                mListener.notifyTalkOptInPopupClosed();
            }
        });

        return window;
    }

    private void initPopupContent() {
        View view = mInflater.inflate(R.layout.brave_talk_opt_in_layout, null);
        mWindow.setContentView(view);

        Button btnRewardsOn = view.findViewById(R.id.brave_talk_opt_in_button);
        btnRewardsOn.setOnClickListener((new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mListener.notifyAdsEnableButtonPressed();
                changeContentToStartFreeCall();
            }
        }));
    }

    private void changeContentToStartFreeCall() {
        View view = mInflater.inflate(R.layout.brave_talk_opt_in_layout, null);
        mWindow.setContentView(view);
    }

    private void dismissPopup() {
        mWindow.dismiss();
    }
}
