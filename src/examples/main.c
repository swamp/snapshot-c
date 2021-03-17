/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <stdio.h>
#include <swamp-dump/dump.h>
#include <swamp-dump/dump_ascii.h>
#include <swamp-dump/types.h>
#include <swamp-dump/dump_yaml.h>
#include <swamp-runtime/allocator.h>

#include <swamp-typeinfo/chunk.h>
#include <swamp-typeinfo/deserialize.h>
#include <swamp-typeinfo/typeinfo.h>

#include <swamp-snapshot/read.h>

clog_config g_clog;

static void tyran_log_implementation(enum clog_type type, const char* string)
{
    (void) type;
    fprintf(stderr, "%s\n", string);
}

int main(int argc, char* argv[])
{
    g_clog.log = tyran_log_implementation;

    if (argc <= 1) {
        CLOG_OUTPUT("usage: %s snapshotfile", argv[0]);
        return -1;
    }

    swamp_allocator allocator;
    swamp_allocator_init(&allocator);

    const char* filename = argv[1];

    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t* data = malloc(fileSize);
    fread(data, 1, fileSize, fp);
    fclose(fp);


    SwtiChunk tempChunk;
    const SwtiType* foundType = 0;
    const swamp_value* value;

    int worked = swsnSnapshotRead(data, fileSize, &allocator, 0, &tempChunk, &foundType, &value, 0);
    if (worked < 0) {
        CLOG_SOFT_ERROR("couldn't read snapshot %d", worked);
    }
#define BUF_SIZE (32*1024)
    char buf[BUF_SIZE];

    int yamlFlags =  swampDumpFlagBlobAscii; // swampDumpFlagBlobExpanded | swampDumpFlagAlias
    CLOG_OUTPUT("yaml:\n%s", swampDumpToYamlString(value, foundType, yamlFlags, buf, BUF_SIZE));
    CLOG_OUTPUT("ascii:\n%s", swampDumpToAsciiString(value, foundType, buf, BUF_SIZE));

    return 0;
}
