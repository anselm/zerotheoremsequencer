

#import "Server.h"
#import "ServerBrowser.h"

extern "C" {

    Server* server = 0;
    ServerBrowser* serverBrowser = 0;

    void serverStart() {
        if(!server) {
            server = [[Server alloc] init];
            [server start];
        }
    }

    void serverStop() {
        if(server) {
            [server stop];
            [server release];
        }
        server = 0;
    }
    
    void serverMessage(const char* message, const char* value) {
        if(server && message && value) {
            NSString* msg = [NSString stringWithUTF8String:message];
            NSString* val = [NSString stringWithUTF8String:value];
            NSDictionary* packet = [NSDictionary dictionaryWithObjectsAndKeys:msg, @"message",val,@"value",nil];
            [server broadcastMessage:packet];
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


