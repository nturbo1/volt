#include "vlt_files.h"
#include "vlt_assert.h"
#include "vlt_string.h"

String* dirName(const String* const filepath)
{
    if (filepath == NULL || filepath->len == 0)
        return NULL;

    const U8* filepathBytes = filepath->bytes;
    int parDirLastIdx = filepath->len - 1;

    for (; parDirLastIdx >= 0; parDirLastIdx--)
    {
        if (filepathBytes[parDirLastIdx] == '/') // TODO: should you add support for Windows?
            break;
    }

    return stringCopy(filepath, 0, parDirLastIdx + 1);
}
