//
// Created by ns on 3/13/25.
//

#include <iostream>
#include "Logging.h"

namespace Gj {
namespace Audio {

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(AudioLogger, boost::log::sources::severity_logger<LogSeverityLevel>);

void Logging::write(LogSeverityLevel severity, const std::string& caller, const std::string& message) {
  // TODO:
  // - debug severity in log
  boost::log::sources::severity_logger<LogSeverityLevel> lg = AudioLogger::get();
  lg.add_attribute("Caller", boost::log::attributes::constant<std::string>(caller));
  BOOST_LOG_SEV(lg, severity) << message;
}

} // Audio
} // GJ