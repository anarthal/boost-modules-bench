#ifdef USE_MODULES
#    ifdef HAS_STDLIB_MODULES
import std;
import std.compat;
#    else
#        include <coroutine>
#        include <cstdlib>
#        include <exception>
#        include <system_error>
#    endif
import asio;
#else
#    include <asio/awaitable.hpp>
#    include <asio/buffer.hpp>
#    include <asio/co_spawn.hpp>
#    include <asio/connect.hpp>
#    include <asio/deferred.hpp>
#    include <asio/detached.hpp>
#    include <asio/io_context.hpp>
#    include <asio/ip/address_v4.hpp>
#    include <asio/ip/tcp.hpp>
#    include <asio/ssl/error.hpp>
#    include <asio/ssl/stream.hpp>
#    include <asio/this_coro.hpp>
#endif

namespace bench {

namespace net = asio;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;
using std::error_code;

inline void fail(error_code ec, char const* what)
{
    if(ec == ssl::error::make_error_code(net::ssl::error::stream_truncated))
        return;
    exit(1);
}

inline net::awaitable<void> do_session(tcp::socket client_sock)
{
    char buff [4096] {};
    // TODO(CK): Not used? auto ex = co_await net::this_coro::executor;

    ssl::context ctx {ssl::context::tls_client};
    ssl::stream<tcp::socket> stream {std::move(client_sock), ctx};

    co_await stream.async_handshake(ssl::stream_base::server, net::deferred);
    co_await stream.async_read_some(net::buffer(buff), net::deferred);
    co_await stream.async_write_some(net::buffer(buff), net::deferred);
    co_await stream.async_shutdown(net::deferred);
    stream.next_layer().close();
}

inline net::awaitable<void> do_listen()
{
    auto ex = co_await net::this_coro::executor;
    error_code ec;
    tcp::endpoint endpoint {net::ip::address_v4::loopback(), 3000};

    // Open the acceptor
    tcp::acceptor acceptor{ex};
    acceptor.open(endpoint.protocol(), ec);
    if(ec)
        fail(ec, "open");

    // Allow address reuse
    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if(ec)
        fail(ec, "set_option");

    // Bind to the server address
    acceptor.bind(endpoint, ec);
    if(ec)
        fail(ec, "bind");

    // Start listening for connections
    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if(ec)
        fail(ec, "listen");

    for(;;)
    {
        tcp::socket socket{ex};
        co_await acceptor.async_accept(socket, net::deferred);
        net::co_spawn(ex, [s = std::move(socket)]() mutable {
            return do_session(std::move(s));
        }, net::detached);
    }
}

inline int main_impl()
{
    // The io_context is required for all I/O
    net::io_context ioc;

    // Spawn a listening port
    net::co_spawn(
        ioc,
        do_listen,
        // on completion, spawn will call this function
        [](std::exception_ptr ex)
        {
            // if an exception occurred in the coroutine,
            // it's something critical, e.g. out of memory
            // we capture normal errors in the ec
            // so we just rethrow the exception here,
            // which will cause `ioc.run()` to throw
            if (ex)
                std::rethrow_exception(ex);
        });

    ioc.run();

    return 0;
}

}
