/***
  This file is part of PulseAudio.

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <iostream>

int main(int argc, char*argv[]) {

  int rate = 32000; 
  
  /* The Sample format to use */
  pa_sample_spec ss; 
  ss.format = PA_SAMPLE_FLOAT32LE; 
  ss.rate = rate; 
  ss.channels = 1; 
  
  pa_simple *s = NULL;
  int ret = 1;
  int error;
  
  
  
  /* Create a new playback stream */
  if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
    fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    //goto finish;
  }
  
  
  ssize_t r;
  
  int towrite = 1000000; 
  unsigned int pos = 0; 
  float freq = 880.0; 
  float FS = rate;

  const int BUFSIZE=4000; 
  float buf[BUFSIZE];


  pa_usec_t latency;


  while (towrite > 0) { 
    // write the buffer
    for (int i = 0; i < BUFSIZE; i++) {
      float t = float(pos) * 1./FS; 
      buf[i] = 1.0 * sin(t * 3.14 * 2 * freq); 
      //std::cout << i << std::endl; 
      //buf[i*2+1] = sin(t * 3.14 * 2 * freq); 
      pos++; 
    }
    std::cout << "sending a buffer of size " << BUFSIZE << std::endl; 
    //std::cout << "hello world" << std::endl; 
    int ret = pa_simple_write(s, buf, BUFSIZE * sizeof(float), &error); 
    std::cout << "ret = " << ret << std::endl; 

    if ((latency = pa_simple_get_latency(s, &error)) == (pa_usec_t) -1) {
      
    }
    std::cout << "latency is " << float(latency) << std::endl; 
    
    towrite -= BUFSIZE; 
  }
	
  /* Make sure that every single sample was played */
  if (pa_simple_drain(s, &error) < 0) {
    fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
    //goto finish;

  }
  
  ret = 0;
  
  if (s)
    pa_simple_free(s);
  
  return ret;
}
