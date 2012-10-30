
#import <Foundation/Foundation.h>
#import "Connection.h"

@interface Server : NSObject<ConnectionDelegate> {
    uint16_t port;
    CFSocketRef listeningSocket;
    NSNetService* netService;
    NSMutableSet* clients;
}

- (BOOL)start;
- (void)stop;
- (void)broadcastMessage:(NSString*)message;

@end
