#include "server.hpp"
#include "poll.hpp"
int main(int argc,char **argv){
    mysql::Poll::libraryInit(argv[1]);
    Server s(80,"ChatServer");
    s.start();
    return 0;
}