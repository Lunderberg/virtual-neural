#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <functional>
#include <vector>

template<typename... Args>
class Signal {
public:
  typedef std::function<void(Args...)> func_t;

  Signal()
    : enabled(true) { }

  void Connect(func_t func) {
    callbacks.push_back(func);
  }

  void Emit(Args... args) const {
    if(enabled) {
      for(auto& func : callbacks) {
        func(args...);
      }
    }
  }

  void Enable() { enabled = true; }
  void Disable() { enabled = false; }
  bool IsEnabled() const { return enabled; }

private:
  std::vector<func_t> callbacks;
  bool enabled;
};

#endif /* _SIGNAL_H_ */
