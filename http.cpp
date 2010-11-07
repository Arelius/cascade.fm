#include <winsock2.h>
#include "utf.h"

// Not in use, trying curl.
// HTTP headers are done in ASCII, all unicode gets % encoded.

struct http_header
{
    const char* header;
    const char* content;
};

struct http_headers
{
    unsigned int num_headers;
    http_header* headers;
};

struct http_session
{
    SOCKET sock;
};

//TODO: replace asserts with soft errors.

void http_init()
{
    WORD wVer = MAKEWORD(2, 2);
    WSAData data;

    int err = WSAStartup(wVer, &data);
    assert(err == 0);
}

http_errors http_request(const wchar* server, const char* location, http_headers* headers)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    size_t buf_size = IdnToAscii(0, server, str_len(server), NULL, 0);
    char* idn_server = new char[buf_size];
    int err = IdnToAscii(0, server, str_len(server), idn_server, buf_size);
    assert(err);

    hostent* host = gethostbyname(idn_server);

    delete[] idn_server;

    assert(host);
    assert(host->h_addrtype == AF_INET);

    sockaddr_in addr;

    addr->sin_family = AF_INET;
    addr->sin_port = 80; // parse port from server var.

    assert(host->h_addr_list[i]);
    for(int i=0; host->h_addr_list[i] != NULL; i++)
    {
        addr->sin_addr = host->h_addr_list[i];
        break;
    }

    err = connect(sock, addr, sizeof(addr));
    assert(err == 0);



    closesocket(sock);
}
