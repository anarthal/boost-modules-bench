module;

#include <asio.hpp>
#include <asio/ssl.hpp>

export module asio;

namespace asio
{
export using asio::io_context;
export using asio::post;
export using asio::any_completion_executor;
export using asio::any_io_executor;
export using asio::bad_executor;
export using asio::cancellation_signal;
export using asio::cancellation_slot;
export using asio::cancellation_state;
export using asio::cancellation_type;
export using asio::coroutine;
export using asio::detached_t;
export using asio::execution_context;
export using asio::executor;
export using asio::executor_arg_t;
export using asio::invalid_service_owner;
export using asio::io_context;
export using asio::multiple_exceptions;
export using asio::service_already_exists;
export using asio::static_thread_pool;
export using asio::system_context;
export using asio::system_executor;
export using asio::thread_pool;
export using asio::awaitable;
export using asio::deferred;
export using asio::detached;
export using asio::connect;
export using asio::async_connect;
export using asio::buffer;
export using asio::socket_base;
export using asio::co_spawn;

namespace error
{
export using asio::error::make_error_code;
}  // namespace error

namespace this_coro
{
export using asio::this_coro::executor;
}  // namespace this_coro

namespace ip
{
export using asio::ip::tcp;
export using asio::ip::address;
export using asio::ip::address_v4;
}  // namespace ip

namespace ssl
{
export using asio::ssl::context;
export using asio::ssl::context_base;
export using asio::ssl::host_name_verification;
export using asio::ssl::stream_base;
export using asio::ssl::verify_context;
export using asio::ssl::stream;
namespace error
{
export using asio::ssl::error::stream_truncated;
export using asio::ssl::error::unspecified_system_error;
export using asio::ssl::error::unexpected_result;
export using asio::ssl::error::stream_errors;
export using asio::ssl::error::make_error_code;
}  // namespace error
}  // namespace ssl

}  // namespace asio
