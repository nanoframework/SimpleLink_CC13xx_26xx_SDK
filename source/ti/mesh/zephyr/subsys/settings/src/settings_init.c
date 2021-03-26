
#ifdef CONFIG_BT_SETTINGS
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr.h>
#include <device.h>
#include <hal_assert.h>
#include "settings/settings.h"
#include <storage/flash_map.h>
#include "settings/settings_nvs.h"

#define k_panic() HAL_ASSERT_SPINLOCK

bool settings_subsys_initialized;

void settings_init(void);

int settings_backend_init(void);

typedef struct
{
  uint8_t state;    // page state
  uint8_t cycle;    // page compaction cycle count. Used to select the 'newest' active page
                    // at device reset, in the very unlikely scenario that both pages are active.
  uint8_t mode;     // compact mode
  uint8_t allActive;  //all items are active or not
  uint8_t sPage;
  uint8_t ePage;
  uint16_t offset;  // page offset
  uint16_t sOffset;
  uint16_t eOffset;
} NVOCMP_pageInfo_t;

typedef struct
{
  uint8_t xDstPage;         // xdst page
  uint8_t xSrcSPage;    // xsrc start page
  uint8_t xSrcEPage;      // xsrc end page
  uint8_t xSrcPages;    // no of xsrc pages
  uint16_t xDstOffset;      // xdst offset
  uint16_t xSrcSOffset;    // xsrc start offset
  uint16_t xSrcEOffset;    // xsrc end offset
} NVOCMP_compactInfo_t;

typedef struct
{
  uint8_t nvSize;       // no of NV pages
  uint8_t headPage;     // head active page
  uint8_t tailPage;     // transfer destination page
  uint8_t actPage;      // current active page
  uint8_t xsrcPage;     // transfer source page
  uint16_t actOffset;   // active page offset
  uint16_t xsrcOffset;  // transfer source page offset
  uint16_t xdstOffset;  // transfer destination page offset
  NVOCMP_compactInfo_t compactInfo;
  NVOCMP_pageInfo_t pageInfo[2];
} NVOCMP_nvHandle_t;

extern NVOCMP_nvHandle_t NVOCMP_nvHandle;
static struct settings_nvs default_settings_nvs;

int settings_backend_init(void)
{
	int rc;
	u16_t cnt = 0;
	size_t nvs_sector_size, nvs_size = 0;
	const struct flash_area *fa;
	struct flash_sector hw_flash_sector;
	u32_t sector_cnt = 1;

	rc = flash_area_open(DT_FLASH_AREA_STORAGE_ID, &fa);
	if (rc) {
		return rc;
	}

	rc = flash_area_get_sectors(DT_FLASH_AREA_STORAGE_ID, &sector_cnt,
				    &hw_flash_sector);
	if (rc == -ENODEV) {
		return rc;
	} else if (rc != 0 && rc != -ENOMEM) {
		k_panic();
	}

	nvs_sector_size = CONFIG_SETTINGS_NVS_SECTOR_SIZE_MULT *
			  hw_flash_sector.fs_size;

	if (nvs_sector_size > UINT16_MAX) {
		return -EDOM;
	}

	while (cnt < CONFIG_SETTINGS_NVS_SECTOR_COUNT) {
		nvs_size += nvs_sector_size;
		if (nvs_size > fa->fa_size) {
			break;
		}
		cnt++;
	}

	/* define the nvs file system using the page_info */
	default_settings_nvs.cf_nvs.sector_size = nvs_sector_size;
	default_settings_nvs.cf_nvs.sector_count = cnt;
	default_settings_nvs.cf_nvs.offset = fa->fa_off + NVOCMP_nvHandle.pageInfo[0].offset;
	default_settings_nvs.flash_dev_name = fa->fa_dev_name;

	rc = settings_nvs_backend_init(&default_settings_nvs);
	if (rc) {
		return rc;
	}

	rc = settings_nvs_src(&default_settings_nvs);

	if (rc) {
		return rc;
	}

	rc = settings_nvs_dst(&default_settings_nvs);

	return rc;
}

int settings_subsys_init(void)
{

	int err = 0;

	if (settings_subsys_initialized) {
		return 0;
	}

	settings_init();

	err = settings_backend_init(); /* func rises kernel panic once error */

	if (!err) {
		settings_subsys_initialized = true;
	}

	return err;
}
#endif //CONFIG_BT_SETTINGS
