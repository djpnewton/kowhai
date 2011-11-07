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
#define NAME_COEFFICIENT   6
#define NAME_GAIN          7
#define NAME_OVEN          8
#define NAME_TEMP          9
#define NAME_TIMEOUT       10

//
// settings tree definition
//

struct kowhai_node_t settings_tree[] =
{
    { NODE_TYPE_BRANCH, NAME_GENERAL,       1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_START,         0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_LEAF,   NAME_STOP,          0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_LEAF,   NAME_RUNNING,       1, LEAF_TYPE_SETTING, SETTING_TYPE_UCHAR },
    { NODE_TYPE_BRANCH, NAME_FLUXCAPACITOR, 2, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_FREQUENCY,     1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   NAME_GAIN,          1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   NAME_COEFFICIENT,   6, LEAF_TYPE_SETTING, SETTING_TYPE_FLOAT },
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
    float coefficient[6];
};

struct oven_t
{
    int16_t temp;
    uint16_t timeout;
};

struct settings_tree_t
{
    uint8_t running;
    struct flux_capacitor_t flux_capacitor[2];
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
    uint16_t symbols4[] = {431, 12343};

#ifdef KOWHAI_DBG
    printf("kowhai debugging enabled!\n");
#endif
    printf("offset of settings_tree_t.oven_t.temp: %d (should be 65)\n\n", kowhai_get_setting_offset(settings_tree, 3, symbols1));
    printf("offset of settings_tree_t.oven_t.timeout: %d (should be 67)\n\n", kowhai_get_setting_offset(settings_tree, 3, symbols2));
    printf("offset of settings_tree_t.flux_capacitor_t: %d (should be 1)\n\n", kowhai_get_setting_offset(settings_tree, 2, symbols3));
    printf("offset of nonsense: %d (should be -1)\n\n", kowhai_get_setting_offset(settings_tree, 2, symbols4));
    printf("size of settings_tree: %d (should be 69)\n\n", kowhai_get_branch_size(settings_tree));
    return 0;
}
