#pragma once
#include <memory>

class SurakartaLoggerStream {
   public:
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
    void Log(const char* log) override {}
};

class SurakartaLoggerNull : public SurakartaLogger {
   public:
    SurakartaLoggerNull()
        : SurakartaLogger(std::make_shared<SurakartaLoggerStreamNull>()) {}
};

class SurakartaLoggerStreamFile : public SurakartaLoggerStream {
   public:
    SurakartaLoggerStreamFile(const char* file_path) {
        file_ = fopen(file_path, "w");
        if (file_ == nullptr) {
            throw std::runtime_error("Error opening log file");
        }
    }

    ~SurakartaLoggerStreamFile() {
        fclose(file_);
    }

    void Log(const char* log) override {
        fprintf(file_, "%s\n", log);
    }

   private:
    FILE* file_;
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
