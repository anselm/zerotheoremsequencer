
#import "ServerBrowser.h"

@implementation ServerBrowser

@synthesize servers;

- (id)init {
  servers = [[NSMutableArray alloc] init];
  return self;
}

- (void)dealloc {
  if ( servers != nil ) {
    [servers release];
    servers = nil;
  }
  [super dealloc];
}

- (BOOL)start {
    if ( netServiceBrowser != nil ) {
        [self stop];
    }
    
	netServiceBrowser = [[NSNetServiceBrowser alloc] init];
	if( !netServiceBrowser ) {
		return NO;
	}
    
	[netServiceBrowser searchForServicesOfType:@"_zerotheorem._tcp." inDomain:@""];
    
    return YES;
}


- (void)stop {
    if ( netServiceBrowser == nil ) {
        return;
    }
    
    [netServiceBrowser stop];
    [netServiceBrowser release];
    netServiceBrowser = nil;
    
    [servers removeAllObjects];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didFindService:(NSNetService *)netService moreComing:(BOOL)moreServicesComing {
    
    if ( ! [servers containsObject:netService] ) {
        [servers addObject:netService];
        NSLog(@"browser found service named %@",[netService name]);
    }
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didRemoveService:(NSNetService *)netService moreComing:(BOOL)moreServicesComing {
    [servers removeObject:netService];
}

@end
