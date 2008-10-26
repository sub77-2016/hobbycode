/*
 * Class festival text to speech
 * By waipot@ngamsaad.name
 */

#ifndef TTS_H
#define TTS_H

#include <festival.h>

class TTS
{
 public:
  TTS();
  TTS(const int, const int);
  ~TTS(); 

  // Say simple file
  int sayFile(const char *);
  // Read and evaluate Lisp command
  int evalCommand(const char *);
  // Say some text;
  int sayText(const char *);
  // Convert to a waveform
  int textToWave(const char *, const char *);
  // Spooling
  void waitForSpooler();

 private:
  EST_Wave wave;

};

#endif
