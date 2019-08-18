//
//  ConsentForm-ios.m
//
//  Created by seongmin hwang on 2018. 9. 26..
//

#include "ConsentForm.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import <Foundation/Foundation.h>
#import <UIKit/UIAlert.h>

#import "AppController.h"
#import "RootViewController.h"

#include "../platform/SBPlatformMacros.h"
#include "ConsentManager.hpp"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

static string globalTitle;
static string globalMessage;
static string globalPolicyUrl;

void ConsentForm::showImpl(const string &title, const string &message,
                           const string &policyUrl) {
    
    globalTitle = title;
    globalMessage = message;
    globalPolicyUrl = policyUrl;
    
    UIAlertController *alert = [UIAlertController
                                alertControllerWithTitle:NS_STRING(title.c_str())
                                message:NS_STRING(message.c_str())
                                preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *policyAction = [UIAlertAction actionWithTitle:@"Privacy Policy"
                                                           style:UIAlertActionStyleDefault
                                                         handler:^(UIAlertAction *action) {
                                                             ConsentManager::setPrivacyPolicyChecked(true);
                                                             Application::getInstance()->openURL(globalPolicyUrl);
                                                         }];
    
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction * action) {
                                                         ConsentManager::setPrivacyPolicyChecked(true);
                                                     }];
    [alert addAction:policyAction];
    [alert addAction:okAction];
    
    [ROOT_VIEW_CONTROLLER presentViewController:alert animated:YES completion:nil];
}

NS_SB_END;

#endif // CC_PLATFORM_IOS
