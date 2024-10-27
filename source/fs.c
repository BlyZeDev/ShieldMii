#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"

static FS_Archive sdmc;

static void writeTxtError(const char* text)
{
    Handle fileHandle;
    u32 bytesWritten;

    FSUSER_DeleteFile(sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE "/log.txt"));

    FSUSER_OpenFile(&fileHandle, sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE "/log.txt"), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);

    FSFILE_Write(fileHandle, &bytesWritten, 0, text, strlen(text), FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
    
    FSFILE_Close(fileHandle);
}

static Result prepareMiiFileOp(Handle* handle, const u32 id, const u32 flags, const bool delete)
{
    const size_t length = 29 + strlen(APP_TITLE);

    char path[length];
    snprintf(path, length, "/config/%s/data/%lu.bin", APP_TITLE, id);

    if (delete) FSUSER_DeleteFile(sdmc, fsMakePath(PATH_ASCII, path));

    return FSUSER_OpenFile(handle, sdmc, fsMakePath(PATH_ASCII, path), flags, FS_ATTRIBUTE_HIDDEN);
}

void initFs()
{
    fsInit();

    FSUSER_OpenArchive(&sdmc, ARCHIVE_SDMC, fsMakePath(PATH_ASCII, ""));

    FSUSER_CreateDirectory(sdmc, fsMakePath(PATH_ASCII, "/config"), FS_ATTRIBUTE_DIRECTORY);
    FSUSER_CreateDirectory(sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE), FS_ATTRIBUTE_DIRECTORY);
    FSUSER_CreateDirectory(sdmc, fsMakePath(PATH_ASCII, "/config/" APP_TITLE "/data"), FS_ATTRIBUTE_DIRECTORY);
}

void writeError(Result result)
{
    result = MAKERESULT(R_LEVEL(result), R_SUMMARY(result), R_MODULE(result), R_DESCRIPTION(result));

    char resultStr[24];
    snprintf(resultStr, 24, "Error code: %ld", result);

    writeTxtError(resultStr);
}

bool readMiiFile(const u32 id, storage* data)
{
    Handle fileHandle;

    Result result = prepareMiiFileOp(&fileHandle, id, FS_OPEN_READ, false);
    if (R_FAILED(result)) return false;

    u32 bytesRead;
    u64 offset = 0;
    FSFILE_Read(fileHandle, &bytesRead, offset, &data->passcodeHash, sizeof(u64));
    offset += sizeof(u64);
    FSFILE_Read(fileHandle, &bytesRead, offset, &data->entryCount, sizeof(u16));
    offset += sizeof(u16);

    data->entries = (entry*)malloc(data->entryCount * sizeof(entry));

    for (size_t i = 0; i < data->entryCount; i++)
    {
        FSFILE_Read(fileHandle, &bytesRead, offset, data->entries[i].name, MAX_PASSNAME_LENGTH * sizeof(char));
        offset += MAX_PASSNAME_LENGTH * sizeof(char);
        FSFILE_Read(fileHandle, &bytesRead, offset, data->entries[i].password, MAX_PASSWORD_LENGTH * sizeof(char));
        offset += MAX_PASSWORD_LENGTH * sizeof(char);
    }

    FSFILE_Close(fileHandle);
    return true;
}

bool writeMiiFile(const u32 id, const storage* data)
{
    Handle fileHandle;

    Result result = prepareMiiFileOp(&fileHandle, id, FS_OPEN_WRITE | FS_OPEN_CREATE, true);
    if (R_FAILED(result))
    {
        writeError(result);
        return false;
    }

    u32 bytesWritten;
    u64 offset = 0;
    FSFILE_Write(fileHandle, &bytesWritten, offset, &data->passcodeHash, sizeof(u64), FS_WRITE_FLUSH);
    offset += sizeof(u64);
    FSFILE_Write(fileHandle, &bytesWritten, offset, &data->entryCount, sizeof(u16), FS_WRITE_FLUSH);
    offset += sizeof(u16);

    for (size_t i = 0; i < data->entryCount; i++)
    {
        FSFILE_Write(fileHandle, &bytesWritten, offset, data->entries[i].name, MAX_PASSNAME_LENGTH * sizeof(char), FS_WRITE_FLUSH);
        offset += MAX_PASSNAME_LENGTH * sizeof(char);
        FSFILE_Write(fileHandle, &bytesWritten, offset, data->entries[i].password, MAX_PASSWORD_LENGTH * sizeof(char), FS_WRITE_FLUSH);
        offset += MAX_PASSWORD_LENGTH * sizeof(char);
    }

    FSFILE_Close(fileHandle);
    return true;
}

void exitFs()
{
    FSUSER_CloseArchive(sdmc);
    fsExit();
}