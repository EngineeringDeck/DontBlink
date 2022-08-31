#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "platform-macos.h"

@interface Observer : NSObject { }

- (void) ForegroundWindowChanged:(NSNotification *) notification;
@end

@implementation Observer
{
	@public PlatformMacOS *platform;
}

- (void) ForegroundWindowChanged:(NSNotification *) notification
{
	NSString *name=[[[notification userInfo] objectForKey:NSWorkspaceApplicationKey] localizedName];
	emit platform->ForegroundWindowChanged(QString::fromNSString(name));
}
@end

PlatformMacOS::PlatformMacOS()
{
	Observer *observer=[[Observer alloc] init];
	observer->platform=this;
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:observer selector:@selector(ForegroundWindowChanged:) name:NSWorkspaceDidActivateApplicationNotification object:nil];
}

PlatformMacOS::~PlatformMacOS()
{

}

void PlatformMacOS::UpdateAvailableWindows()
{
	std::unordered_set<QString> titles;

	NSArray *windows=(NSArray *)CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly,kCGNullWindowID);
	for (NSDictionary *window in windows)
	{
		titles.insert(QString::fromNSString(window[(NSString*)kCGWindowOwnerName]));
	}
	[windows autorelease];

	emit AvailableWindowsUpdated(titles);
}
