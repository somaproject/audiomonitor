#ifndef __AUDIOMON_PULSESINK_H__
#define __AUDIOMON_PULSESINK_H__

#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

#include "audiosink.h"

#include "pulsesink.h"
#include <boost/lockfree/fifo.hpp>


class PulseSink : public IAudioSink
{

public:
  ~PulseSink(); 
  static PulseSink * createWithMainloop(); 
  static PulseSink * createWithGLibMainloop(GMainContext * c); 

  void setVolume(float); 
  void run(); 
  void shutdown(); 
  void addSamples(const std::vector<float> & ); 

  pa_stream * pa_stream_; 

  size_t pos_; 

  /* 
     use a lock-free fifio because you never want to do 
     anything that might block in a core system thread. 


   */ 

  boost::lockfree::fifo<float> samples_; 

private:

  PulseSink(pa_mainloop_api * ml); 

  bool isGLibMainloop_; 
  
  pa_mainloop_api* pa_mainloop_api_; 
  
  pa_mainloop * pa_mainloop_; 
  pa_glib_mainloop * pa_glib_mainloop_; 
  
  
  pa_context * pa_context_; 
  
}; 



#endif
