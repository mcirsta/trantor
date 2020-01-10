/**
 *
 *  Logger.h
 *  An Tao
 *
 *  Public header file in trantor lib.
 *
 *  Copyright 2018, An Tao.  All rights reserved.
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the License file.
 *
 *
 */

#pragma once

#include <trantor/utils/NonCopyable.h>
#include <trantor/utils/Date.h>
#include <trantor/utils/LogStream.h>
#include <string.h>
#include <functional>
#include <iostream>
namespace trantor
{
class Logger : public NonCopyable
{
  public:
    enum LogLevel
    {
        TRANTOR_TRACE = 0,
		TRANTOR_DEBUG,
		TRANTOR_INFO,
		TRANTOR_WARN,
		TRANTOR_ERROR,
		TRANTOR_FATAL,
		TRANTOR_NUM_LOG_LEVELS
    };
    // compile time calculation of basename of source file
    class SourceFile
    {
      public:
        template <int N>
        inline SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1)
        {
            // std::cout<<data_<<std::endl;
            const char *slash = strrchr(data_, '/');  // builtin function
            if (slash)
            {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char *filename) : data_(filename)
        {
            const char *slash = strrchr(filename, '/');
            if (slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char *data_;
        int size_;
    };
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, bool isSysErr);
    Logger(SourceFile file, int line, LogLevel level, const char *func);
    ~Logger();
    LogStream &stream();
    static void setOutputFunction(
        std::function<void(const char *msg, const uint64_t len)> outputFunc,
        std::function<void()> flushFunc)
    {
        outputFunc_() = outputFunc;
        flushFunc_() = flushFunc;
    }

    static void setLogLevel(LogLevel level)
    {
        logLevel_() = level;
    }
    static LogLevel logLevel()
    {
        return logLevel_();
    }

  protected:
    static void defaultOutputFunction(const char *msg, const uint64_t len)
    {
        fwrite(msg, 1, len, stdout);
    }
    static void defaultFlushFunction()
    {
        fflush(stdout);
    }
    void formatTime();
    static LogLevel &logLevel_()
    {
#ifdef RELEASE
        static LogLevel logLevel = LogLevel::TRANTOR_INFO;
#else
        static LogLevel logLevel = LogLevel::TRANTOR_DEBUG;
#endif
        return logLevel;
    }
    static std::function<void(const char *msg, const uint64_t len)>
        &outputFunc_()
    {
        static std::function<void(const char *msg, const uint64_t len)>
            outputFunc = Logger::defaultOutputFunction;
        return outputFunc;
    }
    static std::function<void()> &flushFunc_()
    {
        static std::function<void()> flushFunc = Logger::defaultFlushFunction;
        return flushFunc;
    }
    LogStream logStream_;
    Date date_{Date::now()};
    SourceFile sourceFile_;
    int fileLine_;
    LogLevel level_;
};
#ifdef NDEBUG
#define LOG_TRACE                                                         \
    if (0)                                                                \
    trantor::Logger(__FILE__, __LINE__, trantor::Logger::TRANTOR_TRACE, __func__) \
        .stream()
#else
#define LOG_TRACE                                                         \
    if (trantor::Logger::logLevel() <= trantor::Logger::TRANTOR_TRACE)            \
    trantor::Logger(__FILE__, __LINE__, trantor::Logger::TRANTOR_TRACE, __func__) \
        .stream()
#endif
#define LOG_DEBUG                                                         \
    if (trantor::Logger::logLevel() <= trantor::Logger::TRANTOR_DEBUG)            \
    trantor::Logger(__FILE__, __LINE__, trantor::Logger::TRANTOR_DEBUG, __func__) \
        .stream()
#define LOG_INFO                                              \
    if (trantor::Logger::logLevel() <= trantor::Logger::TRANTOR_INFO) \
    trantor::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN \
    trantor::Logger(__FILE__, __LINE__, trantor::Logger::TRANTOR_WARN).stream()
#define LOG_ERROR \
    trantor::Logger(__FILE__, __LINE__, trantor::Logger::TRANTOR_ERROR).stream()
#define LOG_FATAL \
    trantor::Logger(__FILE__, __LINE__, trantor::Logger::TRANTOR_FATAL).stream()
#define LOG_SYSERR trantor::Logger(__FILE__, __LINE__, true).stream()

const char *strerror_tl(int savedErrno);
}  // namespace trantor
