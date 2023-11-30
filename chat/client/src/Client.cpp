#include "chat/client/Client.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Ping.hpp"
#include "chat/messages/Pong.hpp"
#include "chat/messages/Serializer.hpp"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <cstdint>

namespace chat::client
{

class Client::Impl
{
public:
    Impl(sf::IpAddress host, uint16_t port)
      : m_host{std::move(host)},
        m_port{port},
        m_socket{},
        m_connected{false},
        m_serializer{}
    {
        m_socket.setBlocking(true);
    }

    [[nodiscard]] std::optional<std::string> ping(std::string message)
    {
        LOG_DEBUG << "Sending test request...";

        std::optional<std::string> result;
        if(connect())
        {
            if(auto response = sendAndReceive<chat::messages::Pong>(chat::messages::Ping{std::move(message)}); response.has_value())
            {
                result = std::make_optional(std::move(response.value()->getMessage()));
            }
        }

        LOG_DEBUG << "Finished sending test request";
        return result;
    }

private:
    [[nodiscard]] bool connect()
    {
        if(m_connected)
        {
            return true;
        }

        LOG_DEBUG << "Connecting to host...";

        bool success = false;
        switch(m_socket.connect(m_host, m_port))
        {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Connected to host";
            m_connected = true;
            success = true;
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not connect to host, unexpected `sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not connect to host, unexpected `sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not connect to host, unexpected `sf::Socket::Status::Disconnected`";
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occured while trying to connect to host";
            break;
        }

        LOG_DEBUG << "Finished connecting to host";
        return success;
    }


    [[nodiscard]] bool sendPacket(sf::Packet& packet)
    {
        LOG_DEBUG << "Sending packet...";

        bool success = false;
        switch(m_socket.send(packet))
        {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Packet sent";
            success = true;
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not send request, unexpected `sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not send request, unexpected `sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not send request, unexpected `sf::Socket::Status::Disconnected`";
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occured while trying to send request";
            break;
        }

        LOG_DEBUG << "Finished sending packet";
        return success;
    }

    [[nodiscard]] std::optional<sf::Packet> receivePacket()
    {
        LOG_DEBUG << "Receiving packet...";

        bool success = false;
        sf::Packet packet;
        switch(m_socket.receive(packet))
        {
        case sf::Socket::Status::Done:
            LOG_DEBUG << "Packet received";
            success = true;
            break;

        case sf::Socket::Status::NotReady:
            LOG_WARN << "Could not receive request, unexpected `sf::Socket::Status::NotReady`";
            break;

        case sf::Socket::Status::Partial:
            LOG_WARN << "Could not receive request, unexpected `sf::Socket::Status::Partial`";
            break;

        case sf::Socket::Status::Disconnected:
            LOG_WARN << "Could not receive request, unexpected `sf::Socket::Status::Disconnected`";
            break;

        case sf::Socket::Status::Error:
            LOG_WARN << "An error occured while trying to receive request";
            break;
        }

        LOG_DEBUG << "Finished receiving packet";
        return success ? std::make_optional(packet) : std::nullopt;
    }

    [[nodiscard]] bool sendRequest(const chat::messages::Request& request)
    {
        LOG_DEBUG << "Sending request...";

        sf::Packet packet;
        m_serializer.serialize(request, packet);
        bool success = sendPacket(packet);

        LOG_DEBUG << "Finished sending request";;
        return success;
    }

    template<typename ResponseType>
    [[nodiscard]] std::optional<std::unique_ptr<ResponseType>> receiveResponse()
    {
        static_assert(std::is_base_of_v<chat::messages::Response, ResponseType>, "Response is not a base of ResponseType");

        LOG_DEBUG << "Receiving response...";

        std::optional<std::unique_ptr<ResponseType>> response;
        if(auto packet = receivePacket(); packet.has_value())
        {
            if(auto message = m_serializer.deserialize(packet.value()); message.has_value())
            {
                /*
                The message is placed inside an `std::unique_ptr`. There is no standard library functionality to transfer ownership from a
                `std::unique_ptr` base type to a `std::unique_ptr` derived type. This must be done manually.
                */
                if(auto temp = dynamic_cast<ResponseType*>(message.value().get()); temp != nullptr)
                {
                    message.value().release();
                    response = std::make_optional(std::unique_ptr<ResponseType>{temp});
                }
            }
        }

        LOG_DEBUG << "Finished receiving response";
        return response;
    }

    template<typename ResponseType>
    [[nodiscard]] std::optional<std::unique_ptr<ResponseType>> sendAndReceive(const chat::messages::Request& request)
    {
        if(sendRequest(request))
        {
            return receiveResponse<ResponseType>();
        }

        return std::nullopt;
    }

    sf::IpAddress m_host;
    uint16_t m_port;
    sf::TcpSocket m_socket;
    bool m_connected;
    chat::messages::Serializer m_serializer;
};

Client::Client(sf::IpAddress host, uint16_t port)
  : m_impl{std::make_unique<Impl>(std::move(host), port)}
{}

Client::~Client() = default;

std::optional<std::string> Client::ping(std::string message)
{
    return m_impl->ping(std::move(message));
}

}