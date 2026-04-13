//
// Created by Copilot
//

#ifndef GJ_PATHS_H
#define GJ_PATHS_H

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace Gj {

class Paths {
  static std::filesystem::path homeDir() {
#ifdef _WIN32
    const char* home = std::getenv("LOCALAPPDATA");
    if (!home) home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    if (!home)
      throw std::runtime_error("Unable to determine home directory");
    return std::filesystem::path(home);
  }

public:
  // Persistent application data (database, config, etc.)
  // macOS:   ~/Library/Application Support/GrooveJr/
  // Linux:   $XDG_DATA_HOME/GrooveJr/ (default ~/.local/share/GrooveJr/)
  // Windows: %LOCALAPPDATA%/GrooveJr/
  static std::filesystem::path dataDir() {
#if defined(__APPLE__)
    auto dir = homeDir() / "Library" / "Application Support" / "GrooveJr";
#elif defined(_WIN32)
    auto dir = homeDir() / "GrooveJr";
#else
    const char* xdgData = std::getenv("XDG_DATA_HOME");
    auto dir = xdgData ? std::filesystem::path(xdgData) / "GrooveJr"
                       : homeDir() / ".local" / "share" / "GrooveJr";
#endif
    std::filesystem::create_directories(dir);
    return dir;
  }

  // Log files
  // macOS:   ~/Library/Logs/GrooveJr/
  // Linux:   $XDG_STATE_HOME/GrooveJr/ (default ~/.local/state/GrooveJr/)
  // Windows: %LOCALAPPDATA%/GrooveJr/Logs/
  static std::filesystem::path logDir() {
#if defined(__APPLE__)
    auto dir = homeDir() / "Library" / "Logs" / "GrooveJr";
#elif defined(_WIN32)
    auto dir = homeDir() / "GrooveJr" / "Logs";
#else
    const char* xdgState = std::getenv("XDG_STATE_HOME");
    auto dir = xdgState ? std::filesystem::path(xdgState) / "GrooveJr"
                        : homeDir() / ".local" / "state" / "GrooveJr";
#endif
    std::filesystem::create_directories(dir);
    return dir;
  }

  static std::filesystem::path databasePath() {
    return dataDir() / "groovejr.db";
  }
};

} // Gj

#endif // GJ_PATHS_H
