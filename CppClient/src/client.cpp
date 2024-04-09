#include "DataStreamClient.h"
#include <iostream>
#include <typeinfo>

int main()
{
    ViconDataStreamSDK::CPP::Client StackClient;
    auto Output = StackClient.GetVersion();
    std::cout << typeid(Output).name() << std::endl;
    return 0;
}