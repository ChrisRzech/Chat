#pragma once

#include "RequestHandler.hpp"
#include "Session.hpp"

#include "chat/common/ThreadPool.hpp"

#include <SFML/Network/SocketSelector.hpp>

#include <cstdint>
#include <list>

namespace chat::server
{
/**
 * @brief A manager for client sessions.
 */
class SessionManager
{
public:
    /**
     * @brief Construct a session manager.
     *
     * @param maxThreadCount The number of threads to use for managing sessions.
     */
    explicit SessionManager(int maxThreadCount);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    SessionManager(const SessionManager& other) = delete;
    SessionManager& operator=(const SessionManager& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    SessionManager(SessionManager&& other) = delete;
    SessionManager& operator=(SessionManager&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the manager.
     */
    ~SessionManager() = default;

    /**
     * @brief Add a new session.
     *
     * @param socket The socket for the session.
     */
    void add(std::unique_ptr<sf::TcpSocket> socket);

    /**
     * @brief Update all the sessions.
     *
     * @details For each session, the manager tries to receive a request, send
     * responses, and remove.
     *
     * If a request is received, the manager handles the request in a separate
     * thread.
     */
    void update();

private:
    /**
     * @brief Try to receive requests on all sessions.
     *
     * @details If a request is received, it is handled on a separate thread.
     */
    void tryReceives();

    /**
     * @brief Try to send responses on all sessions.
     */
    void trySends();

    /**
     * @brief Try to remove sessions.
     *
     * @details A sessoin is removed if it has disconnected.
     */
    void tryRemoves();

    /**
     * @brief Handle the request from a session.
     *
     * @param session The session.

     * @param request The request to handle.
     */
    void handleRequest(Session& session,
                       std::unique_ptr<messages::Request> request);

    common::ThreadPool m_threadPool;
    RequestHandler m_requestHandler;
    sf::SocketSelector m_selector;
    std::list<Session> m_sessions;
};
}
