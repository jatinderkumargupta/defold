#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sys.h"
#include "log.h"
#include "dstrings.h"

#ifdef _WIN32
#include <Shlobj.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef __MACH__
#include <Carbon/Carbon.h>
#endif

namespace dmSys
{

    #define DM_SYS_NATIVE_TO_RESULT_CASE(x) case E##x: return RESULT_##x

    static Result NativeToResult(int r)
    {
        switch (r)
        {
            DM_SYS_NATIVE_TO_RESULT_CASE(PERM);
            DM_SYS_NATIVE_TO_RESULT_CASE(NOENT);
            DM_SYS_NATIVE_TO_RESULT_CASE(SRCH);
            DM_SYS_NATIVE_TO_RESULT_CASE(INTR);
            DM_SYS_NATIVE_TO_RESULT_CASE(IO);
            DM_SYS_NATIVE_TO_RESULT_CASE(NXIO);
            DM_SYS_NATIVE_TO_RESULT_CASE(2BIG);
            DM_SYS_NATIVE_TO_RESULT_CASE(NOEXEC);
            DM_SYS_NATIVE_TO_RESULT_CASE(BADF);
            DM_SYS_NATIVE_TO_RESULT_CASE(CHILD);
            DM_SYS_NATIVE_TO_RESULT_CASE(DEADLK);
            DM_SYS_NATIVE_TO_RESULT_CASE(NOMEM);
            DM_SYS_NATIVE_TO_RESULT_CASE(ACCES);
            DM_SYS_NATIVE_TO_RESULT_CASE(FAULT);
            DM_SYS_NATIVE_TO_RESULT_CASE(BUSY);
            DM_SYS_NATIVE_TO_RESULT_CASE(EXIST);
            DM_SYS_NATIVE_TO_RESULT_CASE(XDEV);
            DM_SYS_NATIVE_TO_RESULT_CASE(NODEV);
            DM_SYS_NATIVE_TO_RESULT_CASE(NOTDIR);
            DM_SYS_NATIVE_TO_RESULT_CASE(ISDIR);
            DM_SYS_NATIVE_TO_RESULT_CASE(INVAL);
            DM_SYS_NATIVE_TO_RESULT_CASE(NFILE);
            DM_SYS_NATIVE_TO_RESULT_CASE(MFILE);
            DM_SYS_NATIVE_TO_RESULT_CASE(NOTTY);
#ifndef _WIN32
            DM_SYS_NATIVE_TO_RESULT_CASE(TXTBSY);
#endif
            DM_SYS_NATIVE_TO_RESULT_CASE(FBIG);
            DM_SYS_NATIVE_TO_RESULT_CASE(NOSPC);
            DM_SYS_NATIVE_TO_RESULT_CASE(SPIPE);
            DM_SYS_NATIVE_TO_RESULT_CASE(ROFS);
            DM_SYS_NATIVE_TO_RESULT_CASE(MLINK);
            DM_SYS_NATIVE_TO_RESULT_CASE(PIPE);
        }

        dmLogError("Unknown result code %d\n", r);
        return RESULT_UNKNOWN;
    }
    #undef DM_SYS_NATIVE_TO_RESULT_CASE

    Result Rmdir(const char* path)
    {
        int ret = rmdir(path);
        if (ret == 0)
            return RESULT_OK;
        else
            return NativeToResult(errno);
    }

    Result Mkdir(const char* path, uint32_t mode)
    {
#ifdef _WIN32
        int ret = mkdir(path);
#else
        int ret = mkdir(path, (mode_t) mode);
#endif
        if (ret == 0)
            return RESULT_OK;
        else
            return NativeToResult(errno);
    }

    Result Unlink(const char* path)
    {
        int ret = unlink(path);
        if (ret == 0)
            return RESULT_OK;
        else
            return NativeToResult(errno);
    }

#if defined(__MACH__)
    Result GetApplicationSupportPath(const char* application_name, char* path, uint32_t path_len)
    {
        FSRef file;
        FSFindFolder(kUserDomain, kApplicationSupportFolderType, kCreateFolder, &file);
        FSRefMakePath(&file, (UInt8*) path, path_len);
        if (dmStrlCat(path, "/", path_len) >= path_len)
            return RESULT_INVAL;
        if (dmStrlCat(path, application_name, path_len) >= path_len)
            return RESULT_INVAL;
        Result r =  Mkdir(path, 0755);
        if (r == RESULT_EXIST)
            return RESULT_OK;
        else
            return r;
    }
#elif defined(_WIN32)
    Result GetApplicationSupportPath(const char* application_name, char* path, uint32_t path_len)
    {
        char tmp_path[MAX_PATH];

        if(SUCCEEDED(SHGetFolderPath(NULL,
                                     CSIDL_APPDATA | CSIDL_FLAG_CREATE,
                                     NULL,
                                     0,
                                     tmp_path)))
        {
            if (dmStrlCpy(path, tmp_path, path_len) >= path_len)
                return RESULT_INVAL;
            if (dmStrlCat(path, "\\", path_len) >= path_len)
                return RESULT_INVAL;
            if (dmStrlCat(path, application_name, path_len) >= path_len)
                return RESULT_INVAL;
            Result r =  Mkdir(path, 0755);
            if (r == RESULT_EXIST)
                return RESULT_OK;
            else
                return r;
        }
        else
        {
            return RESULT_UNKNOWN;
        }
    }
#elif defined(__linux__)
    Result GetApplicationSupportPath(const char* application_name, char* path, uint32_t path_len)
    {
        char* home = getenv("HOME");
        if (dmStrlCpy(path, home, path_len) >= path_len)
            return RESULT_INVAL;
        if (dmStrlCat(path, "/", path_len) >= path_len)
            return RESULT_INVAL;
        if (dmStrlCat(path, ".", path_len) >= path_len)
            return RESULT_INVAL;
        if (dmStrlCat(path, application_name, path_len) >= path_len)
            return RESULT_INVAL;
        Result r =  Mkdir(path, 0755);
        if (r == RESULT_EXIST)
            return RESULT_OK;
        else
            return r;
    }
#endif
}
