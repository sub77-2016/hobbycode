/*
 * Class festival text to speech
 * By waipot@ngamsaad.name
 */

#include "tts.h"


TTS::TTS()
{  
  festival_initialize(1, 210000);
}

TTS::TTS(const int load_init_files, const int heap_size)
{
  festival_initialize(load_init_files, heap_size);
}

TTS::~TTS()
{  
}

int TTS::sayFile(const char *filename)
{
  EST_String str(filename);
  return festival_say_file(str);
}

int TTS::evalCommand(const char *expr)
{
  EST_String str(expr);
  return festival_eval_command(str);
}

int TTS::sayText(const char *text)
{
  EST_String str(text);
  return festival_say_text(str);
}

int TTS::textToWave(const char *text, const char *path)
{
  EST_String str(text);
  EST_String dest(path);
  const int flag = festival_text_to_wave(str, wave);
  wave.save(dest, "riff");
  return flag;
}

void TTS::waitForSpooler()
{
  festival_wait_for_spooler();
}
