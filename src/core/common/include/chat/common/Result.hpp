#pragma once

#include <optional>

namespace chat::common
{

/**
 * @brief A strong type to represent an error when constructing a @c Result.
 *
 * @tparam T The type of the value.
 */
template<typename T>
struct Error
{
    /**
     * @brief Construct an error with a value.
     *
     * @tparam T The type of the value.
     *
     * @param value The value.
     */
    explicit Error(T value)
      : value{std::move(value)}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Error(const Error& other) = delete;
    Error& operator=(const Error& other) = delete;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    Error(Error&& other) noexcept = default;
    Error& operator=(Error&& other) noexcept = default;
    /** @} */

    /**
     * @brief Destroy the error.
     */
    ~Error() = default;

    T value;
};

/**
 * @brief A type that holds either a success value or an error value.
 *
 * @details A common use case is to allow a function to return a success value
 * or an error value. The type of the success value is the type expected to be
 * returned when the function is performed successfully. The type of the error
 * value is the type that indicates why a success value is not returned, which
 * is usually simply an integral type but not required to be.
 *
 * To make a result hold a success value, simply use the appropriate constructor
 * or assignment operator. To make a result hold an error value, the error value
 * should be placed in an @c Error before being passed to the appropriate
 * constructor or assignment operator.
 *
 * Requiring the explicit use of @c Error prevents mistaking whether a result
 * holds a success value or an error value. In addition, this also allows the
 * success and error types to be the same.
 *
 * If the error type is `void` or is not provided, a partial template
 * specialization of @c Result is used.
 *
 * @tparam S The type of the success value.
 *
 * @tparam E The type of the error value. Defaults to `void`.
 */
template<typename S, typename E = void>
class Result
{
public:
    /**
     * @brief Construct a result that holds a success value.
     *
     * @param value The value.
     */
    explicit Result(S value)
      : m_success{std::move(value)},
        m_error{}
    {}

    /**
     * @brief Construct a result that holds an error value.
     *
     * @param value The value.
     */
    explicit Result(Error<E> value)
      : m_success{},
        m_error{std::move(value.value)}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Result(const Result& other) = delete;
    Result& operator=(const Result& other) = delete;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    Result(Result&& other) noexcept = default;
    Result& operator=(Result&& other) noexcept = default;
    /** @} */

    /**
     * @brief Destroy the result.
     */
    ~Result() = default;

    /**
     * @brief Assign a new success value.
     *
     * @details If a success value already exists, it is replaced with the new
     * success value. If an error value already exists, it is destroyed and the
     * new success value is held.
     *
     * @param value The value.
     *
     * @return This object.
     */
    Result& operator=(S value)
    {
        m_success = std::move(value);
        m_error.reset();
        return *this;
    }

    /**
     * @brief Assign a new error value.
     *
     * @details If a success value already exists, it is destroyed and the new
     * error value is held. If an error value already exists, it is replaced
     * with the new error value.
     *
     * @param value The value.
     *
     * @return This object.
     */
    Result& operator=(Error<E> value)
    {
        m_success.reset();
        m_error = std::move(value.value);
        return *this;
    }

    /**
     * @brief Check if a success value is held.
     *
     * @return True if a success value is held; otherwise, false.
     */
    [[nodiscard]] bool isSuccess() const
    {
        return m_success.has_value();
    }

    /**
     * @brief Get the success value.
     *
     * @return The success value.
     */
    [[nodiscard]] S& getSuccessValue()
    {
        return m_success.value();
    }

    /**
     * @brief Get the success value.
     *
     * @return The success value.
     */
    [[nodiscard]] const S& getSuccessValue() const
    {
        return m_success.value();
    }

    /**
     * @brief Get the error value.
     *
     * @return The error value.
     */
    [[nodiscard]] E& getErrorValue()
    {
        return m_error.value();
    }

    /**
     * @brief Get the error value.
     *
     * @return The error value.
     */
    [[nodiscard]] const E& getErrorValue() const
    {
        return m_error.value();
    }

private:
    std::optional<S> m_success;
    std::optional<E> m_error;
};

/**
 * @brief A type that holds a success value or no success value.
 *
 * @details This is a partial template specialization of @c Result where the
 * error value does not exist and instead can only indicate that a success value
 * exists or not. This essentially acts as an @c std::optional.
 *
 * @tparam S The type of the success value.
 */
template<typename S>
class Result<S, void>
{
public:
    /**
     * @brief Construct a result that holds no success value.
     */
    Result() = default;

    /**
     * @brief Construct a result that holds a success value.
     *
     * @param value The value.
     */
    explicit Result(S value)
      : m_success{std::move(value)}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    Result(const Result& other) = delete;
    Result& operator=(const Result& other) = delete;
    /** @} */

    /**
     * @brief Move operations are enabled.
     * @{
     */
    Result(Result&& other) noexcept = default;
    Result& operator=(Result&& other) noexcept = default;
    /** @} */

    /**
     * @brief Destroy the result.
     */
    ~Result() = default;

    /**
     * @brief Assign a new success value.
     *
     * @details If a success value already exists, it is replaced with the new
     * success value. If a success value does not exist, the new success value
     * is held.
     *
     * @param value The value.
     *
     * @return This object.
     */
    Result& operator=(S value)
    {
        m_success = std::move(value);
        return *this;
    }

    /**
     * @brief Check if a success value is held.
     *
     * @return True if a success value is held; otherwise, false.
     */
    [[nodiscard]] bool isSuccess() const
    {
        return m_success.has_value();
    }

    /**
     * @brief Get the success value.
     *
     * @return The success value.
     */
    [[nodiscard]] S& getSuccessValue()
    {
        return m_success.value();
    }

    /**
     * @brief Get the success value.
     *
     * @return The success value.
     */
    [[nodiscard]] const S& getSuccessValue() const
    {
        return m_success.value();
    }

private:
    std::optional<S> m_success;
};

}
