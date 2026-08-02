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
        vector<spdlog::sink_ptr> sinks;

        // Default profile
        string active_profile = "STDOUT";

        // Allow overriding via an environment variable
        const char* config_env = getenv("ELIOT_LOG_PROFILE");
        if (config_env != nullptr) {
            active_profile = string(config_env);
        }

        // Assemble sinks based on the active profile evaluation
        if (active_profile == "NOLOG" || active_profile == "") {
            // No logging at all
        }
        else if (active_profile == "STDOUT") {
            auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(console_sink);
        }
        else {
            // Default/fallback profile (including custom names):
            // Logs to both stdout and a rotating file appender
            auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(console_sink);

            // MaxFileSize: 10MB (10 * 1024 * 1024), MaxBackupIndex: 10
            auto file_sink = make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "logs.txt", 10 * 1024 * 1024, 10, true
            );
            sinks.push_back(file_sink);
        }

        // Create the default global combined logger
        auto combined_logger = make_shared<spdlog::logger>("root", sinks.begin(), sinks.end());

        // Match log4cxx PatternLayout: %d{HH:mm:ss} %-5p %-5r %c:%L - %m%n
        combined_logger->set_pattern("[%T] %-5l %-5o %n:%# - %v");

        // Set default root level to DEBUG
        combined_logger->set_level(spdlog::level::debug);

        // Set as default global logger
        spdlog::set_default_logger(combined_logger);

        // Let the environment variable override levels globally or per-class
        spdlog::cfg::load_env_levels();

    } catch (const spdlog::spdlog_ex& ex) {
        cerr << "Log initialization failed: " << ex.what() << endl;
    }
}

