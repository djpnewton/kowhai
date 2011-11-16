#include "../src/kowhai.h"
#include "../src/kowhai_protocol.h"
#include "xpsocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define BUF_SIZE 0x1000
#define MAX_PACKET_SIZE 0x40

void server_buffer_received(xpsocket_handle conn, char* buffer, int buffer_size)
{
    char buffer2[BUF_SIZE];
    struct kowhai_protocol_t prot;
    int bytes_required;
    kowhai_protocol_parse(buffer, buffer_size, &prot);
    if (prot.header.tree_id == TREE_ID_SETTINGS)
    {
        switch (prot.header.command)
        {
            case CMD_WRITE_DATA:
                printf("    CMD write data\n");
                kowhai_write(settings_descriptor, &settings, prot.header.symbol_count, prot.header.symbols, prot.payload.spec.offset, prot.payload.data, prot.payload.spec.size);
                prot.header.command = CMD_WRITE_DATA_ACK;
                kowhai_read(settings_descriptor, &settings, prot.header.symbol_count, prot.header.symbols, prot.payload.spec.offset, prot.payload.data, prot.payload.spec.size);
                kowhai_protocol_create(buffer2, BUF_SIZE, &prot, &bytes_required);
                xpsocket_send(conn, buffer2, bytes_required);
                break;
            case CMD_READ_DATA:
            {
                int node_offset;
                int size, overhead, max_payload_size;
                struct kowhai_node_t* node;
                printf("    CMD read data\n");
                // get node information
                kowhai_get_node(settings_descriptor, prot.header.symbol_count, prot.header.symbols, &node_offset, &node);
                kowhai_get_node_size(node, &size);
                // get protocol overhead
                prot.header.command = CMD_READ_DATA_ACK;
                kowhai_protocol_get_overhead(&prot, &overhead);
                // setup max payload size and payload offset
                max_payload_size = MAX_PACKET_SIZE - overhead;
                prot.payload.spec.offset = 0;
                prot.payload.spec.type = node->data_type;
                // allocate payload buffer
                prot.payload.data = malloc(MAX_PACKET_SIZE - overhead);

                // send packets
                while (size > max_payload_size)
                {
                    prot.payload.spec.size = max_payload_size;
                    kowhai_read(settings_descriptor, &settings, prot.header.symbol_count, prot.header.symbols, prot.payload.spec.offset, prot.payload.data, prot.payload.spec.size);
                    kowhai_protocol_create(buffer2, BUF_SIZE, &prot, &bytes_required);
                    xpsocket_send(conn, buffer2, bytes_required);
                    // increment payload offset and decrement remaining payload size
                    prot.payload.spec.offset += max_payload_size;
                    size -= max_payload_size;
                }
                // send final packet
                prot.header.command = CMD_READ_DATA_ACK_END;
                prot.payload.spec.size = size;
                kowhai_read(settings_descriptor, &settings, prot.header.symbol_count, prot.header.symbols, prot.payload.spec.offset, prot.payload.data, prot.payload.spec.size);
                kowhai_protocol_create(buffer2, BUF_SIZE, &prot, &bytes_required);
                xpsocket_send(conn, buffer2, bytes_required);
                // free payload buffer
                free(prot.payload.data);
                break;
            }
            case CMD_READ_DESCRIPTOR:
            {
                int descriptor_offset;
                int size, overhead, max_payload_size;
                printf("    CMD read descriptor\n");
                // get descriptor size
                size = sizeof(settings_descriptor);
                // get protocol overhead
                prot.header.command = CMD_READ_DESCRIPTOR_ACK;
                kowhai_protocol_get_overhead(&prot, &overhead);
                // setup max payload size and payload offset
                max_payload_size = MAX_PACKET_SIZE - overhead;
                prot.payload.spec.offset = 0;
                prot.payload.spec.type = sizeof(settings_descriptor) / sizeof(settings_descriptor[0]); // TODO: ok we are futzing node-count into spec.type for now
                // allocate payload buffer
                prot.payload.data = malloc(MAX_PACKET_SIZE - overhead);

                // send packets
                while (size > max_payload_size)
                {
                    prot.payload.spec.size = max_payload_size;
                    memcpy(prot.payload.data, (char*)settings_descriptor + prot.payload.spec.offset, prot.payload.spec.size);
                    kowhai_protocol_create(buffer2, BUF_SIZE, &prot, &bytes_required);
                    xpsocket_send(conn, buffer2, bytes_required);
                    // increment payload offset and decrement remaining payload size
                    prot.payload.spec.offset += max_payload_size;
                    size -= max_payload_size;
                }
                // send final packet
                prot.header.command = CMD_READ_DESCRIPTOR_ACK_END;
                prot.payload.spec.size = size;
                memcpy(prot.payload.data, (char*)settings_descriptor + prot.payload.spec.offset, prot.payload.spec.size);
                kowhai_protocol_create(buffer2, BUF_SIZE, &prot, &bytes_required);
                xpsocket_send(conn, buffer2, bytes_required);
                // free payload buffer
                free(prot.payload.data);
                break;
            }
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
    assert(kowhai_get_node(settings_descriptor, 2, symbols12, &offset, &node));
    assert(offset == sizeof(struct flux_capacitor_t));
    printf(" passed!\n");

    // test get node size
    printf("test kowhai_get_node_size...\t\t");
    assert(kowhai_get_node_size(settings_descriptor, &size));
    assert(size == sizeof(struct settings_data_t));
    printf(" passed!\n");

    // test read/write settings
    printf("test kowhai_read/kowhai_write...\t");
    status = 1;
    shadow.status = 0;
    assert(kowhai_write(shadow_descriptor, &shadow, 2, symbols5, 0, &status, 1));
    assert(shadow.status == 1);
    status = 0;
    assert(kowhai_read(shadow_descriptor, &shadow, 2, symbols5, 0, &status, 1));
    assert(status == 1);
    timeout = 999;
    settings.oven.timeout = 0;
    assert(kowhai_write(settings_descriptor, &settings, 3, symbols2, 0, &timeout, sizeof(timeout)));
    assert(settings.oven.timeout == 999);
    timeout = 0;
    assert(kowhai_read(settings_descriptor, &settings, 3, symbols2, 0, &timeout, sizeof(timeout)));
    assert(timeout == 999);
    flux_capacitor.frequency = 100; flux_capacitor.gain = 200;
    settings.flux_capacitor[0].frequency = 0; settings.flux_capacitor[0].gain = 0;
    assert(kowhai_write(settings_descriptor, &settings, 2, symbols3, 0, &flux_capacitor, sizeof(flux_capacitor)));
    assert(settings.flux_capacitor[0].frequency == 100 && settings.flux_capacitor[0].gain == 200);
    flux_capacitor.frequency = 0; flux_capacitor.gain = 0;
    assert(kowhai_read(settings_descriptor, &settings, 2, symbols3, 0, &flux_capacitor, sizeof(flux_capacitor)));
    assert(flux_capacitor.frequency == 100 && flux_capacitor.gain == 200);
    coeff = 999.9f;
    settings.flux_capacitor[1].coefficient[3] = 0;
    assert(kowhai_write(settings_descriptor, &settings, 3, symbols9, 0, &coeff, sizeof(coeff)));
    assert(settings.flux_capacitor[1].coefficient[3] == 999.9f);
    coeff = 0;
    assert(kowhai_read(settings_descriptor, &settings, 3, symbols9, 0, &coeff, sizeof(coeff)));
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
        xpsocket_serve(server_buffer_received, BUF_SIZE);
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
            char buffer[BUF_SIZE];
            int bytes_required;
            int received_size;
            struct kowhai_protocol_t prot;
            int overhead;
            char value;
            struct oven_t oven = {0x0102, 321};
            struct flux_capacitor_t flux_cap[2] = {{100, 200, {1, 2, 3, 4, 5, 6}}, {110, 210, {11, 12, 13, 14, 15, 16}}};
            // write oven.temp
            temp = 25;
            POPULATE_PROTOCOL_WRITE(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 3, symbols1, DATA_TYPE_INT16, 0, sizeof(uint16_t), &temp);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_WRITE_DATA_ACK);
            assert(prot.header.symbol_count == 3);
            assert(memcmp(prot.header.symbols, symbols1, sizeof(union kowhai_symbol_t) * 3) == 0);
            assert(prot.payload.spec.type == DATA_TYPE_INT16);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(uint16_t));
            assert(*((uint16_t*)prot.payload.data) == temp);
            // write low byte of oven.temp
            value = 255;
            POPULATE_PROTOCOL_WRITE(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 3, symbols1, DATA_TYPE_INT16, 1, 1, &value);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_WRITE_DATA_ACK);
            assert(prot.header.symbol_count == 3);
            assert(memcmp(prot.header.symbols, symbols1, sizeof(union kowhai_symbol_t) * 3) == 0);
            assert(prot.payload.spec.type == DATA_TYPE_INT16);
            assert(prot.payload.spec.offset == 1);
            assert(prot.payload.spec.size == 1);
            assert(*((char*)prot.payload.data) == value);
            // double check oven.temp
            POPULATE_PROTOCOL_READ(prot, TREE_ID_SETTINGS, CMD_READ_DATA, 3, symbols1);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(*((uint16_t*)prot.payload.data) >> 8 == (uint8_t)value);
            // write oven
            POPULATE_PROTOCOL_WRITE(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 2, symbols11, 0, 0, sizeof(oven), &oven);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_WRITE_DATA_ACK);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols11, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(oven));
            assert(memcmp(prot.payload.data, &oven, sizeof(oven)) == 0);
            // write flux capacitor array
            POPULATE_PROTOCOL_WRITE(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 2, symbols3, 0, 0, sizeof(struct flux_capacitor_t) * 2, flux_cap);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_WRITE_DATA_ACK);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols3, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(struct flux_capacitor_t) * 2);
            assert(memcmp(prot.payload.data, flux_cap, sizeof(struct flux_capacitor_t) * 2) == 0);
            // write flux capacitor[1]
            POPULATE_PROTOCOL_WRITE(prot, TREE_ID_SETTINGS, CMD_WRITE_DATA, 2, symbols12, 0, 0, sizeof(struct flux_capacitor_t), &flux_cap[1]);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_WRITE_DATA_ACK);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols12, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(struct flux_capacitor_t));
            assert(memcmp(prot.payload.data, &flux_cap[1], sizeof(struct flux_capacitor_t)) == 0);
            // read oven.temp
            POPULATE_PROTOCOL_READ(prot, TREE_ID_SETTINGS, CMD_READ_DATA, 3, symbols1);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 3);
            assert(memcmp(prot.header.symbols, symbols1, sizeof(union kowhai_symbol_t) * 3) == 0);
            assert(prot.payload.spec.type == DATA_TYPE_INT16);
            assert(prot.payload.spec.offset == 0);
            assert(prot.payload.spec.size == sizeof(int16_t));
            assert(*((int16_t*)prot.payload.data) == 0x0102);
            // read flux capacitor array
            POPULATE_PROTOCOL_READ(prot, TREE_ID_SETTINGS, CMD_READ_DATA, 2, symbols3);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols3, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.offset == 0);
            kowhai_protocol_get_overhead(&prot, &overhead);
            assert(overhead == 17);
            assert(prot.payload.spec.size == MAX_PACKET_SIZE - overhead);
            assert(memcmp(prot.payload.data, flux_cap, prot.payload.spec.size) == 0);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DATA_ACK_END);
            assert(prot.header.symbol_count == 2);
            assert(memcmp(prot.header.symbols, symbols3, sizeof(union kowhai_symbol_t) * 2) == 0);
            assert(prot.payload.spec.type == 0);
            assert(prot.payload.spec.offset == MAX_PACKET_SIZE - overhead);
            assert(prot.payload.spec.size == sizeof(struct flux_capacitor_t) * 2 - prot.payload.spec.offset);
            assert(memcmp(prot.payload.data, (char*)flux_cap + prot.payload.spec.offset, prot.payload.spec.size) == 0);
            // read settings tree descriptor
            POPULATE_PROTOCOL_CMD(prot, TREE_ID_SETTINGS, CMD_READ_DESCRIPTOR);
            assert(kowhai_protocol_create(buffer, BUF_SIZE, &prot, &bytes_required));
            xpsocket_send(conn, buffer, bytes_required);
            memset(buffer, 0, BUF_SIZE);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DESCRIPTOR_ACK);
            assert(prot.payload.spec.type == sizeof(settings_descriptor) / sizeof(settings_descriptor[0])); //TODO: yes we are fuzting the node-count param into spec.type for now
            assert(prot.payload.spec.offset == 0);
            kowhai_protocol_get_overhead(&prot, &overhead);
            assert(overhead == 8);
            assert(prot.payload.spec.size == MAX_PACKET_SIZE - overhead);
            assert(memcmp(prot.payload.data, settings_descriptor, prot.payload.spec.size) == 0);
            xpsocket_receive(conn, buffer, BUF_SIZE, &received_size);
            kowhai_protocol_parse(buffer, received_size, &prot);
            assert(prot.header.tree_id == TREE_ID_SETTINGS);
            assert(prot.header.command == CMD_READ_DESCRIPTOR_ACK_END);
            assert(prot.payload.spec.type == sizeof(settings_descriptor) / sizeof(settings_descriptor[0])); //TODO: yes we are fuzting the node-count param into spec.type for now
            assert(prot.payload.spec.offset == MAX_PACKET_SIZE - overhead);
            assert(prot.payload.spec.size == sizeof(settings_descriptor) - prot.payload.spec.offset);
            assert(memcmp(prot.payload.data, (char*)settings_descriptor + prot.payload.spec.offset, prot.payload.spec.size) == 0);

            xpsocket_free_client(conn);
        }
        xpsocket_cleanup();
        printf("\t\t\t\t\t passed!\n");
    }

    return 0;
}
