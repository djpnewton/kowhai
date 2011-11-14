#include "../src/kowhai.h"
#include "../src/kowhai_protocol.h"
#include "xpsocket.h"

#include <stdio.h>
#include <assert.h>

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
#define SYM_DELAY         11
#define SYM_STATUS        11

//
// settings tree descriptor
//

#define FLUX_CAP_COUNT 2
#define COEFF_COUNT    6

struct kowhai_node_t settings_descriptor[] =
{
    { NODE_TYPE_BRANCH, SYM_GENERAL,       1, 0 },
    { NODE_TYPE_BRANCH, SYM_FLUXCAPACITOR, FLUX_CAP_COUNT, 0 },
    { NODE_TYPE_LEAF,   SYM_FREQUENCY,     1, DATA_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   SYM_GAIN,          1, DATA_TYPE_UINT32 },
    { NODE_TYPE_LEAF,   SYM_COEFFICIENT,   COEFF_COUNT, DATA_TYPE_FLOAT },
    { NODE_TYPE_END,    SYM_FLUXCAPACITOR, 0, 0 },
    { NODE_TYPE_BRANCH, SYM_OVEN,          1, 0 },
    { NODE_TYPE_LEAF,   SYM_TEMP,          1, DATA_TYPE_INT16 },
    { NODE_TYPE_LEAF,   SYM_TIMEOUT,       1, DATA_TYPE_UINT16 },
    { NODE_TYPE_END,    SYM_OVEN,          0, 0 },
    { NODE_TYPE_END,    SYM_GENERAL,       0, 0 },
};

//
// shadow tree descriptor
//

struct kowhai_node_t shadow_descriptor[] =
{
    { NODE_TYPE_BRANCH, SYM_GENERAL,       1, 0 },
    { NODE_TYPE_LEAF,   SYM_RUNNING,       1, DATA_TYPE_UCHAR },
    { NODE_TYPE_LEAF,   SYM_STATUS,        1, DATA_TYPE_UCHAR },
    { NODE_TYPE_END,    SYM_GENERAL,       0, 0 },
};

//
// action tree descriptor
//

struct kowhai_node_t action_descriptor[] =
{
    { NODE_TYPE_BRANCH, SYM_GENERAL,       1, 0 },
    { NODE_TYPE_BRANCH, SYM_START,         1, 0 },
    { NODE_TYPE_LEAF,   SYM_DELAY,         1, DATA_TYPE_UINT32 },
    { NODE_TYPE_END,    SYM_START,         0, 0 },
    { NODE_TYPE_BRANCH, SYM_STOP,          1, 0 },
    { NODE_TYPE_END,    SYM_STOP,          0, 0 },
    { NODE_TYPE_END,    SYM_GENERAL,       0, 0 },
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

struct settings_data_t
{
    struct flux_capacitor_t flux_capacitor[FLUX_CAP_COUNT];
    struct oven_t oven;
};

//
// shadow tree stucts
//

struct shadow_tree_t
{
    uint8_t running;
    uint8_t status;
};

#pragma pack()

//
// test commands
//

#define TEST_BASIC           0
#define TEST_PROTOCOL_SERVER 1
#define TEST_PROTOCOL_CLIENT 2

//
// test tree ids
//

#define TREE_ID_SETTINGS 0
#define TREE_ID_SHADOW   1
#define TREE_ID_ACTIONS  2

//
// main
//

struct settings_data_t settings;
struct shadow_tree_t shadow;

void server_buffer_received(xpsocket_handle conn, char* buffer, int buffer_size)
{
    char buffer2[0x1000];
    struct kowhai_protocol_t prot;
    int bytes_required;
    kowhai_protocol_parse(buffer, buffer_size, &prot);
    if (prot.header.tree_id == TREE_ID_SETTINGS)
    {
        switch (prot.header.command)
        {
            case CMD_WRITE_DATA:
                printf("    CMD write data\n");
                kowhai_write(settings_descriptor, &settings, prot.header.symbol_count, prot.header.symbols, prot.payload.data, prot.payload.spec.size);
                prot.header.command = CMD_READ_DATA_ACK_END;
                kowhai_read(settings_descriptor, &settings, prot.header.symbol_count, prot.header.symbols, prot.payload.data, prot.payload.spec.size);
                kowhai_protocol_create(buffer2, 0x1000, &prot, &bytes_required);
                xpsocket_send(conn, buffer2, bytes_required);
                break;
            default:
                printf("unsupported command\n");
                break;
        }
    }
    else
        printf("unsupported tree id\n");
}

int main(int argc, char* argv[])
{
    int test_command = TEST_BASIC;

    union kowhai_symbol_t symbols1[] = {SYM_GENERAL, SYM_OVEN, SYM_TEMP};
    union kowhai_symbol_t symbols2[] = {SYM_GENERAL, SYM_OVEN, SYM_TIMEOUT};
    union kowhai_symbol_t symbols3[] = {SYM_GENERAL, SYM_FLUXCAPACITOR};
    union kowhai_symbol_t symbols4[] = {431, 12343};
    union kowhai_symbol_t symbols5[] = {SYM_GENERAL, SYM_STATUS};
    union kowhai_symbol_t symbols6[] = {SYM_GENERAL, SYM_FLUXCAPACITOR, SYM_GAIN};
    union kowhai_symbol_t symbols7[] = {SYM_GENERAL, SYM_FLUXCAPACITOR, SYM_COEFFICIENT};
    union kowhai_symbol_t symbols8[] = {SYM_GENERAL, KOWHAI_SYMBOL(SYM_FLUXCAPACITOR, 1), SYM_GAIN};
    union kowhai_symbol_t symbols9[] = {SYM_GENERAL, KOWHAI_SYMBOL(SYM_FLUXCAPACITOR, 1), KOWHAI_SYMBOL(SYM_COEFFICIENT, 3)};
    union kowhai_symbol_t symbols10[] = {SYM_GENERAL, SYM_FLUXCAPACITOR, KOWHAI_SYMBOL(SYM_COEFFICIENT, 3)};
    union kowhai_symbol_t symbols11[] = {SYM_GENERAL, SYM_OVEN};
    union kowhai_symbol_t symbols12[] = {SYM_GENERAL, KOWHAI_SYMBOL(SYM_FLUXCAPACITOR, 1)};

    int offset;
    int size;
    struct kowhai_node_t* node;

    uint8_t status;
    uint16_t temp;
    uint16_t timeout;
    uint32_t gain;
    float coeff;
    struct flux_capacitor_t flux_capacitor;

#ifdef KOWHAI_DBG
    printf("kowhai debugging enabled!\n");
#endif

    // determine test command
    if (argc > 1)
    {
        if (strcmp("server", argv[1]) == 0)
            test_command = TEST_PROTOCOL_SERVER;
        else if (strcmp("client", argv[1]) == 0)
            test_command = TEST_PROTOCOL_CLIENT;
    }

    // test tree parsing
    printf("test kowhai_get_node...\t\t\t");
    assert(kowhai_get_node(settings_descriptor, 3, symbols1, &offset, &node));
    assert(offset == 64);
    assert(kowhai_get_node(settings_descriptor, 3, symbols2, &offset, &node));
    assert(offset == 66);
    assert(kowhai_get_node(settings_descriptor, 2, symbols3, &offset, &node));
    assert(offset == 0);
    assert(!kowhai_get_node(settings_descriptor, 2, symbols4, &offset, &node));
    assert(kowhai_get_node(settings_descriptor, 3, symbols6, &offset, &node));
    assert(offset == 4);
    assert(kowhai_get_node(settings_descriptor, 3, symbols8, &offset, &node));
    assert(offset == sizeof(struct flux_capacitor_t) + 4);
    assert(kowhai_get_node(settings_descriptor, 3, symbols9, &offset, &node));
    assert(offset == sizeof(struct flux_capacitor_t) + 8 + 3 * 4);
    assert(kowhai_get_node(settings_descriptor, 3, symbols10, &offset, &node));
    assert(offset == 8 + 3 * 4);
    printf(" passed!\n");

    // test branch size
    printf("test kowhai_get_branch_size...\t\t");
    assert(kowhai_get_branch_size(settings_descriptor, &size));
    assert(size == sizeof(struct settings_data_t));
    printf(" passed!\n");

    // test read/write settings
    printf("test kowhai_read/kowhai_write...\t");
    status = 1;
    shadow.status = 0;
    assert(kowhai_write(shadow_descriptor, &shadow, 2, symbols5, &status, 1));
    assert(shadow.status == 1);
    status = 0;
    assert(kowhai_read(shadow_descriptor, &shadow, 2, symbols5, &status, 1));
    assert(status == 1);
    timeout = 999;
    settings.oven.timeout = 0;
    assert(kowhai_write(settings_descriptor, &settings, 3, symbols2, &timeout, sizeof(timeout)));
    assert(settings.oven.timeout == 999);
    timeout = 0;
    assert(kowhai_read(settings_descriptor, &settings, 3, symbols2, &timeout, sizeof(timeout)));
    assert(timeout == 999);
    flux_capacitor.frequency = 100; flux_capacitor.gain = 200;
    settings.flux_capacitor[0].frequency = 0; settings.flux_capacitor[0].gain = 0;
    assert(kowhai_write(settings_descriptor, &settings, 2, symbols3, &flux_capacitor, sizeof(flux_capacitor)));
    assert(settings.flux_capacitor[0].frequency == 100 && settings.flux_capacitor[0].gain == 200);
    flux_capacitor.frequency = 0; flux_capacitor.gain = 0;
    assert(kowhai_read(settings_descriptor, &settings, 2, symbols3, &flux_capacitor, sizeof(flux_capacitor)));
    assert(flux_capacitor.frequency == 100 && flux_capacitor.gain == 200);
    coeff = 999.9f;
    settings.flux_capacitor[1].coefficient[3] = 0;
    assert(kowhai_write(settings_descriptor, &settings, 3, symbols9, &coeff, sizeof(coeff)));
    assert(settings.flux_capacitor[1].coefficient[3] == 999.9f);
    coeff = 0;
    assert(kowhai_read(settings_descriptor, &settings, 3, symbols9, &coeff, sizeof(coeff)));
    assert(coeff == 999.9f);
    printf(" passed!\n");

    // test set/get settings
    printf("test kowhai_get_xxx/kowhai_set_xxx...\t");
    shadow.status = 0;
    assert(kowhai_set_char(shadow_descriptor, &shadow, 2, symbols5, 255));
    assert(shadow.status == 255);
    assert(kowhai_get_char(shadow_descriptor, &shadow, 2, symbols5, &status));
    assert(status == 255);
    settings.oven.temp = 0;
    assert(kowhai_set_int16(settings_descriptor, &settings, 3, symbols1, 999));
    assert(settings.oven.temp == 999);
    assert(kowhai_get_int16(settings_descriptor, &settings, 3, symbols1, &temp));
    assert(temp == 999);
    settings.oven.timeout = 0;
    assert(kowhai_set_int16(settings_descriptor, &settings, 3, symbols2, 999));
    assert(settings.oven.timeout == 999);
    assert(kowhai_get_int16(settings_descriptor, &settings, 3, symbols2, &timeout));
    assert(timeout == 999);
    settings.flux_capacitor[0].gain = 0;
    assert(kowhai_set_int32(settings_descriptor, &settings, 3, symbols6, 999));
    assert(settings.flux_capacitor[0].gain == 999);
    assert(kowhai_get_int32(settings_descriptor, &settings, 3, symbols6, &gain));
    assert(gain == 999);
    settings.flux_capacitor[0].coefficient[0] = 0;
    assert(kowhai_set_float(settings_descriptor, &settings, 3, symbols7, 999.9f));
    assert(settings.flux_capacitor[0].coefficient[0] ==  999.9f);
    assert(kowhai_get_float(settings_descriptor, &settings, 3, symbols7, &coeff));
    assert(coeff == 999.9f);
    printf(" passed!\n");

    // test server protocol
    if (test_command == TEST_PROTOCOL_SERVER)
    {
        printf("test server protocol...\n");
        xpsocket_init();
        xpsocket_serve(server_buffer_received, 0x1000);
        xpsocket_cleanup();
    }

    // test client protocol
    if (test_command == TEST_PROTOCOL_CLIENT)
    {
        xpsocket_handle conn;
        printf("test client protocol...\n");
        xpsocket_init();
        conn = xpsocket_init_client();
        if (conn != NULL)
        {
            char buffer[0x1000];
            int bytes_required;
            int received_size;
            struct kowhai_protocol_t prot;
            char value;
            struct oven_t oven = {123, 321};
            struct flux_capacitor_t flux_cap[2] = {{100, 200, {1, 2, 3, 4, 5, 6}}, {110, 210, {11, 12, 13, 14, 15, 16}}};
            // write and read oven.temp
            POPULATE_PROTOCOL(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 3, symbols1, DATA_TYPE_UINT16, 1, 0, sizeof(uint16_t), &temp);
            assert(kowhai_protocol_create(buffer, 0x1000, &prot, &bytes_required));
            temp = 25;
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, 0x1000);
            xpsocket_receive(conn, buffer, 0x1000, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 3);
            assert(memcmp(prot.header.symbols, symbols1, sizeof(union kowhai_symbol_t) * 3) == 0);
            assert(prot.payload.spec.type == DATA_TYPE_UINT16);
            assert(prot.payload.spec.count == 1);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(uint16_t));
            assert(*((uint16_t*)prot.payload.data) == temp);
            // write and read low byte of oven.temp
            POPULATE_PROTOCOL(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 3, symbols1, DATA_TYPE_UINT16, 1, 1, 1, &value);
            assert(kowhai_protocol_create(buffer, 0x1000, &prot, &bytes_required));
            value = 255;
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, 0x1000);
            xpsocket_receive(conn, buffer, 0x1000, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 3);
            assert(memcmp(prot.header.symbols, symbols1, sizeof(union kowhai_symbol_t) * 3) == 0);
            assert(prot.payload.spec.type == DATA_TYPE_UINT16);
            assert(prot.payload.spec.count == 1);
            assert(prot.payload.spec.offset == 1);
            assert(prot.payload.spec.size == 1);
            assert(*((char*)prot.payload.data) == value);
            // write and read oven
            POPULATE_PROTOCOL(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 2, symbols11, 0, 1, 0, sizeof(oven), &oven);
            assert(kowhai_protocol_create(buffer, 0x1000, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, 0x1000);
            xpsocket_receive(conn, buffer, 0x1000, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols11, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.count == 1);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(oven));
            assert(memcmp(prot.payload.data, &oven, sizeof(oven)) == 0);
            // write and read flux capacitor array
            POPULATE_PROTOCOL(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 2, symbols3, 0, 1, 0, sizeof(struct flux_capacitor_t) * 2, flux_cap);
            assert(kowhai_protocol_create(buffer, 0x1000, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, 0x1000);
            xpsocket_receive(conn, buffer, 0x1000, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols3, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.count == 1);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(struct flux_capacitor_t) * 2);
            assert(memcmp(prot.payload.data, flux_cap, sizeof(struct flux_capacitor_t) * 2) == 0);
            // write and read flux capacitor[1]
            POPULATE_PROTOCOL(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 2, symbols12, 0, 1, 0, sizeof(struct flux_capacitor_t), &flux_cap[1]);
            assert(kowhai_protocol_create(buffer, 0x1000, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, 0x1000);
            xpsocket_receive(conn, buffer, 0x1000, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols12, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.count == 1);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(struct flux_capacitor_t));
            assert(memcmp(prot.payload.data, &flux_cap[1], sizeof(struct flux_capacitor_t)) == 0);

            xpsocket_free_client(conn);
        }
        xpsocket_cleanup();
        printf("\t\t\t\t\t passed!");
    }

    return 0;
}
