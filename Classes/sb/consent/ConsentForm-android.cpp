//
//  ConsentForm.cpp
//
//  Created by seongmin hwang on 2018. 9. 26..
//

#include "ConsentForm.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"
#include "ConsentManager.hpp"

#define JNI_CLASS_NAME     "com/superbomb/consent/ConsentForm"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

void ConsentForm::showImpl(const string &title, const string &message,
                           const string &policyUrl) {
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "show", title, message, policyUrl);
}

extern "C" {
    void Java_com_superbomb_consent_ConsentForm_nativeOnClickPrivacyPolicy(JNIEnv *env, jobject obj) {
        ConsentManager::setPrivacyPolicyChecked(true);
    }
    
    void Java_com_superbomb_consent_ConsentForm_nativeOnClickOk(JNIEnv *env, jobject obj) {
        ConsentManager::setPrivacyPolicyChecked(true);
    }
}

NS_SB_END;

#endif // CC_PLATFORM_ANDROID
