/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef SWSN_SNAPSHOT_WRITE_H
#define SWSN_SNAPSHOT_WRITE_H

struct SwtiType;

int swsnSnapshotWrite(uint8_t* target, size_t maxCount, const void* value, const struct SwtiType* stateType,
                     int verbosity);

#endif
