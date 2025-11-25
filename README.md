# NHLAPI
api for nhl.com info about nhl hockey
# main
```cpp
#include "NHLAPI.h"
#include <iostream>

int main() {
   NHLAPI api;
    auto schedule_now = api.get_schedule_now().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    schedule_now.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
