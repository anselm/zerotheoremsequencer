extern "C" {
    void serverStart();
    void serverStop();
    void browserStart();
    void browserStop();
    void serverMessage(const char* msg);
}