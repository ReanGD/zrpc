#include <atomic>
#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread_only.hpp>
#include <boost/thread/condition_variable.hpp>

#include "transport/SocketGroup.h"
#include "transport/SocketManager.h"

using namespace std;

struct SGlobal
{
    SGlobal(void)
        : finish(false)
    {

    }

    boost::mutex mutex;
    atomic<bool> finish;
    boost::condition_variable cv;
};

void ServerThread(SGlobal& global);
void ClientThread(SGlobal& global);

void ServerThread(SGlobal& global)
{
    zrpc::CSocketManager mng;

    std::string id("server_id");
    auto socket = mng.CreateServerSocket("tcp://*:6000", zrpc::tBinary(id.begin(), id.end()), /*is_sync*/true);

    auto pkg = socket->Recv();
    auto it = pkg.begin();

    auto client_id = *it;
    auto client_data = *++it;

    std::string question(client_data.begin(), client_data.end());
    cout << "server << " << question << endl;

    std::string answer = question + ":answer";
    cout << "server >> " << answer << endl;
    socket->Send(zrpc::tBinaryPackage{client_id, zrpc::tBinary(answer.begin(), answer.end())});

    {
        boost::unique_lock<boost::mutex> locker(global.mutex);
        while (!global.finish.load())
            global.cv.wait(locker);
    }
}

void ClientThread(SGlobal& global)
{
    zrpc::CSocketManager mng;

    std::string id("client_id");
    auto socket = mng.CreateClientSocket("tcp://127.0.0.1:6000", zrpc::tBinary(id.begin(), id.end()), /*is_sync*/true);

    std::string question("Hello");
    cout << "client >> " << question << endl;
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    socket->Send(zrpc::tBinaryPackage{zrpc::tBinary(question.begin(), question.end())});

    auto pkg = socket->Recv();
    auto answer = *pkg.begin();
    cout << "client << " << std::string(answer.begin(), answer.end()) << endl;

    {
        boost::unique_lock<boost::mutex> locker(global.mutex);
        global.finish = true;
        global.cv.notify_all();
    }
}

int main(void)
{
    cout<<"Start"<<endl;

    SGlobal global;

    boost::thread server(ServerThread, boost::ref(global));
    boost::thread client(ClientThread, boost::ref(global));
    server.join();
    client.join();

    cout<<"Done"<<endl;

    return 0;
}
