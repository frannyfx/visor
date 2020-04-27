#include <iostream>
#include <vector>
#include <thread>
#include "Discovery/Discovery.h"
#include "Server/Server.h"

using namespace std;

int main()
{
    vector<HANDLE> threads;
    threads.push_back(Server::Start());
    threads.push_back(Discovery::Start());
    
    for (auto& thread : threads)
        WaitForSingleObject(thread, INFINITE);

    return 0;
}