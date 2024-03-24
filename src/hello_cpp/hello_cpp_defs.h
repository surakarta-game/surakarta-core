#include <memory>

namespace hello_cpp {
class Base {
   public:
    virtual int DoSomething() { throw std::runtime_error("DoSomething not implemented"); }
};  //* throw 抛出异常
}  // namespace hello_cpp