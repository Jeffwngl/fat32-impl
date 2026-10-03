#pragma once

#ifndef CLUSTER_H
#define CLUSTER_H

#include <stdint.h>

#include "Disk.h"
#include "BootSector.h"

int fat32_read_cluster(
	Disk* disk,
	BootSector* bootSector,
	uint32_t N,
    void* buffer
);

int fat32_is_eoc(
    uint32_t val
);

int fat32_is_bad_cluster(
    uint32_t val
);

int fat32_is_free_cluster(
    uint32_t val
);

#endif
