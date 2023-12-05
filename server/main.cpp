#include "server.hpp"
#include "poll.hpp"
int main(){
    mysql::Poll::libraryInit("/home/xj/projects/distributedServer/dbinc/config.json");
    Server s(8080,"ChatServer");
    s.start();
    return 0;
}