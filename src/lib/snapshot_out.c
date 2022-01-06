#include <clog/clog.h>
#include <flood/out_stream.h>
#include <swamp-typeinfo/chunk.h>
#include <swamp-typeinfo/typeinfo.h>
#include <swamp-typeinfo/serialize.h>
#include <raff/raff.h>
#include <raff/write.h>
#include <raff/tag.h>
#include <swamp-runtime/swamp.h>
#include <swamp-dump/dump_ascii.h>

#include <raff/write.h>

int swsnWriteTypeInformationChunk(FldOutStream* stream, const SwtiChunk* chunk)
{
    uint8_t typeInformationOctets[1024];

    int payloadCount = swtiSerialize(typeInformationOctets, 1024, chunk);
    if (payloadCount < 0) {
        return payloadCount;
    }

    RaffTag icon = {0xF0, 0x9F, 0x93, 0x9C};
    RaffTag name = {'s', 't', 'i', '0'};
    int octets = raffWriteChunkHeader(stream->p, stream->size - stream->pos, icon, name, payloadCount);
    if (octets < 0) {
        return octets;
    }
    stream->p += octets;
    stream->pos += octets;

    fldOutStreamWriteOctets(stream, typeInformationOctets, payloadCount);

    return octets + payloadCount;
}

static int writeRaffAndSnapshotChunks(FldOutStream* outStream, int rootTypeIndex)
{
    int octetCount = raffWriteHeader(outStream->p, outStream->size);
    if (octetCount < 0) {
        return octetCount;
    }
    outStream->p += octetCount;
    outStream->pos += octetCount;

    RaffTag icon = {0xF0, 0x9F, 0x93, 0xB8};
    RaffTag tag = {'s', 'n', 'a', '0'};
    octetCount = raffWriteChunkHeader(outStream->p, outStream->size - outStream->pos, icon, tag, 1);
    if (octetCount < 0) {
        return octetCount;
    }
    outStream->p += octetCount;
    outStream->pos += octetCount;
    fldOutStreamWriteUInt8(outStream, rootTypeIndex);

    return 0;
}

int swsnSnapshotWrite(uint8_t* target, size_t maxCount, const void* value, const struct SwtiType* stateType,
                     int verbosity)
{
    if (verbosity > 0) {
#define TEMP_BUF_SIZE (8192)
        char temp[TEMP_BUF_SIZE];
        CLOG_INFO("write snapshot: %s", swampDumpToAsciiString(value, stateType, 0, temp, TEMP_BUF_SIZE));
    }

    SwtiChunk tempChunk;

    int stateIndex;

    if (verbosity > 0) {
        char temp[TEMP_BUF_SIZE];
        CLOG_INFO("type info %s", swtiDebugString(stateType, 0, temp, TEMP_BUF_SIZE));
    }

    int initResult = swtiChunkInitOnlyOneType(&tempChunk, stateType, &stateIndex);
    if (initResult < 0) {
        CLOG_SOFT_ERROR("could not create a new type information chunk when writing %d", initResult);
        return initResult;
    }

    FldOutStream outStream;
    fldOutStreamInit(&outStream, target, maxCount);

    int headerChunksResult = writeRaffAndSnapshotChunks(&outStream, stateIndex);
    if (headerChunksResult < 0) {
        return headerChunksResult;
    }

    int worked = swsnWriteTypeInformationChunk(&outStream, &tempChunk);
    if (worked < 0) {
        CLOG_SOFT_ERROR("could not write type information chunk %d", worked);
        return worked;
    }

    RaffTag icon = {0xF0, 0x9F, 0x93, 0xB8};
    RaffTag tag = {'s', 't', 'a', '0'};
    int octetCount = raffWriteChunkHeader(outStream.p, outStream.size - outStream.pos, icon, tag, 0);
    if (octetCount < 0) {
        return octetCount;
    }
    outStream.p += octetCount;
    outStream.pos += octetCount;

    int errorCode = 0; // TODO: Fix This: swampDumpToOctets(&outStream, value, stateType);
    if (errorCode != 0) {
        CLOG_SOFT_ERROR("could not save dump to octets: %d", errorCode);
        return errorCode;
    }

    CLOG_INFO("octet count:%zu", outStream.pos);

    return outStream.pos;
}
