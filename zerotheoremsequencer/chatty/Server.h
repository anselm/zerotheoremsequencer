
#import "Connection.h"

@interface Server : NSObject<ConnectionDelegate> {
    uint16_t port;
    CFSocketRef listeningSocket;
    NSNetService* netService;
    @public NSMutableSet* clients;
}

- (BOOL)start;
- (void)stop;
- (void)broadcastMessage:(NSDictionary*)packet;
- (void)narrowcastMessage:(NSDictionary*)packet toName:(NSString*)name;

@end
