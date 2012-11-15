

#import "Server.h"

extern "C" {

    static Server* server = 0;
    static Connection* target = 0;
    int targetclient = 0;

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
    
    void serverBroadcastMessage(const char* message, const char* value) {
        if(server && message && value) {
            NSString* msg = [NSString stringWithUTF8String:message];
            NSString* val = [NSString stringWithUTF8String:value];
            NSDictionary* packet = [NSDictionary dictionaryWithObjectsAndKeys:msg, @"message",val,@"value",nil];
            if(!server->clients || [server->clients count] < 1) return;
            [server->clients makeObjectsPerformSelector:@selector(sendNetworkPacket:) withObject:packet];
        }
    }

    void serverNextClient() {
        if(!server->clients || !server->clients.count) return;
        targetclient++;
        if(targetclient >= server->clients.count) {
            targetclient = 0;
        }
        int i = 0;
        target = 0;
        for(Connection* connection in server->clients) {
            if(targetclient == i) {
                target = connection;
                NSLog(@"network target client name is now %@",[connection name]);
                break;
            }
            i++;
        }
    }

    void serverPrevClient() {
        if(!server->clients || !server->clients.count) return;
        targetclient--;
        if(targetclient<0) {
            targetclient = 0;
            targetclient = server->clients.count - 1;
        }
        target = 0;
        int i = 0;
        for(Connection* connection in server->clients) {
            if(targetclient == i) {
                target = connection;
                NSLog(@"network target client name is now %@",[connection name]);
                break;
            }
            i++;
        }
    }
    
    void serverNarrowcastNext(const char* message, const char* value) {
        // send to the one we have registered as next
        if(server && message && value) {
            NSString* msg = [NSString stringWithUTF8String:message];
            NSString* val = [NSString stringWithUTF8String:value];
            NSDictionary* packet = [NSDictionary dictionaryWithObjectsAndKeys:msg, @"message",val,@"value",nil];
            if(!server->clients || [server->clients count] < 1) return;
            for(Connection* client in server->clients) {
                if(client == target) {
                    [client sendNetworkPacket:packet];
                    break;
                }
            }
        }
    }

    void serverNarrowcastMessage(const char* message, const char* value, const char* targetname) {
        // send to a supplied name
        if(!targetname || !strlen(targetname)) return;
        NSString* targetclientname = [NSString stringWithFormat:@"%s",targetname];
        if(server && message && value && targetclientname) {
            NSString* msg = [NSString stringWithUTF8String:message];
            NSString* val = [NSString stringWithUTF8String:value];
            NSDictionary* packet = [NSDictionary dictionaryWithObjectsAndKeys:msg, @"message",val,@"value",nil];
            if(!server->clients || [server->clients count] < 1) return;
            for(Connection* client in server->clients) {
                if(client->name && [client->name isEqualToString:targetclientname]) {
                    [client sendNetworkPacket:packet];
                }
            }
        }
    }

    
}


