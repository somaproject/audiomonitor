#include "logging.h"
#include <boost/logging/format.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/named_write.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/formatter/high_precision_time.hpp>

  //using namespace boost::logging;
namespace bl = boost::logging; 



BOOST_DEFINE_LOG_FILTER(audiomonitor_log_level, audiomonitor_finder::filter ) // holds the application log level
BOOST_DEFINE_LOG(audiomonitor_l, audiomonitor_finder::logger); 

namespace audiomonitor {
    
  void init_logs( boost::logging::level::type level)  {
    audiomonitor_log_level()->set_enabled(level); 

    audiomonitor_l()->writer().add_formatter(bl::formatter::high_precision_time("$mm.$ss:$micro ")); 
    audiomonitor_l()->writer().add_formatter(bl::formatter::append_newline()); 
    audiomonitor_l()->writer().add_destination( bl::destination::cout() );
    audiomonitor_l()->mark_as_initialized();

  }

}


