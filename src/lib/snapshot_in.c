/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <flood/in_stream.h>
#include <raff/raff.h>
#include <raff/tag.h>
#include <swamp-dump/dump.h>
#include <swamp-dump/dump_ascii.h>
#include <swamp-dump/dump_unmanaged.h>
#include <swamp-runtime/swamp.h>
#include <swamp-snapshot/read.h>
#include <swamp-snapshot/read_typeinfo.h>
#include <swamp-typeinfo/chunk.h>
#include <swamp-typeinfo-serialize/deserialize.h>
#include <swamp-typeinfo/equal.h>
#include <swamp-typeinfo/typeinfo.h>
#include <imprint/allocator.h>

int swsnReadTypeInformationChunk(FldInStream* inStream, SwtiChunk* target, ImprintAllocator* allocator)
{
    RaffTag foundIcon, foundName;
    uint32_t foundChunkSize;
    int octets = raffReadChunkHeader(inStream->p, inStream->size - inStream->pos, foundIcon, foundName,
                                     &foundChunkSize);
    if (octets < 0) {
        return octets;
    }
    inStream->p += octets;
    inStream->pos += octets;

    RaffTag expectedIcon = {0xF0, 0x9F, 0x93, 0x9C};
    RaffTag expectedName = {'s', 't', 'i', '0'};

    if (!raffTagEqual(foundIcon, expectedIcon)) {
        return -3;
    }

    if (!raffTagEqual(foundName, expectedName)) {
        return -4;
    }

    int typeInformationOctetCount = swtisDeserialize(inStream->p, foundChunkSize, target, allocator);
    if (typeInformationOctetCount < 0) {
        return typeInformationOctetCount;
    }

    inStream->p += foundChunkSize;
    inStream->pos += foundChunkSize;

    return octets;
}

static int readRaffAndSnapshotChunkHeaders(FldInStream* inStream, int* outTypeIndex)
{
    RaffTag fileIcon;
    RaffTag fileName;

    int octetCount = raffReadHeader(inStream->p, inStream->size, fileIcon, fileName);
    if (octetCount < 0) {
        return octetCount;
    }
    inStream->p += octetCount;
    inStream->pos += octetCount;

    RaffTag icon;
    RaffTag tag;
    uint32_t chunkSize;

    octetCount = raffReadChunkHeader(inStream->p, inStream->size - inStream->pos, icon, tag, &chunkSize);
    if (octetCount < 0) {
        return octetCount;
    }

    RaffTag expectedIcon = {0xF0, 0x9F, 0x93, 0xB8};
    RaffTag expectedTag = {'s', 'n', 'a', '0'};

    if (!raffTagEqual(expectedIcon, icon)) {
        return -5;
    }

    if (!raffTagEqual(expectedTag, tag)) {
        return -6;
    }

    inStream->p += octetCount;
    inStream->pos += octetCount;

    uint8_t readTypeIndex;
    fldInStreamReadUInt8(inStream, &readTypeIndex);

    *outTypeIndex = readTypeIndex;

    return 0;
}

static int readStateHeaderAndState(FldInStream* inStream, unmanagedTypeCreator creator, void* context,
                                   const SwtiType* foundType, SwampDynamicMemory* dynamicMemory, struct SwampUnmanagedMemory* unmanagedMemory, const void** outValue)
{
    RaffTag icon;
    RaffTag tag;
    uint32_t chunkSize;

    int octetCount = raffReadChunkHeader(inStream->p, inStream->size - inStream->pos, icon, tag, &chunkSize);
    if (octetCount < 0) {
        return octetCount;
    }

    RaffTag expectedIcon = {0xF0, 0x9F, 0x93, 0xB8};
    RaffTag expectedTag = {'s', 't', 'a', '0'};

    if (!raffTagEqual(expectedIcon, icon)) {
        return -5;
    }

    if (!raffTagEqual(expectedTag, tag)) {
        return -6;
    }

    inStream->p += octetCount;
    inStream->pos += octetCount;

    int dumpError = swampDumpFromOctets(inStream, foundType, creator, context, outValue, dynamicMemory, unmanagedMemory);

    return dumpError;
}

int swsnSnapshotRead(const uint8_t* source, size_t sourceOctetCount, unmanagedTypeCreator creator, void* context,
                     const SwtiType* optionalExpectedType, SwtiChunk* targetChunk, const SwtiType** outFoundType,
                     const void** outValue, struct SwampDynamicMemory* targetDynamicMemory, struct SwampUnmanagedMemory* targetUnmanagedMemory, int verbosity, ImprintAllocator* allocator)
{
    FldInStream inStream;
    *outValue = 0;
    *outFoundType = 0;

    fldInStreamInit(&inStream, source, sourceOctetCount);

    int typeIndex;
    int headerError = readRaffAndSnapshotChunkHeaders(&inStream, &typeIndex);
    if (headerError < 0) {
        return headerError;
    }

    int worked = swsnReadTypeInformationChunk(&inStream, targetChunk, allocator);
    if (worked < 0) {
        return worked;
    }

    const SwtiType* foundType = swtiChunkTypeFromIndex(targetChunk, typeIndex);
    if (foundType == 0) {
        CLOG_SOFT_ERROR("could not find type %d", typeIndex)
        return -4;
    }

    *outFoundType = foundType;

    if (verbosity) {
        CLOG_EXECUTE(char buf[2048];)
        CLOG_VERBOSE("found type %s", swtiDebugString(foundType, 0, buf, 2048))
    }

    int dumpError = readStateHeaderAndState(&inStream, creator, context, foundType, targetDynamicMemory, targetUnmanagedMemory, outValue);
    if (dumpError < 0) {
        return dumpError;
    }

    if (verbosity > 0) {
#define TEMP_BUF_SIZE (8192)
        CLOG_EXECUTE(char temp[TEMP_BUF_SIZE];)
        CLOG_INFO("read snapshot: %s", swampDumpToAsciiString(*outValue, foundType, 0, temp, TEMP_BUF_SIZE))
    }

    if (optionalExpectedType != 0) {
        int compareError = swtiTypeEqual(foundType, optionalExpectedType);
        if (compareError < 0) {
            *outValue = 0;
            return compareError;
        }
    }

    return 0;
}
