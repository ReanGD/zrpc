#include "SyncSocketDisconnect.h"

#include <boost/lexical_cast.hpp>

#include "helpers/Cast.h"
#include "helpers/Signals.h"
#include "helpers/ThreadPool.h"

#include "transport/SocketManager.h"

using namespace zrpc;

namespace
{
    TEST_F(TestSyncSocketDisconnect, SendMsgToOfflineServer)
	{
        CSocketManager mng;
        auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5001", helper::StrToBin("client_id"), /*is_sync*/true);

        ASSERT_EQ(false,
             socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));
    }

    TEST_F(TestSyncSocketDisconnect, SendMultipartMsgToOfflineServer)
    {
        CSocketManager mng;
        auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5001", helper::StrToBin("client_id"), /*is_sync*/true);

        ASSERT_EQ(false,
             socket->Send(tBinaryPackage{helper::StrToBin("Hello"), helper::StrToBin("World")}));
    }

    TEST_F(TestSyncSocketDisconnect, SendMsgToDisconnectedServer)
    {
        helper::CSignals signal;
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&]//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5001", server_id, is_sync);

            signal.Send(0);
            ASSERT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello")}),
                 socket->Recv());

            socket->Close();
            boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
            signal.Send(1);
        })
        .Run([&]//client
        {
            signal.Wait(0);
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5001", client_id, is_sync);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

            ASSERT_EQ(true,
                 socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));
            signal.Wait(1);

            ASSERT_EQ(false,
                 socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));

        });
    }

}
