#ifndef _CSV_LOGGER_HPP_
#define _CSV_LOGGER_HPP_

#include "csv_data.hpp"
#include "icsv_log.hpp"

typedef struct {
    ICsvLog *log;
    CsvData *csv_data;
} CsvLogEntryType;

#define MAX_WRITE_COUNT 256
class CsvLogger {
private:
    std::vector<CsvLogEntryType> entries;
    int write_count;
    static bool enable_flag;
    static uint64_t time_usec;
public:
    CsvLogger() : write_count(0) {}

    virtual ~CsvLogger() {
        close();
    }

    void add_entry(ICsvLog& log, const std::string& file_name) {
        CsvData *csv_data = new CsvData(file_name, {log.log_head()});
        csv_data->flush();
        CsvLogEntryType entry = { &log, csv_data };
        entries.push_back(entry);
    }
    static void set_time_usec(uint64_t t)
    {
        time_usec = t;
    }
    static uint64_t get_time_usec()
    {
        return time_usec;
    }
    static void enable()
    {
        enable_flag = true;
    }
    static void disable()
    {
        enable_flag = false;
    }
    void run() {
        if (enable_flag == false) {
            return;
        }
        for (auto& entry : entries) {
            auto log_data = entry.log->log_data();
            entry.csv_data->write(log_data);
        }
        if (++write_count >= MAX_WRITE_COUNT) {
            for (auto& entry : entries) {
                entry.csv_data->flush();
            }
            write_count = 0;
        }
    }

    void close() {
        for (auto& entry : entries) {
            if (entry.csv_data) {
                entry.csv_data->flush();
                delete entry.csv_data;
                entry.csv_data = nullptr;
            }
        }
        entries.clear();
    }
};
#endif /* _CSV_LOGGER_HPP_ */