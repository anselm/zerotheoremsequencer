
#import <Foundation/Foundation.h>

@interface ServerBrowser : NSObject {
  NSNetServiceBrowser* netServiceBrowser;
  NSMutableArray* servers;
}

@property(nonatomic,readonly) NSArray* servers;
- (BOOL)start;
- (void)stop;
@end
