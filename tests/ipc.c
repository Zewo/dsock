/*

  Copyright (c) 2016 Martin Sustrik

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

#include <assert.h>
#include <stdio.h>
#include <libdill.h>

#include "../dillsocks.h"

#include "create_connection.inc"

int main() {

    /* Open and close listening socket. */
    int lst = ipclisten("/tmp/test.ipc", 10);
    assert(lst >= 0);
    hclose(lst);

    /* Simple ping-pong test. */
    int s[2];
    create_ipc_connection(s);
    int rc = socksend(s[0], "ABC", 3, -1);
    assert(rc == 0);
    char buf[3];
    size_t sz;
    rc = sockrecv(s[1], buf, 3, &sz, -1);
    assert(rc == 0);
    assert(sz == 3 && buf[0] == 'A' && buf[1] == 'B' && buf[2] == 'C');
    rc = socksend(s[1], "DEF", 3, -1);
    assert(rc == 0);
    rc = sockrecv(s[0], buf, 3, &sz, -1);
    assert(rc == 0);
    assert(sz == 3 && buf[0] == 'D' && buf[1] == 'E' && buf[2] == 'F');
    rc = hclose(s[0]);
    assert(rc == 0);
    rc = hclose(s[1]);
    assert(rc == 0);

#if 0
    /* Receive less than requested amount of data when connection is closed. */
    create_ipc_connection(s);
    rc = socksend(s[0], "ZX", 2, -1);
    assert(rc == 0);
    rc = ipcclose(s[0], -1);
    assert(rc == 0);
    rc = sockrecv(s[1], buf, 3, &sz, -1);
    assert(rc == -1 && errno == ECONNRESET);
    assert(sz == 2 && buf[0] == 'Z' && buf[1] == 'X');
    rc = sockrecv(s[1], buf, 2, &sz, -1);
    assert(rc == -1 && errno == ECONNRESET);
    assert(sz == 0);
    rc = ipcclose(s[1], -1);
    assert(rc == 0);

    /* Test writing to a closed connection. */
    create_ipc_connection(s);
    rc = ipcclose(s[0], -1);
    assert(rc == 0);
    rc = msleep(now() + 100);
    assert(rc == 0);
    /* Send operation can be asynchronous on multiple levels. Thus, several
       writes may succeed before error is returned. */
    while(1) {
        rc = socksend(s[1], "ABC", 3, -1);
        if(rc == -1 && errno == ECONNRESET)
            break;
        assert(rc == 0);
    }
    rc = ipcclose(s[1], -1);
    assert(rc == 0);
#endif

    return 0;
}
