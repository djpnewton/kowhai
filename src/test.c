#include "kowhai.h"

#include <stdio.h>

#define NAME_GENERAL       0
#define NAME_RESET         100
#define NAME_FLUXCAPACITOR 1
#define NAME_FREQUENCY     2
#define NAME_GAIN          3
#define NAME_OVEN          4
#define NAME_TEMP          5
#define NAME_TIMEOUT       6

struct kowhai_node_t settings_tree[] =
{
    { NODE_TYPE_BRANCH, NAME_GENERAL,       1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_RESET,         0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_BRANCH, NAME_FLUXCAPACITOR, 1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_FREQUENCY,     1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT16 },
    { NODE_TYPE_LEAF,   NAME_GAIN,          1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT16 },
    { NODE_TYPE_END,    NAME_FLUXCAPACITOR, 0, 0, 0 },
    { NODE_TYPE_BRANCH, NAME_OVEN,          1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_TEMP,          1, LEAF_TYPE_SETTING, SETTING_TYPE_INT16 },
    { NODE_TYPE_LEAF,   NAME_TIMEOUT,       1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT16 },
    { NODE_TYPE_END,    NAME_OVEN,          0, 0, 0 },
    { NODE_TYPE_END,    NAME_GENERAL,       0, 0, 0 },
};

struct flux_capacitor_t
{
    uint32_t frequency;
    uint32_t gain;
};

struct oven_t
{
    int16_t temp;
    uint16_t timeout;
};

struct settings_tree_t
{
    struct flux_capacitor_t flux_capacitor;
    struct oven_t oven;
};

int main()
{
    uint16_t symbols[] = {NAME_GENERAL, NAME_OVEN, NAME_TEMP};

#ifdef KOWHAI_DBG
    printf("kowhai debugging enabled!\n");
#endif
    printf("offset of settings_tree_t.oven_t.temp: %d (should be 8)\n", kowhai_get_setting_offset(settings_tree, 3, symbols));
    return 0;
}
