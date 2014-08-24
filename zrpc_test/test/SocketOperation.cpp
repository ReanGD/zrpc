#include "SocketOperation.h"

#include <boost/lexical_cast.hpp>

#include "helpers/Cast.h"
#include "helpers/ThreadPool.h"

#include "transport/SocketManager.h"

using namespace zrpc;

namespace
{
    TEST_F(TestSocketOperation, HandshakeOnSyncSocket)
    {
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello Server")}),
                 socket->Recv());
            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello Client")}));

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals&)//client
        {
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Hello Client")}),
                 socket->Handshake(tBinaryPackage{helper::StrToBin("Hello Server")}, std::chrono::seconds(10), std::chrono::milliseconds(1)));
        });
    }

    TEST_F(TestSocketOperation, MultipartHandshakeOnSyncSocket)
    {
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello"), helper::StrToBin("Server")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello"), helper::StrToBin("Client")}));

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals&)//client
        {
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Hello"), helper::StrToBin("Client")}),
                socket->Handshake(tBinaryPackage{helper::StrToBin("Hello"), helper::StrToBin("Server")}, std::chrono::seconds(10), std::chrono::milliseconds(1)));
        });
    }

    TEST_F(TestSocketOperation, HandshakeOnAsyncSocket)
    {
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello Server")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello Client")}));

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals&)//client
        {
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Hello Client")}),
                socket->Handshake(tBinaryPackage{helper::StrToBin("Hello Server")}, std::chrono::seconds(10), std::chrono::milliseconds(1)));
        });
    }

    TEST_F(TestSocketOperation, MultipartHandshakeOnAsyncSocket)
    {
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello"), helper::StrToBin("Server")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello"), helper::StrToBin("Client")}));

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals&)//client
        {
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Hello"), helper::StrToBin("Client")}),
                socket->Handshake(tBinaryPackage{helper::StrToBin("Hello"), helper::StrToBin("Server")}, std::chrono::seconds(10), std::chrono::milliseconds(1)));
        });
    }

    TEST_F(TestSocketOperation, SendOnSyncSocket)
    {
        bool is_sync = true;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello Server")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello Client")}));

            // one part message
            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("ClientMsg")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("ServerMsg")}));

            // multi part message
            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Client"), helper::StrToBin("Msg")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Server"), helper::StrToBin("Msg")}));

            // 10 messages
            for(int i=0; i!=10; ++i)
            {
                EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("ClientMsg" + boost::lexical_cast<std::string>(i))}),
                    socket->Recv());
                EXPECT_EQ(true,
                    socket->Send(tBinaryPackage{client_id, helper::StrToBin("ServerMsg" + boost::lexical_cast<std::string>(i))}));
            }

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals&)//client
        {
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Hello Client")}),
                socket->Handshake(tBinaryPackage{helper::StrToBin("Hello Server")}, std::chrono::seconds(10), std::chrono::milliseconds(1)));

            // one part message
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{helper::StrToBin("ClientMsg")}));
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("ServerMsg")}),
                socket->Recv());

            // multi part message
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{helper::StrToBin("Client"), helper::StrToBin("Msg")}));
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Server"), helper::StrToBin("Msg")}),
                socket->Recv());

            // 10 messages
            for(int i=0; i!=10; ++i)
                EXPECT_EQ(true,
                    socket->Send(tBinaryPackage{helper::StrToBin("ClientMsg" + boost::lexical_cast<std::string>(i))}));

            for(int i=0; i!=10; ++i)
                EXPECT_EQ(tBinaryPackage({helper::StrToBin("ServerMsg" + boost::lexical_cast<std::string>(i))}),
                    socket->Recv());
        });
    }

    TEST_F(TestSocketOperation, SendOnAsyncSocket)
    {
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        auto client_id = helper::StrToBin("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Hello Server")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello Client")}));

            // one part message
            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("ClientMsg")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("ServerMsg")}));

            // multi part message
            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("Client"), helper::StrToBin("Msg")}),
                socket->Recv());
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{client_id, helper::StrToBin("Server"), helper::StrToBin("Msg")}));

            // 10 messages
            for(int i=0; i!=10; ++i)
            {
                EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("ClientMsg" + boost::lexical_cast<std::string>(i))}),
                    socket->Recv());
                EXPECT_EQ(true,
                    socket->Send(tBinaryPackage{client_id, helper::StrToBin("ServerMsg" + boost::lexical_cast<std::string>(i))}));
            }

            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunAndFinish([&](helper::CSignals&)//client
        {
            CSocketManager mng;
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Hello Client")}),
                socket->Handshake(tBinaryPackage{helper::StrToBin("Hello Server")}, std::chrono::seconds(10), std::chrono::milliseconds(1)));

            // one part message
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{helper::StrToBin("ClientMsg")}));
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("ServerMsg")}),
                socket->Recv());

            // multi part message
            EXPECT_EQ(true,
                socket->Send(tBinaryPackage{helper::StrToBin("Client"), helper::StrToBin("Msg")}));
            EXPECT_EQ(tBinaryPackage({helper::StrToBin("Server"), helper::StrToBin("Msg")}),
                socket->Recv());

            // 10 messages
            for(int i=0; i!=10; ++i)
                EXPECT_EQ(true,
                    socket->Send(tBinaryPackage{helper::StrToBin("ClientMsg" + boost::lexical_cast<std::string>(i))}));

            for(int i=0; i!=10; ++i)
                EXPECT_EQ(tBinaryPackage({helper::StrToBin("ServerMsg" + boost::lexical_cast<std::string>(i))}),
                    socket->Recv());
        });
    }

    TEST_F(TestSocketOperation, CheckMultiClient)
    {
        bool is_sync = false;
        auto server_id = helper::StrToBin("server_id");
        std::string client_base_id_str("client_id");

        helper::CThreadPool()
        .Run([&](helper::CSignals& signal)//server
        {
            CSocketManager mng;
            auto socket = mng.CreateServerSocket("tcp://*:5000", server_id, is_sync);

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
            signal.Wait(helper::CThreadPool::FINISH);
        })
        .RunMultiAndFinish(3, [&](helper::CSignals&)//client
        {
            CSocketManager mng;

            auto client_id = helper::StrToBin(client_base_id_str + ":" + boost::lexical_cast<std::string>(boost::this_thread::get_id()));
            auto socket = mng.CreateClientSocket("tcp://127.0.0.1:5000", client_id, is_sync);

            EXPECT_EQ(true,
                 socket->Send(tBinaryPackage{client_id, helper::StrToBin("Hello")}));

            EXPECT_EQ(tBinaryPackage({client_id, helper::StrToBin("World")}),
                 socket->Recv());
        });
    }

}
