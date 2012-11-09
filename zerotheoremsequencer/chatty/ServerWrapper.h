extern "C" {
    void serverStart();
    void serverStop();
    void serverMessage(const char* msg, const char* val);
}