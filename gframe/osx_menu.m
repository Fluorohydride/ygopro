#include "osx_menu.h"
#import <AppKit/AppKit.h>

void (*toggleFullScreenCallback)(void) = NULL;

@interface EdoproHandler : NSObject
-(void)spawn;
-(void)toggle;
@end

@implementation EdoproHandler
-(void)spawn {
	const char* abspath = [[[NSBundle mainBundle] bundlePath] UTF8String];
	char command[256] = "open -n ";
	strcat(command, abspath);
	system(strcat(command, " --args -m"));
}

-(void)toggle {
	[NSApp activateIgnoringOtherApps:YES];
	[[NSApp mainWindow] toggleFullScreen:nil];
	toggleFullScreenCallback();
}
@end

EdoproHandler* handler;

void EDOPRO_SetupMenuBar(void (*callback)(void)) {
	toggleFullScreenCallback = callback;
	@autoreleasepool {
		// Apparently in a newer version of Irrlicht's CIrrDeviceOSX.mm

		NSString* bundleName = @""; // Cannot actually set the main menu title at runtime
		NSMenu* systemMenuBar = [[[NSMenu alloc] initWithTitle:@"MainMenu"] autorelease];
		NSMenu* appMainMenu = [[[NSMenu alloc] initWithTitle:bundleName] autorelease];
		NSMenu* dockMenu = [[NSApp delegate] applicationDockMenu:NSApp];
		handler = [[EdoproHandler alloc] init];

		NSMenuItem* appMainMenuOpener = [systemMenuBar addItemWithTitle:bundleName action:nil keyEquivalent:@""];
		[systemMenuBar setSubmenu:appMainMenu forItem:appMainMenuOpener];

		NSMenuItem* newWindowItem = [appMainMenu addItemWithTitle:@"New Window" action:@selector(spawn) keyEquivalent:@"n"];
		[newWindowItem setTarget:handler];
		[newWindowItem setKeyEquivalentModifierMask:NSCommandKeyMask];
		[dockMenu addItem:[newWindowItem copyWithZone:nil]];

		NSMenuItem* fullScreenItem = [appMainMenu addItemWithTitle:@"Toggle Full Screen" action:@selector(toggle) keyEquivalent:@"f"];
		[fullScreenItem setTarget:handler];
		[fullScreenItem setKeyEquivalentModifierMask:NSControlKeyMask+NSCommandKeyMask];
		[dockMenu addItem:[fullScreenItem copyWithZone:nil]];

		NSMenuItem* quitItem = [appMainMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
		[quitItem setKeyEquivalentModifierMask:NSCommandKeyMask];

		[NSApp setMainMenu:systemMenuBar];
	}
}

void EDOPRO_ToggleFullScreen() {
	[handler toggle];
}
