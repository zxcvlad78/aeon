#include <vector>
#include <functional>
#include <utility>

template<typename... Args>
class Signal {
private:
    std::vector<std::function<void(Args...)>> callbacks;
    
public:
    void connect(std::function<void(Args...)> callback) {
        if (callback) {
            callbacks.push_back(std::move(callback));
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
