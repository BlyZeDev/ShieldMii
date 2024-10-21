#ifndef UI_H
#define UI_H

void initUI();
void startFrame();
void drawWelcomeScreen();
void initGrid(circle* circles);
void drawGrid(circle* circles);
void drawBattery(u8 percentage, bool isCharging);
void endFrame();
void exitUI();

#endif