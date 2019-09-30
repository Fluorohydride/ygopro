#include "osx_menu.h"
#import <AppKit/AppKit.h>

void EDOPRO_SetupMenuBar() {
    @autoreleasepool {
        // Apparently in a newer version of Irrlicht's CIrrDeviceOSX.mm

        NSString* bundleName = @""; // Cannot actually set the main menu title at runtime
        NSMenu* systemMenuBar = [[[NSMenu alloc] initWithTitle:@"MainMenu"] autorelease];
        NSMenu* appMainMenu = [[[NSMenu alloc] initWithTitle:bundleName] autorelease];

        NSMenuItem* appMainMenuOpener = [systemMenuBar addItemWithTitle:bundleName action:nil keyEquivalent:@""];
        [systemMenuBar setSubmenu:appMainMenu forItem:appMainMenuOpener];

        NSMenuItem* newWindowItem = [appMainMenu addItemWithTitle:@"New Window" action:nil keyEquivalent:@"n"];
        [newWindowItem setKeyEquivalentModifierMask:NSCommandKeyMask];

        NSMenuItem* quitItem = [appMainMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
        [quitItem setKeyEquivalentModifierMask:NSCommandKeyMask];

        [NSApp setMainMenu:systemMenuBar];
    }
}
