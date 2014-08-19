#include "SendRecv.h"

#include <boost/lexical_cast.hpp>

#include "helpers/Signals.h"
#include "helpers/Cast.h"
#include "helpers/ThreadPool.h"

#include "transport/SocketManager.h"
#include "transport/SocketGroup.h"

using namespace zrpc;

namespace
{
    TEST_F(TestSendRecv, CheckSync)
	{
        helper::CSignals signal;
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&]//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);
            signal.Send(0);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello")}),
                 socket->Recv());
            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{client_id, helper::StrToBin("World")}));

            signal.Wait(1);
        })
        .Run([&]//client
        {
            signal.Wait(0);

            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("World")}),
                 socket->Recv());

            signal.Send(1);
        });
    }

    TEST_F(TestSendRecv, CheckAsync)
    {
        helper::CSignals signal;
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&]//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);
            signal.Send(0);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello")}),
                 socket->Recv());
            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{client_id, helper::StrToBin("World")}));

            signal.Wait(1);
        })
        .Run([&]//client
        {
            signal.Wait(0);

            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{helper::StrToBin("Hello")}));
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("World")}),
                 socket->Recv());

            signal.Send(1);
        });
    }

    TEST_F(TestSendRecv, CheckGroupSync)
    {
        helper::CSignals signal;
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&]//server
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

            signal.Wait(1);
        })
        .Run([&]//client
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

            signal.Send(1);
        });
    }

    TEST_F(TestSendRecv, CheckGroupAsync)
    {
        helper::CSignals signal;
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&]//server
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

            signal.Wait(1);
        })
        .Run([&]//client
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

            signal.Send(1);
        });
    }

    TEST_F(TestSendRecv, CheckMultiClient)
    {
        helper::CSignals signal;
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        std::string client_base_id_str("client_id");

        helper::CThreadPool()
        .Run([&]//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);
            signal.Send(0);

            for(int i=0; i!=3; ++i)
            {
                auto pkg = socket->Recv();
                EXPECT_EQ(3, pkg.size());
                auto it = pkg.begin();

                auto client_id = *it;
                EXPECT_EQ(client_id, *++it);
                EXPECT_EQ(std::string("Hello"), helper::BinToStr(*++it));

                EXPECT_EQ(true,
                     socket->Send(tBinaryPackage{client_id, client_id, helper::StrToBin("World")}));
            }
            signal.Wait(1);
        })
        .RunMulti(3, [&]//client
        {
            signal.Wait(0);

            CSocketManager mng;

            auto client_id = helper::StrToBin(client_base_id_str + ":" + boost::lexical_cast<std::string>(boost::this_thread::get_id()));
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello")}));

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("World")}),
                 socket->Recv());

            signal.Send(1);//Fix!!
        });
    }

}