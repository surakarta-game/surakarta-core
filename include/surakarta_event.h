#pragma once
#include <functional>

template <typename T, typename... Args>
class SurakartaEvent {
   public:
    void AddListener(std::function<void(Args...)> listener) {
        listeners_.push_back(listener);
    }

    void RemoveListener(std::function<void(Args...)> listener) {
        listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
    }

    void Invoke(Args... args) {
        for (const auto& listener : listeners_) {
            listener(args...);
        }
    }

   private:
    std::vector<std::function<void(Args...)>> listeners_;
};
