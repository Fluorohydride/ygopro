#include <stdio.h>
#include <sys/stat.h>

#import <AppKit/AppKit.h>

#include "discord_register.h"

void RegisterURL(const char* applicationId)
{
	@autoreleasepool {
		char url[256];
		snprintf(url, sizeof(url), "discord-%s", applicationId);
		CFStringRef cfURL = CFStringCreateWithCString(NULL, url, kCFStringEncodingUTF8);

		NSString* _myBundleId = [[NSBundle mainBundle] bundleIdentifier];
		if (!_myBundleId) {
			fprintf(stderr, "No bundle id found\n");
			return;
		}

		NSString* myBundleId = [NSString stringWithFormat:@"%@.discord", _myBundleId];

		NSURL* myURL = [[NSBundle mainBundle] bundleURL];
		if (!myURL) {
			fprintf(stderr, "No bundle url found\n");
			return;
		}

		OSStatus status = LSSetDefaultHandlerForURLScheme(cfURL, (__bridge CFStringRef)myBundleId);
		if (status != noErr) {
			fprintf(stderr, "Error in LSSetDefaultHandlerForURLScheme: %d\n", (int)status);
			return;
		}

		status = LSRegisterURL((__bridge CFURLRef)myURL, true);
		if (status != noErr) {
			fprintf(stderr, "Error in LSRegisterURL: %d\n", (int)status);
		}
	}
}
