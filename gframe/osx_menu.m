#include "osx_menu.h"
#import <AppKit/AppKit.h>

@interface NewAppInstanceHandler : NSObject
-(void)spawn;
@end

@implementation NewAppInstanceHandler
-(void)spawn {
    const char* abspath = [[[NSBundle mainBundle] bundlePath] UTF8String];
    char command[256] = "open -n ";
    system(strcat(command, abspath));
}
@end

void EDOPRO_SetupMenuBar() {
    @autoreleasepool {
        // Apparently in a newer version of Irrlicht's CIrrDeviceOSX.mm

        NSString* bundleName = @""; // Cannot actually set the main menu title at runtime
        NSMenu* systemMenuBar = [[[NSMenu alloc] initWithTitle:@"MainMenu"] autorelease];
        NSMenu* appMainMenu = [[[NSMenu alloc] initWithTitle:bundleName] autorelease];

        NSMenuItem* appMainMenuOpener = [systemMenuBar addItemWithTitle:bundleName action:nil keyEquivalent:@""];
        [systemMenuBar setSubmenu:appMainMenu forItem:appMainMenuOpener];

        NSMenuItem* newWindowItem = [appMainMenu addItemWithTitle:@"New Window" action:@selector(spawn) keyEquivalent:@"n"];
        NewAppInstanceHandler* handler = [[NewAppInstanceHandler alloc] init];
        [newWindowItem setTarget:handler];
        [newWindowItem setKeyEquivalentModifierMask:NSCommandKeyMask];

        NSMenuItem* quitItem = [appMainMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
        [quitItem setKeyEquivalentModifierMask:NSCommandKeyMask];

        [NSApp setMainMenu:systemMenuBar];
    }
}
