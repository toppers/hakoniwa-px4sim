#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include "logger/ilog.hpp"
#include "logger/ilog_file.hpp"
#include <vector>
#include <string>
#include <memory>      // For smart pointers
#include <mutex>       // For thread safety
#include <atomic>      // For atomic operations
#include <stdexcept>   // For exceptions

namespace hako::logger {

typedef struct {
    ILog* log;
    std::unique_ptr<ILogFile> log_file;
} LogEntryType;

#define MAX_WRITE_COUNT 256

class HakoLogger {
private:
    std::vector<LogEntryType> entries;
    int write_count;
    static std::atomic<bool> enable_flag;
    static std::atomic<uint64_t> time_usec;
    mutable std::mutex logger_mutex;

public:
    HakoLogger() : write_count(0) {}

    ~HakoLogger() {
        close();
    }

    void add_entry(ILog& log, std::unique_ptr<ILogFile> log_file) {
        std::lock_guard<std::mutex> lock(logger_mutex);
        log_file->flush();
        entries.push_back({&log, std::move(log_file)});
    }

    static void set_time_usec(uint64_t t) {
        time_usec.store(t, std::memory_order_relaxed);
    }

    static uint64_t get_time_usec() {
        return time_usec.load(std::memory_order_relaxed);
    }

    static void enable() {
        enable_flag.store(true, std::memory_order_relaxed);
    }

    static void disable() {
        enable_flag.store(false, std::memory_order_relaxed);
    }

    void run() {
        if (!enable_flag.load(std::memory_order_relaxed)) {
            return;
        }

        std::lock_guard<std::mutex> lock(logger_mutex);

        try {
            for (auto& entry : entries) {
                auto log_data = entry.log->log_data();
                entry.log_file->write(log_data);
            }

            if (++write_count >= MAX_WRITE_COUNT) {
                for (auto& entry : entries) {
                    entry.log_file->flush();
                }
                write_count = 0;
            }
        } catch (const std::exception& e) {
            // 例外をキャッチしてログを記録するか、リカバリ処理を行う
            throw std::runtime_error(std::string("Error during logging: ") + e.what());
        }
    }

    void reset() {
        std::lock_guard<std::mutex> lock(logger_mutex);
        for (auto& entry : entries) {
            if (entry.log_file) {
                entry.log_file->reset();
                entry.log_file->flush();
            }
        }
        write_count = 0;
    }

    void close() {
        std::lock_guard<std::mutex> lock(logger_mutex);
        for (auto& entry : entries) {
            if (entry.log_file) {
                entry.log_file->flush();
                entry.log_file.reset(); // スマートポインタで安全に削除
            }
        }
        entries.clear();
    }
};

} // namespace hako::logger

#endif /* _LOGGER_HPP_ */
