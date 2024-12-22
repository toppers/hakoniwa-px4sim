#ifndef _ILOGFILE_HPP_
#define _ILOGFILE_HPP_

#include <string>
#include <vector>
#include "ilog.hpp"

namespace hako::logger {

/**
 * Abstract interface for log file management.
 * Defines methods for writing, reading, flushing, and managing log data.
 */
class ILogFile {
public:
    /**
     * Virtual destructor to ensure proper cleanup in derived classes.
     */
    virtual ~ILogFile() {}

    /**
     * Pure virtual method for writing log data.
     * Must be implemented in derived classes.
     * @param value Log data to be written.
     */
    virtual void write(const std::vector<LogDataType>& value) = 0;

    /**
     * Pure virtual method for flushing log data to the file.
     * Must be implemented in derived classes.
     */
    virtual void flush() = 0;

    /**
     * Pure virtual method for reading log data.
     * Must be implemented in derived classes.
     * @param value Output parameter to hold the read data.
     * @return True if reading was successful, false otherwise.
     */
    virtual bool read(std::vector<LogDataType>& value) = 0;

    /**
     * Pure virtual method for removing the last log entry.
     * Must be implemented in derived classes.
     */
    virtual void remove_last() = 0;

    /**
     * Pure virtual method for resetting the log file.
     * Must be implemented in derived classes.
     */
    virtual void reset() = 0;

protected:
    /**
     * Constructor for creating a new log file.
     * @param file_name The name of the file.
     * @param header The header information for the log file.
     */
    ILogFile(const std::string& file_name, const std::vector<LogHeaderType>& header) 
    {
        (void)file_name;
        (void)header;
    }

    /**
     * Constructor for loading an existing log file.
     * @param file_name The name of the file to load.
     */
    ILogFile(const std::string& file_name) {
        (void)file_name;
    }
};

} // namespace hako::logger

#endif /* _ILOGFILE_HPP_ */
