#ifndef STREAMLOGGER_H
#define STREAMLOGGER_H

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>

/** \brief LogHeader
 * 
 */
struct LogMsgHeader 
{
    unsigned int msg_type;
    unsigned int msg_size;
    unsigned long long log_time_ns; // from 1970

    void set_time_ns() {
        log_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    LogMsgHeader(unsigned int size, unsigned int type = 0) {
        msg_size = size;
        msg_type = type;
        set_time_ns();
    }

    ~LogMsgHeader() {} 
};

/** \brief Stream Logger
 *  Binary logger
 */
class StreamLogger
{
    
public:

    // Set log directory in constructor
    StreamLogger(std::string log_dir = "")
    {
        log_directory = log_dir;
        is_opened = false;
    }
    StreamLogger(const StreamLogger & other)
    {
        std::lock_guard<std::mutex> lk(other.log_lock);
    }
    void operator=( const StreamLogger& other) = delete;
    
    ~StreamLogger()
    {
        if (log_fs != nullptr)
            close_log();
        delete log_fs;
    }

    /** \brief new_log
     *  Create new log file. Only one log can be opened.
     *  \param log_name
     */
    int new_log(std::string log_name)
    {
        log_fs = new std::ofstream((log_directory + log_name).c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app);
        
        if (log_fs == nullptr)
            return -1;
        
        is_opened = true;
        return 0;
    }
    
    /** \brief close_log
     * Close current log
     */
    void close_log()
    {
        log_fs->rdbuf()->pubsync();
        log_fs->close();
        delete log_fs;
        log_fs = nullptr;
        is_opened = false;
    }
    
    /** \brief write
     *  \param data_ptr - data ptr
     *  \param data_length - length of data
     */
    int write(void * data_ptr, unsigned int data_length, unsigned int msg_type = 0)
    {
        log_lock.lock();
        if (log_fs != nullptr) {
            LogMsgHeader cur_header = LogMsgHeader(data_length, msg_type);
            write_header(cur_header);
            log_fs->write((const char *)data_ptr, data_length);
            log_fs->flush();
            log_fs->rdbuf()->pubsync();
        }
        log_lock.unlock();

        return 0;
    }

    /** \brief write_message
     *  \param str_data - some string.
     */
    int write(std::string & str_data, unsigned int msg_type = 0)
    {
        return write((void*)str_data.c_str(), str_data.length(), msg_type);
    }

protected:

    // Log directory
    std::string log_directory;

    // File ptr
    std::ofstream * log_fs = nullptr;

    // Flags & Locks
    bool is_opened;
    mutable std::mutex log_lock;

    // Write header
    void write_header(LogMsgHeader & header) {
        log_fs->write((const char *)&header, sizeof(LogMsgHeader));
    } 
};

#endif // STREAMLOGGER_H
