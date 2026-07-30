/*****************************************************************************
 * Eliot
 * Copyright (C) 2011 Olivier Teulière
 * Authors: Olivier Teulière <ipkiss @@ gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************************/

#ifndef DIC_LOGGING_H_
#define DIC_LOGGING_H_

#include <config.h>

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/spdlog.h>

// Define the static logger pointer inside the class header
#define DEFINE_LOGGER() static std::shared_ptr<spdlog::logger> logger

// Initialize the logger in the .cpp file.
// Uses the combined "prefix.className" as the logger name.
#define INIT_LOGGER(prefix, className) \
    std::shared_ptr<spdlog::logger> className::logger = []() { \
        auto l = spdlog::get(#prefix "." #className); \
        return l ? l : spdlog::default_logger(); \
    }()

// Instance-specific logging macros supporting modern formatting
#define LOG_TRACE(...) SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#define LOG_INFO(...)  SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#define LOG_WARN(...)  SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#define LOG_FATAL(...) SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)

// Root logging macros (maps to spdlog's default global logger)
#define LOG_ROOT_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_ROOT_FATAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

void initialize_logging();

#endif
