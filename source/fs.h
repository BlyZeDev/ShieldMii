#ifndef FS_H
#define FS_H

void initFs(void);
void writeError(Result result);
bool readMiiFile(const u32 id, storage* data);
bool writeMiiFile(const u32 id, const storage* data);
void exitFs(void);

#endif