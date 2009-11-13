#include <iostream>
#include <pulse/pulseaudio.h>
#include <unistd.h>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <sys/time.h>
#include <unistd.h>

// callbacks

pa_stream * globalstream; 
int pos = 0; 
static float lastval = 0.0; 

boost::mutex samples_mutex; 

std::list<float> samples; 

void stream_success_callback(pa_stream * stream, int success, void * userdata) 
{
  std::cout << "stream_success_callback" << std::endl; 
  pa_stream_update_timing_info(stream, NULL, NULL); 
}
 
void stream_write_callback(pa_stream * stream, size_t nbytes, void * userdata) 
{
  
  pa_usec_t usec = 0; 
  int neg; 
  pa_stream_update_timing_info(stream, NULL, NULL); 
  int ret = pa_stream_get_latency(stream, &usec, &neg); 
  
  if (pos % 60 == 0) { 
    std::cout << "stream_write_callback "
	      << nbytes / 4 << " samples requested, "; 
    std::cout << " latency = " << usec << " usec" << " " << ret << std::endl; 
  }
  void *data;

  const int SIZEN = nbytes/sizeof(float); 
  
  data = pa_xmalloc(SIZEN * sizeof(float));
  float * fdata  = (float *) data; 

//   for (int i = 0; i < SIZEN; i++) { 
//     boost::mutex::scoped_lock lock(samples_mutex); 
//     if (!samples.empty()) {
//       fdata[i] = samples.front(); 
//       lastval = samples.front(); 
//       samples.pop_front(); 
//     } else {
//       std::cout << "oops! " << std::endl; 
//       fdata[i] = lastval; 
//     }
    
//   }
  float * sdata = (float*)data; 
  double fs = 32000; 
  double rate = 550.0; 
  
  
  for(int i = 0; i < SIZEN; i++) {
    float val; 
    float time = pos / fs; 
    val = sin(time * rate * 3.14 * 2); 
    
    sdata[i] = val;     
    pos++; 
    
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
  std::cout << "stream_state_callback" << std::endl; 


}
 
void stream_event_callback(pa_stream * stream, const char * name, 
			   pa_proplist * pl, void * userdata) 
{
  std::cout << "stream_event_callback" << std::endl; 
  
}


void context_state_callback(pa_context * c, void * userdata)
{
  switch (pa_context_get_state(c)) {
  case PA_CONTEXT_CONNECTING:
    std::cout << "Connecting..." << std::endl; 
    break; 
    
  case PA_CONTEXT_AUTHORIZING:
    std::cout << "Authorizing..." << std::endl; 
    break; 

  case PA_CONTEXT_SETTING_NAME:
    std::cout << "Setting name" << std::endl; 
    break;
    
  case PA_CONTEXT_READY: {
    
    std::cout <<  "Connection established." << std::endl; 
    
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
    
    globalstream = pa_stream_new(c, "stream name", 
				 &samplespec, &channelmap); 
    pa_stream_set_state_callback(globalstream, stream_state_callback, NULL); 
    pa_stream_set_write_callback(globalstream, stream_write_callback, NULL); 
    //pa_stream_set_started_callback(globalstream, stream_notify_callback, NULL); 
    pa_stream_set_event_callback(globalstream, stream_event_callback, NULL); 
    
    pa_buffer_attr bufferattr; 
    bufferattr.maxlength = (uint32_t) -1; 
    bufferattr.tlength = pa_usec_to_bytes(20000, &samplespec) ; // for playback
    bufferattr.prebuf = (uint32_t) -1; 
    bufferattr.minreq = (uint32_t)-1; // pa_usec_to_bytes(10000, &samplespec) /2; 
    bufferattr.fragsize = (uint32_t) -1; 
    
    pa_stream_connect_playback(globalstream,
			       NULL, 
			       &bufferattr,
			       PA_STREAM_ADJUST_LATENCY, 
			       //(pa_stream_flags_t)0, 
			       NULL, 
			       NULL); 
  
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
  
  std::cout << "context notify callback " << long(userdata)  << std::endl; 
}

void context_success_callback(pa_context * c, int success, void * userdata)
{

}

void context_event_callback(pa_context * c, const char * name, pa_proplist * p, 
			    void * userdata)
{

  std::cout << "context event callback" << std::endl; 

}

// separate thread for buffers

float get_time_delta(const timeval  & t1, const  timeval & t2)
{
  float secs = t2.tv_sec - t1.tv_sec; 
  
  float usecs = t2.tv_usec - t1.tv_usec; 
  
  return secs + usecs/1e6; 
}

void worker_thread()
{

  int pos = 0; 
  double fs = 32000; 
  double rate = 550.0; 

  timeval lasttime, now; 
  gettimeofday(&lasttime, NULL); 
  float waitingtime = 0; 
  while(true) { 
    gettimeofday(&now, NULL); 
    float delta = get_time_delta(lasttime, now); 
    waitingtime += delta; 
    while(waitingtime > 1./fs) {
      
      float t = pos * 1./fs; 
      float val = sin(t * rate * 3.14 * 2); 
      
      pos++; 

      waitingtime -= 1./fs; 
      boost::mutex::scoped_lock lock(samples_mutex); 
      samples.push_back(val); 

    }
    lasttime.tv_sec = now.tv_sec; 
    lasttime.tv_usec = now.tv_usec;
  }
  //   for(int i = 0; i < SIZEN; i++) {
//     float val; 
//     float time = pos / fs; 
//     val = sin(time * rate * 3.14 * 2); 
    
//     sdata[i] = val;     
//     pos++; 
    
//   }
  



}; 


int main()
{
  // set scheduling
//   sched_param params; 
//   params.sched_priority = 99; 

//   int ret = sched_setscheduler(0, SCHED_RR, &params);
  
//   if (ret != 0) { 
//     if (errno == EPERM) { 
//       std::cerr << "Process lacked permissions to change priority" << std::endl; 
//       return -1; 
      
//     }
//     if (errno == EINVAL) { 
//       std::cerr << "Invalid sched policy" << std::endl; 
//       return -1; 
      
//     }
//     if (errno == ESRCH) { 
//       std::cerr << "Could not find PID" << std::endl; 
//       return -1; 
      
//     }
//     return -1; 
//   }
  
  pa_mainloop * mainloop = pa_mainloop_new(); 
  pa_mainloop_api * api = pa_mainloop_get_api(mainloop); 

  pa_context *  context = pa_context_new(api, "Name of the context"); 
  pa_context_set_state_callback(context, context_state_callback, 0); 
  pa_context_set_event_callback(context, context_event_callback, NULL); 
  
  pa_context_connect(context, NULL, (pa_context_flags)0, NULL); 

  
  //boost::thread thread(worker_thread); 
  std::cout << "Hello world" << std::endl; 
  int x; 
  pa_mainloop_run(mainloop, &x); 

  pa_stream_disconnect(globalstream); 
  pa_stream_unref(globalstream); 
  pa_mainloop_quit(mainloop, 0); 



}
