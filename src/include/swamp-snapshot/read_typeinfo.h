/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef SWAMP_SNAPSHOT_SRC_INCLUDE_SWAMP_SNAPSHOT_READ_TYPEINFO_H
#define SWAMP_SNAPSHOT_SRC_INCLUDE_SWAMP_SNAPSHOT_READ_TYPEINFO_H

struct SwtiChunk;
struct FldInStream;

int swsnReadTypeInformationChunk(struct FldInStream* inStream, struct SwtiChunk* target);

#endif // SWAMP_SNAPSHOT_SRC_INCLUDE_SWAMP_SNAPSHOT_READ_TYPEINFO_H
