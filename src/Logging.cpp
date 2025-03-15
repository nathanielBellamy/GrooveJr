//
// Created by ns on 3/13/25.
//

#include "Logging.h"

namespace Gj {

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(Logger, boost::log::sources::severity_logger<LogSeverityLevel>);

void Logging::init() {
  boost::log::register_simple_formatter_factory< LogSeverityLevel, char >("Severity");
  boost::log::register_simple_formatter_factory< Audio::LogSeverityLevel, char >("AudioSeverity");
  boost::log::add_file_log(
      boost::log::keywords::file_name = "/Users/ns/code/GrooveJr/logs/groove_jr_%N.log",
      boost::log::keywords::rotation_size = 10 * 1024 * 1024,
      boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "[%TimeStamp%] :: %Severity%%AudioSeverity% :: %Caller% :: %Message%"
  );
  boost::log::add_common_attributes();

  boost::log::sources::severity_logger< LogSeverityLevel > slg;

  boost::log::sources::severity_logger<LogSeverityLevel> lg = Logger::get();
  lg.add_attribute("Caller", boost::log::attributes::constant<std::string>("initLogging"));
  BOOST_LOG_SEV(lg, Info) << "GrooveJr log initialized";

  // boost::log::core::get()->set_filter(
  //     boost::log::trivial::severity >= boost::log::trivial::info
  // );
}

void Logging::write(LogSeverityLevel severity, const std::string& caller, const std::string& message) {
  boost::log::sources::severity_logger<LogSeverityLevel> lg = Logger::get();
  lg.add_attribute("Caller", boost::log::attributes::constant<std::string>(caller));
  BOOST_LOG_SEV(lg, severity) << message;
}

} // GJ