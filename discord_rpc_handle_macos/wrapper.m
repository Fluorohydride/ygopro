#include <stdlib.h>
#import <AppKit/AppKit.h>

int main(int argc, char* argv[]) {
	if(argc == 1) {
		char* name = [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
		int pos = strsrch(name, '.', -1);
		if(pos != -1)
			return 0;
		name[pos] = 0;
		char launch_argument[256];
		sprintf("open -b %s --args from_discord", name);
		system(launch_argument);
	}
	return 0;
}