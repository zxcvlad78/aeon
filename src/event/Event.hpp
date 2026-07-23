#include <vector>
#include <functional>
#include <utility>

template<typename... Args>
class Event {
private:
    std::vector<std::function<void(Args...)>> callbacks;
    
public:
    void listen(std::function<void(Args...)> callback) {
        if (callback) {
            callbacks.push_back(std::move(callback));
        }
    }


    void unlisten(std::function<void(Args...)> callback) {
        if (callbacks.find(callback)) {
            callbacks.erase(callback);
        }
    }

    void emit(Args... args) const {
        for (const auto& cb : callbacks) {
            cb(std::forward<Args>(args)...); 
        }
    }
    
    void clear() {
        callbacks.clear();
    }
    
    size_t size() const {
        return callbacks.size();
    }
};
