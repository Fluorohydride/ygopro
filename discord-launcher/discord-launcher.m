#include <string.h>
#import <AppKit/AppKit.h>

int main(int argc, char* argv[]) {
    (void) argv;
    if(argc == 1) {
        const char* name = [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
        char* last = strrchr(name, '.');
        if(last == 0)
            return 0;
        *last = 0;
        char launch_argument[256];
        sprintf(launch_argument, "open -b %s --args from_discord", name);
        system(launch_argument);
    }
    return 0;
}
