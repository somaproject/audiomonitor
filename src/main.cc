#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <somanetwork/network.h>
#include <somanetwork/networkinterface.h>
#include <string>
#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <somanetwork/logging.h>
#include "audiosink.h"
#include "pulsesink.h"
#include "audiomon.h"
#include "audiomongui.h"

namespace po = boost::program_options;

//std::string LOG_ROOT("soma.streams.streamsmain"); 

#include <boost/logging/logging.hpp>
#include <src/logging.h>
#include "logging.h"

using namespace std; 
namespace bf = boost::filesystem; 

namespace sn = somanetwork; 

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);
  
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("soma-ip", po::value<string>(), "The IP of the soma hardware")
    ("enable-log", po::value<string>()->default_value("warning"), "Enable soma DSP IO debugging at this level")
    ("enable-network-log", po::value<string>()->default_value("warning"), "Enable soma network debugging at this level")
    ("force-reference-time", po::value<long>()->default_value(0), "Force the reference timestamp")
    ("audio-sink", po::value<string>()->default_value("pulse"), "Audio sink (pulse, jack)")
    ; 

//   desc.add(logging_desc()); 
    
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  
  if (vm.count("enable-log")) {
    string logstr = vm["enable-log"].as<string>(); 
    if (logstr == "") {
      logstr = "warning"; 
    }
    boost::logging::level::type lt = log_level_parse(logstr); 
    audiomonitor::init_logs(lt);
  }

  
  std::string somaip; 
  if (!vm.count("soma-ip")) {
    AML_(fatal) << "soma-ip was not specified, no way to get data"; 
    return -1; 
  }
  
  somaip = vm["soma-ip"].as<string>(); 

  if (vm.count("enable-network-log")) {
    string logstr = vm["enable-network-log"].as<string>(); 
    if (logstr == "") {
      logstr = "warning"; 
    }
    boost::logging::level::type lt = log_level_parse(logstr); 
    somanetwork::init_logs(lt);  
  }
  


  sn::pNetworkInterface_t pnetwork =  sn::Network::createINet(somaip); 

  IAudioSink * as; 
  
  if(vm["audio-sink"].as<string>() 
     == std::string("pulse")) {
    as = PulseSink::createWithGLibMainloop(g_main_context_default()); 
    
  } else if (vm["audio-sink"].as<string>() 
	     == std::string("jack")) { 


  } else {

    AML_(fatal) << "Unrecognized audio sink, valid values are pulse, jack"
		<< std::endl; 

  }


  AudioMonitor  am(pnetwork, as); 
  
  as->run(); 

  AudioMonGui amgui(am); 

  kit.run(amgui); 
  
  
  as->shutdown(); 


  delete as; 

  return 0;
}

