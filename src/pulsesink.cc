#include <boost/foreach.hpp>

#include "logging.h"
#include "pulsesink.h"

using namespace boost::logging; 

void stream_success_callback(pa_stream * stream, int success, void * userdata) 
{
  PulseSink * pa = (PulseSink*)userdata; 

  AML_(debug) << "PulseSink: stream_success_callback"; 
  pa_stream_update_timing_info(pa->pa_stream_, NULL, NULL); 
  
}
 
void stream_write_callback(pa_stream * stream, size_t nbytes, void * userdata) 
{
  PulseSink * pa = (PulseSink*)userdata; 

  pa_usec_t usec = 0; 
  int neg; 
  pa_stream_update_timing_info(stream, NULL, NULL); 
  int ret = pa_stream_get_latency(stream, &usec, &neg); 
  
  if (pa->pos_ % 60 == 0) { 
    AML_(info) << "PulseSink latency = " << usec 
	      << " usec" << " " << ret ; 
    AML_(info) << "PulseSink Buffer size : "
	       << pa->samples_in_buffer_;
  }
  pa->pos_++; 
  void *data;

  const int SIZEN = nbytes/sizeof(float); 
  
  data = pa_xmalloc(SIZEN * sizeof(float));
  float * fdata  = (float *) data; 

  //boost::mutex::scoped_lock lock(pa->samples_mutex_);   
  float lastval = 0; 
  for(int i = 0; i < SIZEN; ++i) { 
    if (pa->samples_.empty()) { 
      // std::cout << "latency whoops" ; 

      fdata[i] = lastval; 
    } else { 
      float val; 
      pa->samples_.dequeue(&val); 
      pa->samples_in_buffer_--; 

      fdata[i] = val * pa->volume_; 
      lastval = val;       
    }
  }
 
  pa_stream_write(stream, data, SIZEN * sizeof(float), pa_xfree, 0, PA_SEEK_RELATIVE); 
//     if (readf_function) {
//         size_t k = pa_frame_size(&sample_spec);

//         if ((bytes = readf_function(sndfile, data, (sf_count_t) (length/k))) > 0)
//             bytes *= (sf_count_t) k;

//     } else
//         bytes = sf_read_raw(sndfile, data, (sf_count_t) length);

//     if (bytes > 0)
//         pa_stream_write(s, data, (size_t) bytes, pa_xfree, 0, PA_SEEK_RELATIVE);
//     else
//         pa_xfree(data);

//     if (bytes < (sf_count_t) length) {
//         sf_close(sndfile);
//         sndfile = NULL;
//         pa_operation_unref(pa_stream_drain(s, stream_drain_complete, NULL));
//     }

}
 
void stream_state_callback(pa_stream * stream, void * userdata) 
{
  PulseSink * pa = (PulseSink*)userdata; 

  AML_(debug) << "stream_state_callback" ; 


}
 
void stream_event_callback(pa_stream * stream, const char * name, 
			   pa_proplist * pl, void * userdata) 
{
  PulseSink * pa = (PulseSink*)userdata; 
  AML_(debug) << "stream_event_callback" ; 
  
}


void context_state_callback(pa_context * c, void * userdata)
{
  PulseSink * pa = (PulseSink*)userdata; 
  switch (pa_context_get_state(c)) {
  case PA_CONTEXT_CONNECTING:
    AML_(debug) << "PulseSink: Connecting..." ; 
    break; 
    
  case PA_CONTEXT_AUTHORIZING:
    AML_(debug) << "PulseSink: Authorizing..." ; 
    break; 

  case PA_CONTEXT_SETTING_NAME:
    AML_(debug) << "PulseSink: Setting name" ; 
    break;
    
  case PA_CONTEXT_READY: {
    
    AML_(debug) <<  "PulseSink: Connection established." ; 
    
//     stream = pa_stream_new(c, stream_name, &sample_spec, channel_map_set ? &channel_map : NULL);
//     assert(stream);
    
//     pa_stream_set_state_callback(stream, stream_state_callback, NULL);
//     pa_stream_set_write_callback(stream, stream_write_callback, NULL);
//    pa_stream_connect_playback(stream, device, NULL, 0, pa_cvolume_set(&cv, sample_spec.channels, volume), NULL);

    pa_sample_spec samplespec; 
    samplespec.format = PA_SAMPLE_FLOAT32LE ; 
    samplespec.rate = 32000; 
    samplespec.channels = 1; 
    
    pa_channel_map channelmap; 
    pa_channel_map_init_mono(&channelmap); 
    
    pa->pa_stream_ = pa_stream_new(c, "stream name", 
				 &samplespec, &channelmap); 
    pa_stream_set_state_callback(pa->pa_stream_, stream_state_callback, userdata); 
    pa_stream_set_write_callback(pa->pa_stream_, stream_write_callback, userdata); 
    //pa_stream_set_started_callback(pa->pa_stream_, stream_notify_callback, NULL); 
    pa_stream_set_event_callback(pa->pa_stream_, stream_event_callback, userdata); 
    
    bool low_latency = false ; 
    if (low_latency) { 
      pa_buffer_attr bufferattr; 
      bufferattr.maxlength = (uint32_t) -1; 
      bufferattr.tlength = pa_usec_to_bytes(20000, &samplespec) ; // for playback
      bufferattr.prebuf = (uint32_t) -1; 
      bufferattr.minreq = (uint32_t)-1; // pa_usec_to_bytes(10000, &samplespec) /2; 
      bufferattr.fragsize = (uint32_t) -1; 
    
      pa_stream_connect_playback(pa->pa_stream_,
				 NULL, 
				 &bufferattr,
				 PA_STREAM_ADJUST_LATENCY, 
			       //(pa_stream_flags_t)0, 
				 NULL, 
				 NULL); 
    } else {

      pa_stream_connect_playback(pa->pa_stream_,
				 NULL, 
				 NULL, 
				 (pa_stream_flags_t)0, 
				 NULL, 
				 NULL); 

    }
 
  //pa_stream_trigger(stream, stream_succes_scallback, NULL); 
    
    break;
  }
    
  case PA_CONTEXT_TERMINATED:
    //quit(0);
    break;
    
  case PA_CONTEXT_FAILED:
  default:
    //fprintf(stderr, ("Connection failure: %s\n"), pa_strerror(pa_context_errno(c)));
    //quit(1);
    break;
  }
  
  AML_(debug) << "PulseSink: context notify callback " << long(userdata)  ; 
}

void context_success_callback(pa_context * c, int success, void * userdata)
{
  PulseSink * pa = (PulseSink*)userdata; 

}

void context_event_callback(pa_context * c, const char * name, pa_proplist * p, 
			    void * userdata)
{
  PulseSink * pa = (PulseSink*)userdata; 

  AML_(debug) << "PulseSink: context event callback" ; 

}



PulseSink::PulseSink(pa_mainloop_api * ml):
  pos_(0), 
  pa_mainloop_api_(ml), 
  isGLibMainloop_(0),
  volume_(1.0), 
  samples_in_buffer_(0)
{
  // do stuff
  pa_context_ = pa_context_new(pa_mainloop_api_,
					 "Soma Audio Monitor"); 
  pa_context_set_state_callback(pa_context_, context_state_callback, this); 
  pa_context_set_event_callback(pa_context_, context_event_callback, this); 
  
  pa_context_connect(pa_context_, NULL, (pa_context_flags)0, 0); 


}

PulseSink::~PulseSink()
{
  if(isGLibMainloop_) { 
    pa_glib_mainloop_free(pa_glib_mainloop_); 
  } else { 
    pa_mainloop_free(pa_mainloop_); 
  }

}


void PulseSink::setVolume(float v) {
  volume_ = v; 
}


void PulseSink::run() {

  int x; 
  if (!isGLibMainloop_) 
    pa_mainloop_run(pa_mainloop_, &x); 
  
}

void PulseSink::shutdown() 
{

  pa_stream_disconnect(pa_stream_); 
  pa_stream_unref(pa_stream_); 
  if (!isGLibMainloop_) 
    pa_mainloop_quit(pa_mainloop_, 0); 
}


void PulseSink::addSamples(const std::vector<float> & val)
{

  //boost::mutex::scoped_lock lock(samples_mutex_);   
  BOOST_FOREACH(float x, val) { 
    while(!samples_.enqueue(x)) {
    }
    samples_in_buffer_++; 
  }  
  
}


PulseSink * PulseSink::createWithMainloop()
{
  pa_mainloop * pml = pa_mainloop_new(); 
  PulseSink *  ps = new PulseSink(pa_mainloop_get_api(pml)); 
  ps->pa_mainloop_ = pml; 
  ps->isGLibMainloop_ = false; 
  return ps; 
}


PulseSink * PulseSink::createWithGLibMainloop(GMainContext * context)
{
  pa_glib_mainloop * pgml = pa_glib_mainloop_new(context); 
  PulseSink *  ps = new PulseSink(pa_glib_mainloop_get_api(pgml)); 
  ps->pa_glib_mainloop_ = pgml; 
  ps->isGLibMainloop_ = true; 
  return ps; 
}

int PulseSink::samplesInBuffer()
{
  return samples_in_buffer_; 


}
