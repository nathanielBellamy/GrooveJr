//
// Created by ns on 5/31/25.
//

#ifndef DBLOGGING_H
#define DBLOGGING_H

#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

namespace Gj {
namespace Db {

enum LogSeverityLevel {
  Info,
  Notification,
  Warning,
  Error,
  Critical
};

class Logging {

  public:
   static void write(LogSeverityLevel severity, const std::string& caller, const std::string& message);

};

} // Db
} // Gj

#endif //DBLOGGING_H
