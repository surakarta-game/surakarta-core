#pragma once
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

class SurakartaLoggerStream {
   public:
    virtual ~SurakartaLoggerStream() = default;
    virtual void Log(const char* log) = 0;
};

class SurakartaLogger {
   public:
    SurakartaLogger(std::shared_ptr<SurakartaLoggerStream> stream)
        : stream_(stream) {}

    void Log(const char* format, ...);

   private:
    std::shared_ptr<SurakartaLoggerStream> stream_;
};

class SurakartaLoggerStreamStdout : public SurakartaLoggerStream {
   public:
    void Log(const char* log) override {
        printf("%s\n", log);
    }
};

class SurakartaLoggerStdout : public SurakartaLogger {
   public:
    SurakartaLoggerStdout()
        : SurakartaLogger(std::make_shared<SurakartaLoggerStreamStdout>()) {}
};

class SurakartaLoggerStreamStderr : public SurakartaLoggerStream {
   public:
    void Log(const char* log) override {
        fprintf(stderr, "%s\n", log);
    }
};

class SurakartaLoggerStderr : public SurakartaLogger {
   public:
    SurakartaLoggerStderr()
        : SurakartaLogger(std::make_shared<SurakartaLoggerStreamStderr>()) {}
};

class SurakartaLoggerStreamNull : public SurakartaLoggerStream {
   public:
    void Log(const char* log [[maybe_unused]]) override {}
};

class SurakartaLoggerNull : public SurakartaLogger {
   public:
    SurakartaLoggerNull()
        : SurakartaLogger(std::make_shared<SurakartaLoggerStreamNull>()) {}
};

class SurakartaLoggerStreamFile : public SurakartaLoggerStream {
   public:
    SurakartaLoggerStreamFile(const char* file_path) {
        file_.open(file_path, std::ios::out | std::ios::app);
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
    }

    ~SurakartaLoggerStreamFile() {
        file_.close();
    }

    void Log(const char* log) override {
        file_ << log << std::endl;
    }

   private:
    std::fstream file_;
};

class SurakartaLoggerFile : public SurakartaLogger {
   public:
    SurakartaLoggerFile(const char* file_path)
        : SurakartaLogger(std::make_shared<SurakartaLoggerStreamFile>(file_path)) {}
};

class SurakartaLoggerStreamMultiple : public SurakartaLoggerStream {
   public:
    SurakartaLoggerStreamMultiple(std::shared_ptr<SurakartaLoggerStream> stream1,
                                  std::shared_ptr<SurakartaLoggerStream> stream2)
        : stream1_(stream1), stream2_(stream2) {}

    void Log(const char* log) override {
        stream1_->Log(log);
        stream2_->Log(log);
    }

   private:
    std::shared_ptr<SurakartaLoggerStream> stream1_;
    std::shared_ptr<SurakartaLoggerStream> stream2_;
};

class SurakartaLoggerStreamWithPrefix : public SurakartaLoggerStream {
   public:
    SurakartaLoggerStreamWithPrefix(std::shared_ptr<SurakartaLoggerStream> stream, std::string prefix)
        : stream_(stream), prefix_(prefix) {}

    void Log(const char* log) override {
        std::string log_str = prefix_ + log;
        stream_->Log(log_str.c_str());
    }

   private:
    std::shared_ptr<SurakartaLoggerStream> stream_;
    std::string prefix_;
};
