#pragma once

#include "chat/common/InputByteStream.hpp"
#include "chat/common/OutputByteStream.hpp"

#include "chat/messages/Request.hpp"

namespace chat::messages
{

/**
 * @brief A ping request.
 */
class Ping : public Request
{
public:
    /**
     * @brief Construct a ping request.
     */
    Ping();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Ping(const Ping& other) = delete;
    Ping& operator=(const Ping& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    Ping(Ping&& other) = delete;
    Ping& operator=(Ping&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the ping request.
     */
    ~Ping() override = default;

    /**
     * @brief Serialize the message into a stream.
     *
     * @param stream The stream to serialize the message into.
     */
    void serialize(common::OutputByteStream& stream) const override;

    /**
     * @brief Deserialize the message from a stream.
     *
     * @param stream The stream to deserialize the message from.
     *
     * @return True if the message successfully deserialized; otherwise, false.
     */
    [[nodiscard]] bool deserialize(common::InputByteStream& stream) override;
};

}
