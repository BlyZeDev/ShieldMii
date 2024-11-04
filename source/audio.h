#ifndef AUDIO_H
#define AUDIO_H

bcstm* loadBcstm(const char* path);
void playBcstm(bcstm* audioPtr);
void stopBcstm(bcstm* audioPtr);
void freeBcstm(bcstm** audioPtrPtr);

#endif