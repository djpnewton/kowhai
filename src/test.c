#include "kowhai.h"

#include <stdio.h>

//
// treenode name symbols
//

#define NAME_GENERAL       0
#define NAME_START         1
#define NAME_STOP          2
#define NAME_RUNNING       3
#define NAME_FLUXCAPACITOR 4
#define NAME_FREQUENCY     5
#define NAME_GAIN          6
#define NAME_OVEN          7
#define NAME_TEMP          8
#define NAME_TIMEOUT       9

//
// settings tree definition
//

struct kowhai_node_t settings_tree[] =
{
    { NODE_TYPE_BRANCH, NAME_GENERAL,       1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_START,         0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_LEAF,   NAME_STOP,          0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_LEAF,   NAME_RUNNING,       1, LEAF_TYPE_SETTING, SETTING_TYPE_UCHAR | SETTING_TYPE_READONLY },
    { NODE_TYPE_BRANCH, NAME_FLUXCAPACITOR, 1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_FREQUENCY,     1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   NAME_GAIN,          1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT32 },
    { NODE_TYPE_END,    NAME_FLUXCAPACITOR, 0, 0, 0 },
    { NODE_TYPE_BRANCH, NAME_OVEN,          1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_TEMP,          1, LEAF_TYPE_SETTING, SETTING_TYPE_INT16 },
    { NODE_TYPE_LEAF,   NAME_TIMEOUT,       1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT16 },
    { NODE_TYPE_END,    NAME_OVEN,          0, 0, 0 },
    { NODE_TYPE_END,    NAME_GENERAL,       0, 0, 0 },
};

//
// settings tree structs
//

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

//
// main
//

int main()
{
    uint16_t symbols1[] = {NAME_GENERAL, NAME_OVEN, NAME_TEMP};
    uint16_t symbols2[] = {NAME_GENERAL, NAME_OVEN, NAME_TIMEOUT};
    uint16_t symbols3[] = {NAME_GENERAL, NAME_FLUXCAPACITOR};

#ifdef KOWHAI_DBG
    printf("kowhai debugging enabled!\n");
#endif
    printf("offset of settings_tree_t.oven_t.temp: %d (should be 9)\n\n", kowhai_get_setting_offset(settings_tree, 3, symbols1));
    printf("offset of settings_tree_t.oven_t.timeout: %d (should be 11)\n\n", kowhai_get_setting_offset(settings_tree, 3, symbols2));
    printf("offset of settings_tree_t.flux_capacitor_t: %d (should be 1)\n\n", kowhai_get_setting_offset(settings_tree, 2, symbols3));
    return 0;
}
