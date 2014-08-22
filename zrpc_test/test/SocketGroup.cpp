#include "SocketGroup.h"

#include <boost/lexical_cast.hpp>

#include "helpers/Cast.h"
#include "helpers/ThreadPool.h"

#include "transport/SocketGroup.h"
#include "transport/SocketManager.h"

using namespace zrpc;

namespace
{

    TEST_F(TestSocketGroup, CheckGroupSync)
    {
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            CSocketGroup group;
            group.Add(mng.CreateServerSocket("tcp://*:5000", server_id, is_sync));
            signal.Send(0);

            auto recv_sockets = group.Recv();

            EXPECT_EQ(1,
                 recv_sockets.size());
            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello")}),
                 (*recv_sockets.begin())->Recv());
            EXPECT_EQ(true,
                 (*recv_sockets.begin())->Send(tBinaryPackage{client_id, helper::StrToBin("World")}));

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals& signal)//client
        {
            signal.Wait(0);

            CSocketManager mng;
            CSocketGroup group;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);
            group.Add(socket);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));

            auto recv_sockets = group.Recv();
            EXPECT_EQ(1,
                 recv_sockets.size());
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("World")}),
                 (*recv_sockets.begin())->Recv());
        });
    }

    TEST_F(TestSocketGroup, CheckGroupAsync)
    {
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            CSocketGroup group;
            group.Add(mng.CreateServerSocket("tcp://*:5000", server_id, is_sync));
            signal.Send(0);

            auto recv_sockets = group.Recv();

            EXPECT_EQ(1,
                 recv_sockets.size());
            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello")}),
                 (*recv_sockets.begin())->Recv());
            EXPECT_EQ(true,
                 (*recv_sockets.begin())->Send(tBinaryPackage{client_id, helper::StrToBin("World")}));

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals& signal)//client
        {
            signal.Wait(0);

            CSocketManager mng;
            CSocketGroup group;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);
            group.Add(socket);

            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));

            auto recv_sockets = group.Recv();
            EXPECT_EQ(1,
                 recv_sockets.size());
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("World")}),
                 (*recv_sockets.begin())->Recv());
        });
    }

}
