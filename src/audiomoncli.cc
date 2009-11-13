#include <gtkmm.h>
#include "pulsesink.h"
#include <somanetwork/network.h>


#include "audiomon.h"


/*
  Audio monitor test client, CLI only, Pulse Sink only. 

*/ 

AudioMonitor * pam; 

bool timeout_handler2(); 
bool timeout_handler()
{
  std::cout << "Running timeout handler" << std::endl; 
  
  pam->setSource(20); 
  pam->setChannel(2); 
  pam->play(); 

  Glib::signal_timeout().connect(sigc::ptr_fun(&timeout_handler2), 4000);

  
  return false; 
  
}

bool timeout_handler2()
{
  std::cout << "Running timeout handler 2" << std::endl; 
  
  pam->setChannel(1); 

  return false; 
  
}


int main(void)
{
 
  Glib::RefPtr< Glib::MainLoop > pmainloop = Glib::MainLoop::create(); 
  
  

  PulseSink * ps = PulseSink::createWithGLibMainloop(
			   pmainloop->get_context()->gobj()); 

  sn::pNetworkInterface_t ni = sn::Network::createINet("127.0.0.1"); 
  
  pam = new AudioMonitor(ni, ps); 
  

  Glib::signal_timeout().connect(sigc::ptr_fun(&timeout_handler), 1000);

  ps->run(); 


  std::cout << "Running mainloop" << std::endl; 
  pmainloop->run(); 
  
  ps->shutdown(); 
  
  delete pam; 

  return 0; 


}
