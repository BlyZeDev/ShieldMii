#ifndef FS_H
#define FS_H

void initFs();
void writeError(Result result);
bool readMiiFile(const u32 id, storage* data);
bool writeMiiFile(const u32 id, const storage* data);
void exitFs();

#endif