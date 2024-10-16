#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

static FS_Archive archive;

void initFs()
{
    fsInit();

    FSUSER_OpenArchive(&archive, ARCHIVE_SDMC, fsMakePath(PATH_ASCII, ""));

    FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, "/3ds"), FS_ATTRIBUTE_DIRECTORY);
    FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, "/3ds/" APP_TITLE), FS_ATTRIBUTE_DIRECTORY);
}

void writeError(const char* text)
{
    Handle fileHandle;
    u32 bytesWritten;

    FSUSER_DeleteFile(archive, fsMakePath(PATH_ASCII, "/3ds/" APP_TITLE "/logs.txt"));

    FSUSER_OpenFile(&fileHandle, archive, fsMakePath(PATH_ASCII, "/3ds/" APP_TITLE "/logs.txt"), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);

    FSFILE_Write(fileHandle, &bytesWritten, 0, text, strlen(text), FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
    
    FSFILE_Close(fileHandle);
}

void exitFs()
{
    FSUSER_CloseArchive(archive);
    fsExit();
}