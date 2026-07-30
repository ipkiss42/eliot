#include <cstdlib>
#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "logging.h"

using namespace std;


void initialize_logging() {
    try {
        // Collect sinks into a vector
        vector<spdlog::sink_ptr> sinks;

        // 1. Replicate ConsoleAppender (A1)
        auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(console_sink);

        // 2. Check environment to see if we should replicate RollingFileAppender (A2)
        // If your script sets a custom variable, we drop the file logs.txt here
        const char* config_env = getenv("ELIOT_LOG_PROFILE");
        bool enable_file_logging = (config_env == nullptr || string(config_env) != "NOLOG");

        if (enable_file_logging) {
            // MaxFileSize: 10MB (10 * 1024 * 1024), MaxBackupIndex: 10
            auto file_sink = make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "logs.txt", 10 * 1024 * 1024, 10, true
            );
            sinks.push_back(file_sink);
        }

        // 3. Create the Default Global Combined Logger
        auto combined_logger = make_shared<spdlog::logger>("root", sinks.begin(), sinks.end());

        // 4. Match log4cxx PatternLayout: %d{HH:mm:ss} %-5p %-5r %c:%L - %m%n
        // [%T] Time | [%-5l] Level | [%-5o] Millis elapsed | [%n:%#] Logger Name:Line | %v Actual Message
        combined_logger->set_pattern("[%T] %-5l %-5o %n:%# - %v");

        // Set baseline fallback root level to DEBUG
        combined_logger->set_level(spdlog::level::debug);

        // Set as default global logger so fallback macros work instantly
        spdlog::set_default_logger(combined_logger);

        // 5. Let the environment variable override levels globally or per-class
        spdlog::cfg::load_env_levels();

    } catch (const spdlog::spdlog_ex& ex) {
        cerr << "Log initialization failed: " << ex.what() << endl;
    }
}
