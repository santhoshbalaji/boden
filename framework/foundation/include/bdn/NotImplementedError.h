#ifndef BDN_NotImplementedError_H_
#define BDN_NotImplementedError_H_

#include <stdexcept>

namespace bdn
{

    class NotImplementedError : public std::logic_error
    {
      public:
        NotImplementedError(const String &funcName) : std::logic_error("Not implemented: " + funcName) {}
    };
}

#endif
