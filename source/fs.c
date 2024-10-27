#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"

static FS_Archive sdmc;

void initFs()
{
    fsInit();

    FSUSER_OpenArchive(&sdmc, ARCHIVE_SDMC, fsMakePath(PATH_ASCII, ""));

    FSUSER_CreateDirectory(sdmc, fsMakePath(PATH_ASCII, "/config"), FS_ATTRIBUTE_DIRECTORY);
    FSUSER_CreateDirectory(sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE), FS_ATTRIBUTE_DIRECTORY);
    FSUSER_CreateDirectory(sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE "/data"), FS_ATTRIBUTE_DIRECTORY);
}

void writeError(const char* text)
{
    Handle fileHandle;
    u32 bytesWritten;

    FSUSER_DeleteFile(sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE "/log.txt"));

    FSUSER_OpenFile(&fileHandle, sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE "/log.txt"), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);

    FSFILE_Write(fileHandle, &bytesWritten, 0, text, strlen(text), FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
    
    FSFILE_Close(fileHandle);
}

Result readMiiFile(const u32 id, storage* data) //Work
{
    Handle fileHandle;

    const size_t length = 29 + strlen(APP_TITLE);

    char path[length];
    snprintf(path, length, "/config/%s/data/%lu.bin", APP_TITLE, id);

    Result result = FSUSER_OpenFile(&fileHandle, sdmc, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, FS_ATTRIBUTE_HIDDEN);
    if (R_FAILED(result)) return result;

    u32 bytesRead;
    u64 offset = 0;
    FSFILE_Read(fileHandle, &bytesRead, offset, &data->passcodeLength, sizeof(u8));
    offset += sizeof(u8);
    FSFILE_Read(fileHandle, &bytesRead, offset, data->passcode, data->passcodeLength * sizeof(u8));
    offset += data->passcodeLength * sizeof(u8);
    FSFILE_Read(fileHandle, &bytesRead, offset, &data->entryCount, sizeof(u16));
    offset += sizeof(u16);

    data->entries = (entry*)malloc(data->entryCount * sizeof(entry));

    for (size_t i = 0; i < data->entryCount; i++)
    {
        FSFILE_Read(fileHandle, &bytesRead, offset, data->entries[i].name, MAX_PASSNAME_LENGTH + sizeof(char));
        offset += MAX_PASSNAME_LENGTH + sizeof(char);
        FSFILE_Read(fileHandle, &bytesRead, offset, data->entries[i].password, MAX_PASSWORD_LENGTH + sizeof(char));
        offset += MAX_PASSWORD_LENGTH + sizeof(char);
    }

    FSFILE_Close(fileHandle);
    return 0;
}

Result writeMiiFile(const u32 id, const storage* data) //Work
{
    Handle fileHandle;
    
    const size_t length = 29 + strlen(APP_TITLE);
    char path[length];
    snprintf(path, length, "/config/%s/data/%lu.bin", APP_TITLE, id);

    Result result = FSUSER_OpenFile(&fileHandle, sdmc, fsMakePath(PATH_ASCII, path), FS_OPEN_WRITE | FS_OPEN_CREATE, FS_ATTRIBUTE_HIDDEN);
    if (R_FAILED(result)) return result;

    u32 bytesWritten;
    u64 offset = 0;
    FSFILE_Write(fileHandle, &bytesWritten, offset, &data->passcodeLength, sizeof(u8), FS_WRITE_FLUSH);
    offset += sizeof(u8);
    FSFILE_Write(fileHandle, &bytesWritten, offset, data->passcode, data->passcodeLength * sizeof(u8), FS_WRITE_FLUSH);
    offset += data->passcodeLength * sizeof(u8);
    FSFILE_Write(fileHandle, &bytesWritten, offset, &data->entryCount, sizeof(u16), FS_WRITE_FLUSH);
    offset += sizeof(u16);

    for (size_t i = 0; i < data->entryCount; i++)
    {
        FSFILE_Write(fileHandle, &bytesWritten, offset, data->entries[i].name, MAX_PASSNAME_LENGTH + sizeof(char), FS_WRITE_FLUSH);
        offset += MAX_PASSNAME_LENGTH + sizeof(char);
        FSFILE_Write(fileHandle, &bytesWritten, offset, data->entries[i].password, MAX_PASSWORD_LENGTH + sizeof(char), FS_WRITE_FLUSH);
        offset += MAX_PASSWORD_LENGTH + sizeof(char);
    }

    FSFILE_Close(fileHandle);
    return 0;
}

void exitFs()
{
    FSUSER_CloseArchive(sdmc);
    fsExit();
}