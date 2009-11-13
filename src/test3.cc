#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <iostream>
#include <string>
#include <math.h>

main (int argc, char *argv[])
{
  int i;
  int err;
  snd_pcm_t *playback_handle;
  snd_pcm_hw_params_t *hw_params;
  
  std::string devname = "default"; 

  if ((err = snd_pcm_open (&playback_handle, devname.c_str(),
			   SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
	     argv[1],
	     snd_strerror (err));
    exit (1);
  }
  
//   if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
//     fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
//     fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
//     fprintf (stderr, "cannot set access type (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_FLOAT_LE)) < 0) {
//     fprintf (stderr, "cannot set sample format (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   unsigned int rate = 44100;  
//   if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0) {
//     fprintf (stderr, "cannot set sample rate (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
//   std::cout << "The exact rate is" << rate <<  std::endl; 
  
//   if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2)) < 0) {
//     fprintf (stderr, "cannot set channel count (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
//     fprintf (stderr, "cannot set parameters (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   snd_pcm_hw_params_free (hw_params);
  unsigned int rate = 32000; 
  if ((err = snd_pcm_set_params(playback_handle,
				SND_PCM_FORMAT_FLOAT_LE,
				SND_PCM_ACCESS_RW_INTERLEAVED,
				1,
				rate, 
				1,
	                              500000)) < 0) {	/* 0.5sec */
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }


	
//   if ((err = snd_pcm_prepare (playback_handle)) < 0) {
//     fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
// 	     snd_strerror (err));
//     exit (1);
//   }
  
//   for (i = 0; i < 1000; ++i) {
//     if ((err = snd_pcm_writei (playback_handle, buf, 128)) != 128) {
//       std::cout << "we really wrote" << err << std::endl; 
//       fprintf (stderr, "write to audio interface failed (%s)\n",
// 	       snd_strerror (err));
//       exit (1);
//     }
//   }
  int towrite = 1000000; 
  unsigned int pos = 0; 
  float freq = 880.0; 
  float FS = rate;

  const int BUFSIZE=4096; 
  float buf[BUFSIZE];

  while (towrite > 0) { 
    // write the buffer
    for (int i = 0; i < BUFSIZE/2; i++) {
      float t = float(pos) * 1./FS; 
      buf[i*2] = sin(t * 3.14 * 2 * freq); 
      buf[i*2+1] = sin(t * 3.14 * 2 * freq); 
      pos++; 
    }
    //std::cout << "hello world" << std::endl; 
    while((err = snd_pcm_writei(playback_handle, buf, BUFSIZE)) < 0) {
      std::cout << "whoa?" << std::endl;
      snd_pcm_prepare(playback_handle); 
    }
    towrite -= 128; 
  }
  snd_pcm_close (playback_handle);
  exit (0);
}
