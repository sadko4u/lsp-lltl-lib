/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 11 мая 2020 г.
 *
 * lsp-lltl-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-lltl-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-lltl-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/lltl/pphash.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/stdlib/string.h>

UTEST_BEGIN("lltl", pphash)

    void test_basic()
    {
        lltl::parray<char> k, v;
        lltl::pphash<char, char> h;

        char **ps, *xv, *ov, *s;

        printf("Testing basic functions...\n");

        // Check initial state
        UTEST_ASSERT(h.size() == 0);
        UTEST_ASSERT(h.capacity() == 0);
        UTEST_ASSERT(h.is_empty());
        UTEST_ASSERT(xv = ::strdup("test value"));

        // Get from empty
        UTEST_ASSERT(!(s = h.get("test")));

        // Put items first
        UTEST_ASSERT(ps = h.put("key1", NULL));
        UTEST_ASSERT(*ps = ::strdup("value1"));
        UTEST_ASSERT(h.put("key2", ::strdup("value2"), NULL));
        UTEST_ASSERT(h.put("key3", ::strdup("value3"), NULL));
        UTEST_ASSERT(h.put(NULL, ::strdup("null value"), NULL));
        UTEST_ASSERT(h.size() == 4);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Create items
        UTEST_ASSERT(h.create("key4", ::strdup("value4")));
        UTEST_ASSERT(!h.create("key1", xv));
        UTEST_ASSERT(h.create("key5", NULL));
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Check for existence
        UTEST_ASSERT(h.exists("key1"));
        UTEST_ASSERT(h.exists("key2"));
        UTEST_ASSERT(h.exists("key3"));
        UTEST_ASSERT(h.exists("key4"));
        UTEST_ASSERT(h.exists("key5"));
        UTEST_ASSERT(h.exists(NULL));
        UTEST_ASSERT(!h.exists("unexisting"));
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Check reads
        UTEST_ASSERT(s = h.get("key3"));
        UTEST_ASSERT(::strcmp(s, "value3") == 0);
        UTEST_ASSERT(s = h.get(NULL));
        UTEST_ASSERT(::strcmp(s, "null value") == 0);
        UTEST_ASSERT(!(s = h.get("unexisting")));
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Get with write-back
        UTEST_ASSERT(ps = h.wbget("key3"));
        UTEST_ASSERT(::strcmp(*ps, "value3") == 0);
        ::free(*ps);
        UTEST_ASSERT(*ps = ::strdup("new value3"));
        UTEST_ASSERT(s = h.get("key3"));
        UTEST_ASSERT(::strcmp(s, "new value3") == 0);
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Gets with defaults
        UTEST_ASSERT(s = h.dget("key3", xv));
        UTEST_ASSERT(::strcmp(s, "new value3") == 0);
        UTEST_ASSERT(!(s = h.dget("key5", xv)));
        UTEST_ASSERT(s = h.dget("unexisting", xv));
        UTEST_ASSERT(::strcmp(s, "test value") == 0);
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Replace items
        ov  = xv;
        UTEST_ASSERT(!h.replace("unexisting", xv, &ov));
        UTEST_ASSERT(ov == xv);
        UTEST_ASSERT(h.replace("key1", xv, &ov));
        UTEST_ASSERT(ov != NULL);
        UTEST_ASSERT(::strcmp(ov, "value1") == 0);
        ::free(ov);
        UTEST_ASSERT(h.size() == 6);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Remove items
        UTEST_ASSERT(!h.remove("unexisting", &ov));
        UTEST_ASSERT(h.remove("key2", &ov));
        UTEST_ASSERT(ov != NULL);
        UTEST_ASSERT(::strcmp(ov, "value2") == 0);
        ::free(ov);
        UTEST_ASSERT(h.size() == 5);
        UTEST_ASSERT(h.capacity() == 0x10);

        // Get keys
        UTEST_ASSERT(h.keys(&k));
        printf("hash keys:\n");
        for (size_t i=0, n=k.size(); i<n; ++i)
            printf("  %s\n", k.uget(i));
        k.flush();

        // Get values
        UTEST_ASSERT(h.values(&v));
        printf("hash values:\n");
        for (size_t i=0, n=v.size(); i<n; ++i)
            printf("  %s\n", v.uget(i));
        v.flush();

        // Get keys and values
        UTEST_ASSERT(h.items(&k, &v));
        UTEST_ASSERT(k.size() == v.size());

        printf("hash items:\n");
        for (size_t i=0, n=k.size(); i<n; ++i)
            printf("  %s = %s\n", k.uget(i), v.uget(i));
        k.flush();

        // Clear the hash
        h.clear();
        UTEST_ASSERT(h.size() == 0);
        UTEST_ASSERT(h.capacity() == 0x10);
        h.flush();
        UTEST_ASSERT(h.size() == 0);
        UTEST_ASSERT(h.capacity() == 0);

        // Drop values
        printf("freeing hash items\n");
        for (size_t i=0, n=v.size(); i<n; ++i)
        {
            char *data = v.uget(i);
            if (data != NULL)
            {
                printf("  freeing value: %s\n", data);
                ::free(data);
            }
        }
    }

    void test_large()
    {
        char buf[32], *s;
        lltl::pphash<char, char> h;

        printf("Generating large data...\n");
        for (size_t i=0; i<100000; ++i)
        {
            ::snprintf(buf, sizeof(buf), "%08lx", long(i));
            UTEST_ASSERT(h.put(buf, ::strdup(buf), NULL));
            if (!((i+1) % 10000))
                printf("  generated %d keys\n", int(i+1));
        }
        UTEST_ASSERT(h.size() == 100000);
        UTEST_ASSERT(h.capacity() == 0x20000);

        printf("Validating contents...\n");
        for (size_t i=0; i<100000; ++i)
        {
            ::snprintf(buf, sizeof(buf), "%08lx", long(i));
            UTEST_ASSERT(s = h.get(buf));
            UTEST_ASSERT(::strcmp(s, buf) == 0);
            ::free(s);
            if (!((i+1) % 10000))
                printf("  validated %d keys\n", int(i+1));
        }
        UTEST_ASSERT(h.size() == 100000);
        UTEST_ASSERT(h.capacity() == 0x20000);
    }

    UTEST_MAIN
    {
        test_basic();
        test_large();
    }

UTEST_END


