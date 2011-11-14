#include "xpsocket.h"

#include <stdio.h>

struct xpsocket_t
{
    SOCKET sock;
};

int xpsocket_init()
{
#ifdef WIN32
    int err;
    WSADATA wsadata;
    err = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (err != 0)
    {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return 0;
    }
#endif
    return 1;
}

void xpsocket_cleanup()
{
#ifdef WIN32
    WSACleanup();
#endif
}

int xpsocket_serve(xpsocket_receive_callback buffer_received, int buffer_size)
{
    struct xpsocket_t conn;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in service;
    SOCKET acc_socket = SOCKET_ERROR;
    int bytes_received;
    char* recv_buffer = malloc(buffer_size);

    if (sock == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld.\n", WSAGetLastError());
        return 0;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(55555);
    
    if (bind(sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("bind() failed: %ld.\n", WSAGetLastError());
        closesocket(sock);
        return 0;
    }

    if (listen(sock, 1) == SOCKET_ERROR)
    {
        printf("listen(): Error listening on socket %ld.\n", WSAGetLastError());
        return 0;
    }

    while (1)
    {
        while (acc_socket == SOCKET_ERROR)
            acc_socket = accept(sock, NULL, NULL);
        printf("client connected\n");
        sock = acc_socket;
        break;
    }

    while (1)
    {
        bytes_received = recv(sock, recv_buffer, buffer_size, 0);
        if (bytes_received > 0)
        {
            printf("  received %d bytes\n", bytes_received);
            conn.sock = sock;
            buffer_received(&conn, recv_buffer, bytes_received);
        }
        else if (bytes_received == 0)
        {
            printf("connection closed\n");
            break;
        }
        else
        {
            printf("recv(): Error on socket %ld.\n", WSAGetLastError());
            return 0;
        }
    }

    return 1;
}

int xpsocket_send(xpsocket_handle conn, char* buffer, int size)
{
    int bytes_sent;

    bytes_sent = send(conn->sock, buffer, size, 0);

    if (bytes_sent == SOCKET_ERROR)
    {
        printf("send() error %ld.\n", WSAGetLastError());
        return 0;
    }

    printf("  sent %d bytes\n", bytes_sent);

    return 1;
}

int xpsocket_receive(xpsocket_handle conn, char* buffer, int buffer_size, int* received_size)
{
    *received_size = recv(conn->sock, buffer, buffer_size, 0);

    if (*received_size > 0)
    {
        printf("  received %d bytes\n", *received_size);
    }
    else if (*received_size == 0)
    {
        printf("connection closed\n");
    }
    else
    {
        printf("recv(): Error on socket %ld.\n", WSAGetLastError());
        return 0;
    }

    return 1;
}

xpsocket_handle xpsocket_init_client()
{
    struct sockaddr_in service;
    struct xpsocket_t* xpsock = malloc(sizeof(struct xpsocket_t));
    if (xpsock == NULL)
        return NULL;

    xpsock->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (xpsock->sock == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld.\n", WSAGetLastError());
        goto cleanup;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(55555);
    
    if (connect(xpsock->sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("connect() failed: %ld.\n", WSAGetLastError());
        closesocket(xpsock->sock);
        goto cleanup;
    }

    return xpsock;

cleanup:

    if (xpsock != NULL)
        free(xpsock);

    return NULL;
}

void xpsocket_free_client(xpsocket_handle conn)
{
    if (shutdown(conn->sock, SD_BOTH) == SOCKET_ERROR)
    {
        printf("shutdown() failed with error: %d\n", WSAGetLastError());
    }

    closesocket(conn->sock);
    free(conn);
}
