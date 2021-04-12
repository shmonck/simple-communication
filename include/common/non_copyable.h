#pragma once

/**
 * @class NonCopyable
 *
 * @brief Used to make inherited classes uncopyable
 *
 */
class NonCopyable
{
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};