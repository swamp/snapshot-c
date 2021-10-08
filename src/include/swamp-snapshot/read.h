/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef SWSN_SNAPSHOT_READ_H
#define SWSN_SNAPSHOT_READ_H

struct SwtiType;
struct SwtiChunk;

int swsnSnapshotRead(const uint8_t* source, size_t count, unmanagedTypeCreator creator, void* context,
                     const SwtiType* optionalExpectedType, struct SwtiChunk* targetChunk,
                     const struct SwtiType** foundType, const void** outValue, int verbosity);

#endif
