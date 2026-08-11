#include <cstdlib>
#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "logging.h"

using namespace std;


void initialize_logging(string_view iDefaultProfile) {
    try {
        string_view activeProfile = iDefaultProfile;
        if (const char* envProfile = std::getenv("ELIOT_LOG_PROFILE")) {
            activeProfile = envProfile;
        }

        vector<spdlog::sink_ptr> sinks;

        const bool isNoLog = activeProfile.contains("NOLOG");
        if (!isNoLog) {
            if (activeProfile.contains("STDOUT")) {
                auto consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
                sinks.push_back(consoleSink);
            }

            if (activeProfile.contains("STDERR")) {
                auto stderrSink = make_shared<spdlog::sinks::stderr_color_sink_mt>();
                sinks.push_back(stderrSink);
            }

            if (activeProfile.contains("FILE")) {
                // MaxFileSize: 10MB, MaxBackupIndex: 10
                auto fileSink = make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    "logs.txt", 10 * 1024 * 1024, 10, true
                );
                sinks.push_back(fileSink);
            }
        }

        // Create the default global combined logger
        auto combinedLogger = make_shared<spdlog::logger>("root", sinks.begin(), sinks.end());

        // Match log4cxx PatternLayout: %d{HH:mm:ss} %-5p %-5r %c:%L - %m%n
        // [%T] Time | [%-5l] Level | [%-5o] Millis elapsed | [%n:%#] Logger Name:Line | %v Actual Message
        combinedLogger->set_pattern("[%T] %-5l %-5o %n:%# - %v");

        // Apply the baseline log level
        if (isNoLog || sinks.empty()) {
            combinedLogger->set_level(spdlog::level::off);
        } else {
            combinedLogger->set_level(spdlog::level::debug);
        }

        // Set as default global logger
        spdlog::set_default_logger(combinedLogger);

        // Let the environment variable override levels globally or per-class
        spdlog::cfg::load_env_levels();
    } catch (const spdlog::spdlog_ex& ex) {
        cerr << "Log initialization failed: " << ex.what() << endl;
    }
}

