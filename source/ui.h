#ifndef UI_H
#define UI_H

void initUI();
void startFrame();
void drawWelcomeScreen();
void drawGrid(const circle* circles);
void drawBattery(const u8 percentage, const bool isCharging);
void endFrame();
void exitUI();

#endif