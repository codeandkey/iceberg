#ifndef IB_CONFIG
#define IB_CONFIG

#define IB_CONFIG_BUFLEN 64
#define IB_CONFIG_FILENAME "ib.cfg"

/* define config key name consts */

#define IB_CONFIG_GRAPHICS_WIDTH "graphics.width"
#define IB_CONFIG_GRAPHICS_HEIGHT "graphics.height"
#define IB_CONFIG_GRAPHICS_FS "graphics.fullscreen"
#define IB_CONFIG_GRAPHICS_VSYNC "graphics.vsync"

/* function declarations */

int ib_config_init(void);
void ib_config_free(void);

int ib_config_get_int(const char* key, int def);
const char* ib_config_get_str(const char* key, const char* def);

#endif
