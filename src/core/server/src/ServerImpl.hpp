#pragma once

#include "Listener.hpp"
#include "ServerState.hpp"
#include "SessionManager.hpp"

#include "chat/server/Server.hpp"

#include <atomic>
#include <cstdint>

namespace chat::server
{

/**
 * @brief Implementation for @c chat::server::Server.
 */
class Server::Impl
{
public:
    /**
     * @brief Construct a server.
     *
     * @param port The port to listen on.
     *
     * @param maxThreadCount The number of threads for the server to use.
     */
    Impl(std::uint16_t port, int maxThreadCount);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Impl(Impl&& other) = delete;
    Impl& operator=(Impl&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the server.
     */
    ~Impl() = default;

    /**
     * @brief Run the server.
     *
     * @details This blocks until the server is stopped.
     */
    void run();

    /**
     * @brief Stop the server.
     *
     * @details Notify the server's running thread to stop running.
     */
    void stop();

private:
    /**
     * @brief The states of the server.
     */
    enum class State
    {
        Running,
        Stopping,
        Stopped
    };

    std::atomic<State> m_state;
    Listener m_listener;
    SessionManager m_sessionManager;
};

}
