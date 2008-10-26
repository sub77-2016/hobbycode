#include "tts.h"

int main(int argc, char **argv)
{
  TTS *tts = new TTS(1,210000);

  // Say simple file
  tts->sayFile("/etc/motd");

  // Read and evaluate Lisp command
  tts->evalCommand("(voice_ked_diphone)");

  // Say some text;
  tts->sayText("hello world");

  // Convert to a waveform
  tts->textToWave("hello world", "./sound.wave");

  tts->waitForSpooler();

  return 0;
}
