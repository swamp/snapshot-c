/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef SWAMP_SNAPSHOT_SRC_INCLUDE_SWAMP_SNAPSHOT_WRITE_TYPEINFO_H
#define SWAMP_SNAPSHOT_SRC_INCLUDE_SWAMP_SNAPSHOT_WRITE_TYPEINFO_H

struct SwtiChunk;
struct FldOutStream;

int swsnWriteTypeInformationChunk(struct FldOutStream* stream, const struct SwtiChunk* chunk);

#endif // SWAMP_SNAPSHOT_SRC_INCLUDE_SWAMP_SNAPSHOT_WRITE_TYPEINFO_H
