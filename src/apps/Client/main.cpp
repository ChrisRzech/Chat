#include "chat/client/Client.hpp"

#include "chat/common/Logging.hpp"

int main()
{
    try
    {
        const auto logFilepath = "client.log";
        chat::common::Logging::enableLoggingToFile(logFilepath, true);

        chat::client::Client client{"localhost", 25565};
        auto ping = client.ping();
        if(ping.has_value())
        {
            LOG_DEBUG << "Ping: " << ping.value().count() << "ms";
        }
        else
        {
            LOG_DEBUG << "Ping failed";
        }
    }
    catch(const std::exception& exception)
    {
        LOG_ERROR << exception.what();
        return 1;
    }
    catch(...)
    {
        LOG_ERROR << "Unknown exception!";
        return 1;
    }

    return 0;
}
