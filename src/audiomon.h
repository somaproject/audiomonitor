#ifndef __AUDIOMON_AUDIOMON_H__
#define __AUDIOMON_AUDIOMON_H__

#include <somanetwork/networkinterface.h>
#include "audiosink.h"
#include <gtkmm.h>

namespace sn = somanetwork; 

/* 
   GLIB-based soma audio monitor core. 
   
   Assumes ownership of the network interface, will call run
   and shutdown on it. 
   
   
 */
 
class AudioMonitor
{
public:
  AudioMonitor(somanetwork::pNetworkInterface_t ni, IAudioSink * sink); 
  ~AudioMonitor(); 
  
  void setSource(sn::eventsource_t src); 
  void setChannel(int channum); 

  /*
    Downs network interface, 
  */ 
  void pause(); 
  void play(); 
  
  // signals: Source enabled, source disabled, etc. 

  static const sn::eventcmd_t CMD_AUDIOBCAST = 0x18; 
  static const sn::eventcmd_t CMD_AUDIOCOMMAND_TX = 0x30; 

  // Channel signal is emitted every time status info changes
  sigc::signal<void, bool, int, int> & statusSignal(); 
  

private:
  sn::pNetworkInterface_t pNetwork_; 
  IAudioSink * pSink_; 
  void enableSource(sn::eventsource_t src); 
  void disableSource(sn::eventsource_t src); 
  
  bool paused_; 
  sn::eventsource_t src_; 
  int channel_; 
  int samplingrate_; 


  bool event_rx_callback(Glib::IOCondition io_condition); 
  void processInboundEvents(const sn::pEventList_t & events); 

  void newAudioSamplesEvent(const sn::Event_t evt); 
  void newAudioStateEvent(const sn::Event_t evt); 
  
  sigc::signal<void, bool, int, int> statussignal_; 


}; 


#endif 
