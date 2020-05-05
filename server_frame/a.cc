#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <utility>
#include <chrono>
class Thread :public std::thread
{
public:
    Thread() : std::thread()
    {

    }

    template<typename T, typename...Args>
    Thread(T&&func, Args&&...args) : std::thread(std::forward<T>(func),
            std::forward<Args>(args)...)
    {
    } 

    int GetId() const 
    {
        return id_;
    }
private:
    int id_;    
};


void run()
{
    std::cout << "run: " << syscall(SYS_gettid) << std::endl;
}


int main1()
{
    return 0;
}
#include <shared_mutex>

int main()
{
    std::shared_mutex mutex;
    std::shared_lock<std::shared_mutex> lock(mutex);
    return 0;
}
