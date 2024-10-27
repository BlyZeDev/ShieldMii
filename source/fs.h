#ifndef FS_H
#define FS_H

void initFs();
void writeError(const char* text);
Result readMiiFile(const u32 id, storage* data);
Result writeMiiFile(const u32 id, const storage* data);
void exitFs();

#endif