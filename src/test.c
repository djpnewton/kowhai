#include "kowhai.h"

#include <stdio.h>
#include <assert.h>

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

#define FLUX_CAP_COUNT 2
#define COEFF_COUNT    6

struct kowhai_node_t settings_tree[] =
{
    { NODE_TYPE_BRANCH, NAME_GENERAL,       1, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_START,         0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_LEAF,   NAME_STOP,          0, LEAF_TYPE_ACTION, 0 },
    { NODE_TYPE_LEAF,   NAME_RUNNING,       1, LEAF_TYPE_SETTING, SETTING_TYPE_UCHAR },
    { NODE_TYPE_BRANCH, NAME_FLUXCAPACITOR, FLUX_CAP_COUNT, 0, 0 },
    { NODE_TYPE_LEAF,   NAME_FREQUENCY,     1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   NAME_GAIN,          1, LEAF_TYPE_SETTING, SETTING_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   NAME_COEFFICIENT,   COEFF_COUNT, LEAF_TYPE_SETTING, SETTING_TYPE_FLOAT },
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

#pragma pack(1)

struct flux_capacitor_t
{
    uint32_t frequency;
    uint32_t gain;
    float coefficient[COEFF_COUNT];
};

struct oven_t
{
    int16_t temp;
    uint16_t timeout;
};

struct settings_tree_t
{
    uint8_t running;
    struct flux_capacitor_t flux_capacitor[FLUX_CAP_COUNT];
    struct oven_t oven;
};

#pragma pack()

//
// main
//

int main()
{
    uint16_t symbols1[] = {NAME_GENERAL, NAME_OVEN, NAME_TEMP};
    uint16_t symbols2[] = {NAME_GENERAL, NAME_OVEN, NAME_TIMEOUT};
    uint16_t symbols3[] = {NAME_GENERAL, NAME_FLUXCAPACITOR};
    uint16_t symbols4[] = {431, 12343};
    uint16_t symbols5[] = {NAME_GENERAL, NAME_RUNNING};
    uint16_t symbols6[] = {NAME_GENERAL, NAME_FLUXCAPACITOR, NAME_GAIN};
    uint16_t symbols7[] = {NAME_GENERAL, NAME_FLUXCAPACITOR, NAME_COEFFICIENT};

    struct settings_tree_t settings;

    uint8_t running;
    uint16_t temp;
    uint16_t timeout;
    uint32_t gain;
    float coeff;

#ifdef KOWHAI_DBG
    printf("kowhai debugging enabled!\n");
#endif

    // test tree parsing
    printf("test kowhai_get_setting_offset...\t");
    assert(kowhai_get_setting_offset(settings_tree, 3, symbols1) == 65);
    assert(kowhai_get_setting_offset(settings_tree, 3, symbols2) == 67);
    assert(kowhai_get_setting_offset(settings_tree, 2, symbols3) == 1);
    assert(kowhai_get_setting_offset(settings_tree, 2, symbols4) == -1);
    assert(kowhai_get_setting_offset(settings_tree, 3, symbols6) == 5);
    printf(" passed!\n");

    // test branch size
    printf("test kowhai_get_branch_size...\t\t");
    assert(kowhai_get_branch_size(settings_tree) == sizeof(struct settings_tree_t));
    printf(" passed!\n");
    
    // test set/get settings
    printf("test kowhai_get_xxx/kowhai_set_xxx...\t");
    settings.running = 0;
    assert(kowhai_set_char(settings_tree, &settings, 2, symbols5, 255));
    assert(settings.running == 255);
    assert(kowhai_get_char(settings_tree, &settings, 2, symbols5, &running));
    assert(running == 255);
    settings.oven.temp = 0;
    assert(kowhai_set_int16(settings_tree, &settings, 3, symbols1, 999));
    assert(settings.oven.temp == 999);
    assert(kowhai_get_int16(settings_tree, &settings, 3, symbols1, &temp));
    assert(temp == 999);
    settings.oven.timeout = 0;
    assert(kowhai_set_int16(settings_tree, &settings, 3, symbols2, 999));
    assert(settings.oven.timeout == 999);
    assert(kowhai_get_int16(settings_tree, &settings, 3, symbols2, &timeout));
    assert(timeout == 999);
    settings.flux_capacitor[0].gain = 0;
    assert(kowhai_set_int32(settings_tree, &settings, 3, symbols6, 999));
    assert(settings.flux_capacitor[0].gain == 999);
    assert(kowhai_get_int32(settings_tree, &settings, 3, symbols6, &gain));
    assert(gain == 999);
    settings.flux_capacitor[0].coefficient[0] = 0;
    assert(kowhai_set_float(settings_tree, &settings, 3, symbols7, 999.9f));
    assert(settings.flux_capacitor[0].coefficient[0] ==  999.9f);
    assert(kowhai_get_float(settings_tree, &settings, 3, symbols7, &coeff));
    assert(coeff == 999.9f);
    printf(" passed!\n");

    return 0;
}
