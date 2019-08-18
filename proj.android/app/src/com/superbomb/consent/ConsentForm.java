package com.superbomb.consent;

import android.app.Activity;
import android.content.DialogInterface;

import androidx.appcompat.app.AlertDialog;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

public class ConsentForm {

    private static String title;
    private static String message;
    private static String policyUrl;

    public static void show(final String title, final String message, final String policyUrl) {

        ConsentForm.title = title;
        ConsentForm.message = message;
        ConsentForm.policyUrl = policyUrl;

        final Activity context = (Activity) Cocos2dxActivity.getContext();

        context.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                new AlertDialog.Builder(context)
                        .setCancelable(false)
                        .setTitle(title)
                        .setMessage(message)
                        .setNegativeButton("Privacy Policy", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                Cocos2dxHelper.openURL(ConsentForm.policyUrl);
                                nativeOnClickPrivacyPolicy();
                            }
                        })
                        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                nativeOnClickOk();
                            }
                        })
                        .show();
            }
        });
    }

    public static native void nativeOnClickPrivacyPolicy();
    public static native void nativeOnClickOk();
}
