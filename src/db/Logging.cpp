//
// Created by ns on 5/31/25.
//

#include "Logging.h"

namespace Gj {
namespace Db {

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(DbLogger, boost::log::sources::severity_logger<LogSeverityLevel>);

void Logging::write(LogSeverityLevel severity, const std::string& caller, const std::string& message) {
  boost::log::sources::severity_logger<LogSeverityLevel> lg = DbLogger::get();
  lg.add_attribute("DbSeverity", boost::log::attributes::constant<LogSeverityLevel>(severity));
  lg.add_attribute("Caller", boost::log::attributes::constant<std::string>(caller));
  BOOST_LOG_SEV(lg, severity) << message; // NOTE: severity here is a placeholder
}

} // Db
} // GJ
