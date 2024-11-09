#ifndef UI_H
#define UI_H

void initUI(bool setWide);
void startFrame(void);
void drawWelcomeScreen(void);
void drawPasscodeEntry(const circle* circles);
void drawOverlay(const screenOverlay* overlayPtr);
void endFrame(void);
void exitUI(void);

#endif