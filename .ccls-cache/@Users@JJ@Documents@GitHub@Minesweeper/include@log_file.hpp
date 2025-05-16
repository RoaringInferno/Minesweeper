#pragma once

#include <fstream>
#include <string>
#include <filesystem>
#include <stdexcept>

#define LOG_DIR "log/"

class log_file {
    private:
        std::ofstream file;
        std::string filepath;
        bool enabled;

    private:
        void set_path(const std::string &filename) {
            filepath = LOG_DIR + filename + ".log";
        }

        void setup() {
            // Create logs directory if it doesn't exist
            std::filesystem::create_directory(LOG_DIR);

            // Open the file
            file.open(filepath, std::ofstream::trunc | std::ofstream::out);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open log file: " + filepath);
            }
        }

        void setdown() {
            file.close();
        }

    public:
        // Constructor
        log_file(const std::string& filename, bool enable = false)
#ifndef NDEBUG
            : enabled(enable) {
                set_path(filename);
                if (enabled) {
                    setup();
                }
            }
#else
        {};
#endif

        // Move constructor
        log_file(log_file&& other) noexcept
#ifndef NDEBUG
            : file(std::move(other.file))
            , enabled(other.enabled) {
                other.enabled = false;
            }
#else
        {};
#endif

        // Move assignment
        log_file& operator=(log_file&& other) noexcept
#ifndef NDEBUG
        {
            if (this != &other) {
                file = std::move(other.file);
                enabled = other.enabled;
                other.enabled = false;
            }
            return *this;
        }
#else
        {
            return *this;
        };
#endif

        // Copy operations deleted
        log_file(const log_file&) = delete;
        log_file& operator=(const log_file&) = delete;

        // Destructor
        ~log_file()
#ifndef NDEBUG
        {
            if (enabled) {
                setdown();
            }
        }
#else
        {};
#endif

        // Stream insertion operator
        template<typename T>
            log_file& operator<<(T const& value)
#ifndef NDEBUG
            {
                if (enabled) {
                    file << value;
                }
                return *this;
            }
#else
        {
            return *this;
        }
#endif

        // Manipulator support
        log_file& operator<<(std::ostream& (*pf)(std::ostream&))
#ifndef NDEBUG
        {
            if (enabled) {
                file << pf;
            }
            return *this;
        }
#else
        {
            return *this;
        }
#endif

        // Check if logging is enabled
        explicit operator bool() const
#ifndef NDEBUG
        {
            return enabled;
        }
#else
        {
            return false;
        }
#endif

        void enable()
#ifndef NDEBUG
        {
            if (!enabled)
            {
                setup();
            }
            enabled = true;
        }
#else
        {}
#endif

        void enable_log_file()
#ifndef NDEBUG
        {
            enable();
        }
#else
        {}
#endif

        void disable()
#ifndef NDEBUG
        {
            if (enabled) {
                setdown();
            }
            enabled = false;
        }
#else
        {}
#endif

        void set_name(const std::string &filename)
#ifndef NDEBUG
        {
            set_name(filename);
        }
#else
        {}
#endif
};

#undef LOG_DIR
