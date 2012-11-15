extern "C" {
    void serverStart();
    void serverStop();
    void serverNextClient();
    void serverPrevClient();
    void serverNarrowcastNext(const char* msg, const char* val);
    void serverNarrowcastMessage(const char* msg, const char* val, const char* targetname);
    void serverBroadcastMessage(const char* msg, const char* val);
}