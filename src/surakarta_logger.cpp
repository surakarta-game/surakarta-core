#include "surakarta_logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdexcept>
#include <vector>

void SurakartaLogger::Log(const char* format, ...) {
    const size_t initialBufferSize = 256;
    std::vector<char> buffer(initialBufferSize);

    va_list args;
    va_start(args, format);

    int requiredSize = vsnprintf(buffer.data(), buffer.size(), format, args);

    va_end(args);

    // If vsnprintf returned a negative value, there was an error
    if (requiredSize < 0) {
        // Return an empty string or handle the error as appropriate
        throw std::runtime_error("Error formatting log message");
    }

    // Resize the buffer if the initial size was not enough
    if (static_cast<size_t>(requiredSize) >= buffer.size()) {
        buffer.resize(requiredSize + 1);  // +1 for null terminator
        va_start(args, format);
        vsnprintf(buffer.data(), buffer.size(), format, args);
        va_end(args);
    }

    stream_->Log(buffer.data());
}
