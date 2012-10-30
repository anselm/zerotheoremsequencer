

#import "Server.h"
#import "ServerBrowser.h"

extern "C" {

    Server* server = 0;
    ServerBrowser* serverBrowser = 0;

    void serverStart() {
        server = [[Server alloc] init];
        [server start];
    }

    void serverStop() {
        if(server) {
            [server stop];
            [server release];
        }
        server = 0;
    }
    
    void serverMessage(const char* message) {
        if(server) {
            NSString* msg = [NSString stringWithUTF8String:message];
            [server broadcastMessage:msg];
        }
    }
    
    void browserStart() {
        serverBrowser = [[ServerBrowser alloc] init];
        [serverBrowser start];
    }

    void browserStop() {
        if(serverBrowser) {
            [serverBrowser stop];
            [serverBrowser release];
        }
        serverBrowser = 0;
    }
    
}


