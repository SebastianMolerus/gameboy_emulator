#include <restclient-cpp/restclient.h>

int main()
{
    if (RestClient::Response r = RestClient::get("https://gbdev.io/gb-opcodes/Opcodes.json"); r.code == 200)
    {
        // Work Work Work
    }
}