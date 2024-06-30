#ifndef TOOLS_SCOPED_ASYNC_WRAPPER_H
#define TOOLS_SCOPED_ASYNC_WRAPPER_H

#include <future>

namespace tools {

class AsyncWrapper {
public:
    AsyncWrapper() = delete;

    template<class... Args>
    AsyncWrapper(Args&&... args) {
        future_ = std::async(std::launch::async, std::forward<Args>(args)...);
    }

    ~AsyncWrapper() {
        future_.get();
    }

private:
    std::future<void> future_{};
};

} // namespace tools

#endif // TOOLS_SCOPED_ASYNC_WRAPPER_H