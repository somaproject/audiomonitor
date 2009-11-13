#include <boost/foreach.hpp>

#include "audiomon.h"

AudioMonitor::AudioMonitor(somanetwork::pNetworkInterface_t ni, IAudioSink * sink) :
  pNetwork_(ni), 
  pSink_(sink), 
  paused_(false)
{

  int read_fd = pNetwork_->getEventFifoPipe(); 
  
  Glib::signal_io().connect(sigc::mem_fun(*this, &AudioMonitor::event_rx_callback),
			    read_fd, Glib::IO_IN);

  pNetwork_->run(); 
 
  
}

AudioMonitor::~AudioMonitor() {


  pNetwork_->shutdown(); 
}
  
void AudioMonitor::setSource(sn::eventsource_t src) {
  /*
    Disable the current audio source we are connected to 
    Enable the new source . 

    FIXME: multiple audio monitors thus may fight to enable/disable
    other channels. We should fix this. 
    
  */ 
  disableSource(src_); 
  enableSource(src); 
  src_ = src; 
  
}


void AudioMonitor::pause() 
{
  paused_ = true;  

}

void AudioMonitor::play()
{
  paused_ = false; 
  
}
  
void  AudioMonitor::enableSource(sn::eventsource_t src)
{
  // FIXME log
  
  sn::EventTXList_t etxl; 

  sn::EventTX_t etx; 
  etx.destaddr[src_] = true; 
  etx.event.cmd = CMD_AUDIOCOMMAND_TX; 
  etx.event.data[0] = 1; 
  etx.event.data[1] = 1;
  etx.event.data[2] = 0; 

  etxl.push_back(etx); 

  pNetwork_->sendEvents(etxl); 
  

}

void  AudioMonitor::disableSource(sn::eventsource_t src)
{
  // fixme log
  sn::EventTXList_t etxl; 

  sn::EventTX_t etx; 
  etx.destaddr[src_] = true; 
  etx.event.cmd = CMD_AUDIOCOMMAND_TX; 
  etx.event.data[0] = 1; 
  etx.event.data[1] = 0; // always on

  etxl.push_back(etx); 

  pNetwork_->sendEvents(etxl); 

}
  

bool AudioMonitor::event_rx_callback(Glib::IOCondition io_condition)
{

  if ((io_condition & Glib::IO_IN) == 0) {
    // FIXME log
    std::cerr << "Invalid fifo response" << std::endl;
  }
  else 
    {
      char x; 
      read(pNetwork_->getEventFifoPipe(), &x, 1); 
      
      sn::pEventPacket_t rdp = pNetwork_->getNewEvents(); 
      processInboundEvents(rdp->events); 

    }
  
  return true; 
  
}

void AudioMonitor::processInboundEvents(const sn::pEventList_t & events)
{
  BOOST_FOREACH(sn::Event_t & e, *events) {
    // log all event status messages from all sources
    if (e.cmd == CMD_AUDIOBCAST) { 
      if (e.src == src_) { 
	if (e.data[0] == 0)  {
	  newAudioStateEvent(e); 
	} else { 
	  newAudioSamplesEvent(e); 
	} 
	// FIXME : we should log the others anyway

      } else {
	// from someone else, just log 
      }
    } 
  }
  
}

void AudioMonitor::newAudioSamplesEvent(const sn::Event_t evt)
{
  /* 
     New samples have arrived, put them in sink
     
   */ 
  if (paused_) {
    return; 
  }
  
  std::vector<float> samples(4) ; 
  for(int i = 0; i < 4; i++) {
    int16_t val = (int16_t)evt.data[i+1]; 
    samples[i] = float(val)/ (1<<15); 
  }
  
  pSink_->addSamples(samples); 
  
}

void AudioMonitor::newAudioStateEvent(const sn::Event_t evt) 
{
  /*
    we assume it is for us at this point
    
  */ 
  bool is_on; 

  if(evt.data[1] == 0) { 
    is_on = false; 
  } else {
    is_on = true; 
  }

  channel_ = evt.data[2]; 
  samplingrate_ = evt.data[3]; 
  statussignal_.emit(is_on, channel_, samplingrate_); 
  
}
  
sigc::signal<void, bool, int, int> & AudioMonitor::statusSignal()
{
  return statussignal_;   

}

void AudioMonitor::setChannel(int channum)
{
  sn::EventTXList_t etxl; 

  sn::EventTX_t etx; 
  etx.destaddr[src_] = true; 
  etx.event.cmd = CMD_AUDIOCOMMAND_TX; 
  etx.event.data[0] = 1; 
  etx.event.data[1] = 1; // always on
  etx.event.data[2] = channum; 

  etxl.push_back(etx); 

  pNetwork_->sendEvents(etxl); 
  
}


