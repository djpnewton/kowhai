#include "kowhai.h"

#include <stdio.h>
#include <assert.h>
#include <stddef.h>

//
// treenode symbols
//

#define SYM_GENERAL       0
#define SYM_START         1
#define SYM_STOP          2
#define SYM_RUNNING       3
#define SYM_FLUXCAPACITOR 4
#define SYM_FREQUENCY     5
#define SYM_COEFFICIENT   6
#define SYM_GAIN          7
#define SYM_OVEN          8
#define SYM_TEMP          9
#define SYM_TIMEOUT       10


//
// settings tree
//

#define FLUX_CAP_COUNT 2
#define COEFF_COUNT    6

// settings descriptor

struct kowhai_node_t settings_desc[] =
{
	// root level
	{ BRANCH_START, 	SYM_GENERAL, 		1 },
	{ UINT8_T, 			SYM_RUNNING, 		1 },
	
	// our flux capacitors
	{ BRANCH_START, 	SYM_FLUXCAPACITOR, 	FLUX_CAP_COUNT },
	{ UINT32_T,			SYM_FREQUENCY, 		1 },
	{ UINT32_T,			SYM_GAIN,	 		1 },
	{ FLOAT_T, 			SYM_COEFFICIENT,	COEFF_COUNT },
	{ BRANCH_END, 		SYM_FLUXCAPACITOR,	FLUX_CAP_COUNT },

	// our oven
	{ BRANCH_START, 	SYM_OVEN, 			1 },
	{ UINT16_T, 		SYM_TEMP, 			1 },
	{ UINT16_T, 		SYM_TIMEOUT, 		1 },
	{ BRANCH_END, 		SYM_OVEN,			1 },

	// end of root 
	{ BRANCH_END, 		SYM_GENERAL, 		1 },
};


// settings buffers 

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

struct settings_buf_t
{
    uint8_t running;
    struct flux_capacitor_t flux_capacitor[FLUX_CAP_COUNT];
    struct oven_t oven;
};

struct settings_buf_t settings_buf =
{
	// general
	255,

	// flux capacitors
	{
		{ 1000, 10, {0.1, 0.2, 0.3, 0.4, 0.5, 0.6} },
		{ 2000,	20, {0.7, 0.8, 0.9, 1.1, 1.2, 1.3} },
	},

	// oven
	{ 180, 30 },
};

#pragma pack()

// settings tree
struct kowhai_tree_t tree = 
{
	settings_desc,
	KOWHAI_DESC_SIZE(settings_desc),
	&settings_buf,
};


//
// main
//

int main()
{
	// basic type tests
	assert(kowhai_get_node_type_size(BRANCH_START) == 0);
	assert(kowhai_get_node_type_size(BRANCH_END) == 0);
	assert(kowhai_get_node_type_size(INT8_T) == 1);
	assert(kowhai_get_node_type_size(UINT8_T) == 1);
	assert(kowhai_get_node_type_size(INT32_T) == 4);
	assert(kowhai_get_node_type_size(UINT32_T) == 4);

	int nodes = 12;
	assert(kowhai_get_node_size(&tree.desc[1], NULL) == sizeof(settings_buf.running)); 			// size of running
	assert(kowhai_get_node_size(&tree.desc[7], NULL) == sizeof(settings_buf.oven)); 			// size of oven
	assert(kowhai_get_node_size(&tree.desc[2], NULL) == sizeof(settings_buf.flux_capacitor)); 	// size of fluxcap
	assert(kowhai_get_node_size(tree.desc, &nodes) == sizeof(struct settings_buf_t)); 			// size of everything 
	assert(nodes == 12);

	uint16_t offset;
	union kowhai_path_item path1[] = {SYM_GENERAL, SYM_RUNNING};
	nodes = 12;
	assert(seek_item(tree.desc, &nodes, 2, path1, NULL) == 0);
	nodes = 12;
	assert(seek_item(tree.desc, &nodes, 2, path1, &offset) == 0);
	assert(offset == 0);
	union kowhai_path_item path2[] = {SYM_GENERAL, SYM_OVEN, SYM_TIMEOUT};
	assert(seek_item(tree.desc, NULL, 3, path2, NULL) == 67);
	nodes = 12;
	assert(seek_item(tree.desc, &nodes, 3, path2, NULL) == 67);
	assert(seek_item(tree.desc, NULL, 3, path2, &offset) == 67);
	assert(offset == 67);

	nodes = 12;
	union kowhai_path_item path3[] = {SYM_GENERAL, FULL_PATH(SYM_FLUXCAPACITOR, 1), FULL_PATH(SYM_COEFFICIENT, 2)};
	assert (seek_item(tree.desc, NULL, 3, path3, NULL) == offsetof(struct settings_buf_t, flux_capacitor[1].coefficient[2]));
	assert (seek_item(tree.desc, &nodes, 3, path3, &offset) == offsetof(struct settings_buf_t, flux_capacitor[1].coefficient[2]));
	nodes = 12;
	union kowhai_path_item path4[] = {SYM_GENERAL, FULL_PATH(SYM_FLUXCAPACITOR, 0), FULL_PATH(SYM_COEFFICIENT, 5)};
	assert (seek_item(tree.desc, NULL, 3, path4, NULL) == offsetof(struct settings_buf_t, flux_capacitor[0].coefficient[5]));
	assert (seek_item(tree.desc, &nodes, 3, path4, &offset) == offsetof(struct settings_buf_t, flux_capacitor[0].coefficient[5]));

#if 0
    union kowhai_node_address symbols1[] = {SYM_GENERAL, SYM_OVEN, SYM_TEMP};
    union kowhai_node_address symbols2[] = {SYM_GENERAL, SYM_OVEN, SYM_TIMEOUT};
    union kowhai_node_address symbols3[] = {SYM_GENERAL, SYM_FLUXCAPACITOR};
    union kowhai_node_address symbols4[] = {431, 12343};
    union kowhai_node_address symbols5[] = {SYM_GENERAL, SYM_RUNNING};
    union kowhai_node_address symbols6[] = {SYM_GENERAL, SYM_FLUXCAPACITOR, SYM_GAIN};
    union kowhai_node_address symbols7[] = {SYM_GENERAL, SYM_FLUXCAPACITOR, SYM_COEFFICIENT};
    union kowhai_node_address symbols8[] = {SYM_GENERAL, KOWHAI_SYMBOL(SYM_FLUXCAPACITOR, 1), SYM_GAIN};
    union kowhai_node_address symbols9[] = {SYM_GENERAL, KOWHAI_SYMBOL(SYM_FLUXCAPACITOR, 1), KOWHAI_SYMBOL(SYM_COEFFICIENT, 3)};
    union kowhai_node_address symbols10[] = {SYM_GENERAL, SYM_FLUXCAPACITOR, KOWHAI_SYMBOL(SYM_COEFFICIENT, 3)};

    int offset;
    int size;
    struct kowhai_node_t* node;

    uint8_t running;
    uint16_t temp;
    uint16_t timeout;
    uint32_t gain;
    float coeff;
    struct flux_capacitor_t flux_capacitor;

#ifdef KOWHAI_DBG
    printf("kowhai debugging enabled!\n");
#endif

    // test tree parsing
    printf("test kowhai_get_setting...\t\t");
    assert(kowhai_get_setting(settings_tree, 3, symbols1, &offset, &node));
    assert(offset == 65);
    assert(kowhai_get_setting(settings_tree, 3, symbols2, &offset, &node));
    assert(offset == 67);
    assert(kowhai_get_setting(settings_tree, 2, symbols3, &offset, &node));
    assert(offset == 1);
    assert(!kowhai_get_setting(settings_tree, 2, symbols4, &offset, &node));
    assert(kowhai_get_setting(settings_tree, 3, symbols6, &offset, &node));
    assert(offset == 5);
    assert(kowhai_get_setting(settings_tree, 3, symbols8, &offset, &node));
    assert(offset == 1 + sizeof(struct flux_capacitor_t) + 4);
    assert(kowhai_get_setting(settings_tree, 3, symbols9, &offset, &node));
    assert(offset == 1 + sizeof(struct flux_capacitor_t) + 8 + 3 * 4);
    assert(kowhai_get_setting(settings_tree, 3, symbols10, &offset, &node));
    assert(offset == 1 + 8 + 3 * 4);
    printf(" passed!\n");

    // test branch size
    printf("test kowhai_get_branch_size...\t\t");
    assert(kowhai_get_branch_size(settings_tree, &size));
    assert(size == sizeof(struct settings_tree_t));
    printf(" passed!\n");

    // test read/write settings
    printf("test kowhai_read/kowhai_write...\t");
    running = 1;
    settings.running = 0;
    assert(kowhai_write(settings_tree, &settings, 2, symbols5, &running, 1));
    assert(settings.running == 1);
    running = 0;
    assert(kowhai_read(settings_tree, &settings, 2, symbols5, &running, 1));
    assert(running == 1);
    timeout = 999;
    settings.oven.timeout = 0;
    assert(kowhai_write(settings_tree, &settings, 3, symbols2, &timeout, sizeof(timeout)));
    assert(settings.oven.timeout == 999);
    timeout = 0;
    assert(kowhai_read(settings_tree, &settings, 3, symbols2, &timeout, sizeof(timeout)));
    assert(timeout == 999);
    flux_capacitor.frequency = 100; flux_capacitor.gain = 200;
    settings.flux_capacitor[0].frequency = 0; settings.flux_capacitor[0].gain = 0;
    assert(kowhai_write(settings_tree, &settings, 2, symbols3, &flux_capacitor, sizeof(flux_capacitor)));
    assert(settings.flux_capacitor[0].frequency == 100 && settings.flux_capacitor[0].gain == 200);
    flux_capacitor.frequency = 0; flux_capacitor.gain = 0;
    assert(kowhai_read(settings_tree, &settings, 2, symbols3, &flux_capacitor, sizeof(flux_capacitor)));
    assert(flux_capacitor.frequency == 100 && flux_capacitor.gain == 200);
    coeff = 999.9f;
    settings.flux_capacitor[1].coefficient[3] = 0;
    assert(kowhai_write(settings_tree, &settings, 3, symbols9, &coeff, sizeof(coeff)));
    assert(settings.flux_capacitor[1].coefficient[3] == 999.9f);
    coeff = 0;
    assert(kowhai_read(settings_tree, &settings, 3, symbols9, &coeff, sizeof(coeff)));
    assert(coeff == 999.9f);
    printf(" passed!\n");

#if 0
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
#endif

#endif
    printf(" passed!\n");

    return 0;
}
