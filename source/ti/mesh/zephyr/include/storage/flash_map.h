/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public API for flash map
 */

#ifndef ZEPHYR_INCLUDE_STORAGE_FLASH_MAP_H_
#define ZEPHYR_INCLUDE_STORAGE_FLASH_MAP_H_

/**
 * @brief Abstraction over flash area and its driver which helps to operate on
 * flash regions easily and effectively.
 *
 * @defgroup flash_area_api flash area Interface
 * @{
 */

/**
 *
 * Provides abstraction of flash regions for type of use,
 * for example,  where's my image?
 *
 * System will contain a map which contains flash areas. Every
 * region will contain flash identifier, offset within flash, and length.
 *
 * 1. This system map could be in a file within filesystem (Initializer
 * must know/figure out where the filesystem is at).
 * 2. Map could be at fixed location for project (compiled to code)
 * 3. Map could be at specific place in flash (put in place at mfg time).
 *
 * Note that the map you use must be valid for BSP it's for,
 * match the linker scripts when platform executes from flash,
 * and match the target offset specified in download script.
 */
#include <zephyr/types.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_FLASH_0_ID 0	/** device_id for SoC flash memory driver   */
#define SPI_FLASH_0_ID 1	/** device_id for external SPI flash driver */

/**
 * @brief Structure for store flash partition data
 *
 * It is used as the flash_map array entry or stand-alone user data. Structure
 * contains all data needed to operate on the flash partitions.
 */
struct flash_area {
	u8_t fa_id; /** ID of flash area */
	u8_t fa_device_id;
	u16_t pad16;
	off_t fa_off; /** flash partition offset */
	size_t fa_size; /** flash partition size */
	const char *fa_dev_name; /** flash device name */
};

/**
 * @brief Structure for transfer flash sector boundaries
 *
 * This template is used for presentation of flash memory structure. It
 * consumes much less RAM than @ref flash_area
 */
struct flash_sector {
	off_t fs_off; /** flash sector offset */
	size_t fs_size; /** flash sector size */
};

/**
 * @brief Retrieve partitions flash area from the flash_map.
 *
 * Function Retrieves flash_area from flash_map for given partition.
 *
 * @param[in]  id ID of the flash partition.
 * @param[out] fa Pointer which has to reference flash_area. If
 * @p ID is unknown, it will be NULL on output.
 */
int flash_area_open(u8_t id, const struct flash_area **fa);

/**
 * Retrieve info about sectors within the area.
 *
 * @param[in]  fa_id    Given flash area ID
 * @param[out] sectors  buffer for sectors data
 * @param[in,out] count On input Capacity of @p sectors, on output number of
 * sectors Retrieved.
 *
 * @return  0 on success, negative errno code on fail. Especially returns
 * -ENOMEM if There are too many flash pages on the flash_area to fit in the
 * array.
 */
int flash_area_get_sectors(int fa_id, u32_t *count,
			   struct flash_sector *sectors);

/**
 * Flash map iteration callback
 *
 * @param fa flash area
 * @param user_data User supplied data
 *
 */
typedef void (*flash_area_cb_t)(const struct flash_area *fa,
				void *user_data);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_STORAGE_FLASH_MAP_H_ */