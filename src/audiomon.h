#ifndef __AUDIOMON_AUDIOMON_H__
#define __AUDIOMON_AUDIOMON_H__

#include <boost/filesystem.hpp>

#include <fstream>

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
  void setVolumeScaling(int scale); 

  /*
    Downs network interface, 
  */ 
  void pause(); 
  void play(); 
  
  // signals: Source enabled, source disabled, etc. 

  static const sn::eventcmd_t CMD_AUDIOBCAST = 0x18; 
  static const sn::eventcmd_t CMD_AUDIOCOMMAND_TX = 0x30; 

  // debugging / logging
  void enableEventLogging(boost::filesystem::path path); 
  void disableEventLogging(); 

  
  // Channel signal is emitted every time status info changes
  sigc::signal<void, bool, int, int, int> & statusSignal(); 
  
  sigc::signal<void, float> & peakSignal(); 

  void setVolume(double); 

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
  
  sigc::signal<void, bool, int, int, int> statussignal_; 

  bool isEventLogging_; 
  boost::filesystem::path logfilename_; 
  std::fstream logfstream_; 

  int volscale_; 

  void setRemoteState(bool, int chan, int volscale); 
  
  int16_t recentpeak_; 
  sigc::signal<void, float> peaksignal_; 


}; 


#endif 
