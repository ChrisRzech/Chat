#pragma once

#include <mutex>

namespace chat::common
{

/**
 * @brief Syncrhonize an object throughout its lifetime.
 *
 * @details When an object needs to be synchronized, a mutex is usually paired
 * with the object. However, simply pairing the two invites user errors since
 * the object can be used without locking the mutex. This class aims to prevent
 * such errors by only exposing the object once the mutex has been locked.
 *
 * @tparam T The type of the object to synchronize.
 */
template<typename T>
class SynchronizedObject
{
public:
    /**
     * @brief Construct a synchronized object.
     *
     * @tparam Args The types of the arguments.
     *
     * @param args The arguments used to construct the object.
     */
    template<typename... Args>
    explicit SynchronizedObject(Args&&... args)
      : m_mutex{},
        m_value{std::forward<Args>(args)...}
    {}

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    SynchronizedObject(const SynchronizedObject& other) = delete;
    SynchronizedObject& operator=(const SynchronizedObject& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    SynchronizedObject(SynchronizedObject&& other) = delete;
    SynchronizedObject& operator=(SynchronizedObject&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the synchronized object.
     */
    ~SynchronizedObject() = default;

    /**
     * @brief Provides mutually exclusive access to the object.
     *
     * @details @c ConstProxy and @c Proxy only differ in the `const`ness of the
     * return value of their respective @c get().
     */
    class ConstProxy
    {
    public:
        /**
         * @brief Copy operations are disabled.
         * @{
         */
        ConstProxy(const ConstProxy& other) = delete;
        ConstProxy& operator=(const ConstProxy& other) = delete;
        /** @} */

        /**
         * @brief Move operations are disabled.
         * @{
         */
        ConstProxy(ConstProxy&& other) = delete;
        ConstProxy& operator=(ConstProxy&& other) = delete;
        /** @} */

        /**
         * @brief Destroy the proxy.
         */
        ~ConstProxy() = default;

        /**
         * @brief Get the object being synchronized.
         *
         * @details This can only be called when the proxy is an lvalue to
         * prevent obtaining a reference to the object while the mutex is
         * unlocked.
         *
         * @return The object being synchronized.
         */
        [[nodiscard]] const T& get() &
        {
            return *m_value;
        }

        /**
         * @brief Get the lock used on the mutex.
         *
         * @details This allows this type to be used with mechanisms like
         * @c std::condition_variable.
         *
         * @return The locked used on the mutex.
         */
        [[nodiscard]] std::unique_lock<std::mutex>& getLock()
        {
            return m_lock;
        }

    private:
        friend class SynchronizedObject;

        /**
         * @brief Construct a proxy.
         *
         * @param synchronizedValue The synchronized value creating the proxy.
         */
        explicit ConstProxy(const SynchronizedObject& synchronizedValue)
          : m_lock{synchronizedValue.m_mutex},
            m_value{&synchronizedValue.m_value}
        {}

        std::unique_lock<std::mutex> m_lock;
        const T* m_value;
    };

    /**
     * @brief Provides mutually exclusive access to the object.
     *
     * @details @c ConstProxy and @c Proxy only differ in the `const`ness of the
     * return value of their respective @c get().
     */
    class Proxy
    {
    public:
        /**
         * @brief Copy operations are disabled.
         * @{
         */
        Proxy(const Proxy& other) = delete;
        Proxy& operator=(const Proxy& other) = delete;
        /** @} */

        /**
         * @brief Move operations are disabled.
         * @{
         */
        Proxy(Proxy&& other) = delete;
        Proxy& operator=(Proxy&& other) = delete;
        /** @} */

        /**
         * @brief Destroy the proxy.
         */
        ~Proxy() = default;

        /**
         * @brief Get the object being synchronized.
         *
         * @details This can only be called when the proxy is an lvalue to
         * prevent obtaining a reference to the object while the mutex is
         * unlocked.
         *
         * @return The object being synchronized.
         */
        [[nodiscard]] T& get() &
        {
            return *m_value;
        }

        /**
         * @brief Get the lock used on the mutex.
         *
         * @details This allows this type to be used with mechanisms like
         * @c std::condition_variable.
         *
         * @return The locked used on the mutex.
         */
        [[nodiscard]] std::unique_lock<std::mutex>& getLock()
        {
            return m_lock;
        }

    private:
        friend class SynchronizedObject;

        /**
         * @brief Construct a proxy.
         *
         * @param synchronizedValue The synchronized value creating the proxy.
         */
        explicit Proxy(SynchronizedObject& synchronizedValue)
          : m_lock{synchronizedValue.m_mutex},
            m_value{&synchronizedValue.m_value}
        {}

        std::unique_lock<std::mutex> m_lock;
        T* m_value;
    };

    /**
     * @brief Get a proxy that provides exclusive access to the object.
     *
     * @return A proxy that provides exclusive access to the object.
     */
    [[nodiscard]] ConstProxy lock() const
    {
        return ConstProxy{*this};
    }

    /**
     * @brief Get a proxy that provides exclusive access to the object.
     *
     * @return A proxy that provides exclusive access to the object.
     */
    [[nodiscard]] Proxy lock()
    {
        return Proxy{*this};
    }

private:
    mutable std::mutex m_mutex;
    T m_value;
};

}
