/*
 * cstorage.cpp
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/lltl/cstorage.h>

#include <stdlib.h>

namespace lsp
{
    namespace lltl
    {
        void raw_cstorage::init(size_t n_sizeof)
        {
            vItems      = NULL;
            nCapacity   = 0;
            nItems      = 0;
            nSizeOf     = n_sizeof;
        }

        raw_cstorage::~raw_cstorage()
        {
            flush();
        }

        bool raw_cstorage::grow(size_t capacity)
        {
            if (capacity < 32)
                capacity        = 32;

            // Do aligned (re)allocation
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(::realloc(vItems, nSizeOf * capacity));
            if (ptr == NULL)
                return false;

            // Update pointer and capacity
            vItems          = ptr;
            nCapacity       = capacity;
            return true;
        }

        bool raw_cstorage::truncate(size_t capacity)
        {
            if (capacity < 32)
            {
                if (capacity == 0)
                {
                    flush();
                    return true;
                }
                capacity        = 32;
            }
            if (nCapacity <= capacity)
                return true;

            // Do aligned (re)allocation
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(::realloc(vItems, nSizeOf * capacity));
            if (ptr == NULL)
                return false;

            // Update pointer, capacity and size
            vItems          = ptr;
            nCapacity       = capacity;
            if (nItems > capacity)
                nItems          = capacity;
            return true;
        }

        uint8_t *raw_cstorage::slice(size_t idx, size_t size)
        {
            if (size <= 0)
                return NULL;

            size_t tail = idx + size;
            return (tail <= nItems) ? &vItems[idx * nSizeOf] : NULL;
        }

        bool raw_cstorage::xswap(size_t i1, size_t i2)
        {
            if ((i1 >= nItems) || (i2 >= nItems))
                return false;
            if (i1 != i2)
                uswap(i1, i2);
            return true;
        }

        void raw_cstorage::uswap(size_t i1, size_t i2)
        {
            uint8_t buf[0x200];
            uint8_t *a = &vItems[i1 * nSizeOf];
            uint8_t *b = &vItems[i2 * nSizeOf];

            for (size_t i=0; i<nSizeOf; i += sizeof(buf))
            {
                // Size of block
                size_t n = nSizeOf - i;
                if (n > sizeof(buf))
                    n = sizeof(buf);

                // Perform swap
                ::memcpy(buf, a, n);
                ::memmove(a, b, n);
                ::memcpy(b, buf, n);

                // Update pointers
                a  += sizeof(buf);
                b  += sizeof(buf);
            }
        }

        uint8_t *raw_cstorage::append(size_t n)
        {
            size_t size = nItems + n;
            if (size > nCapacity)
            {
                size_t dn = nCapacity + n;
                if (!grow(dn + (dn >> 1)))
                    return NULL;
            }

            uint8_t *ptr    = &vItems[nItems * nSizeOf];
            nItems          = size;
            return ptr;
        }

        uint8_t *raw_cstorage::append(size_t n, const void *src)
        {
            size_t size = nItems + n;
            if (size > nCapacity)
            {
                size_t dn = nCapacity + n;
                if (!grow(dn + (dn >> 1)))
                    return NULL;
            }

            uint8_t *ptr    = &vItems[nItems * nSizeOf];
            ::memcpy(ptr, src, n * nSizeOf);
            nItems          = size;
            return ptr;
        }

        uint8_t *raw_cstorage::set(size_t n, const void *src)
        {
            if (n > nCapacity)
            {
                if (!grow(n))
                    return NULL;
            }
            else if (n < (nCapacity >> 1))
            {
                if (!truncate(n))
                    return NULL;
            }

            ::memcpy(vItems, src, n * nSizeOf);
            nItems          = n;
            return vItems;
        }

        uint8_t *raw_cstorage::insert(size_t index, size_t n)
        {
            if ((index < 0) || (index > nItems))
                return NULL;
            if ((nItems + n) > nCapacity)
            {
                size_t dn = nCapacity + n;
                if (!grow(dn + (dn >> 1)))
                    return NULL;
            }
            uint8_t *res = &vItems[index * nSizeOf];
            if (index < nItems)
                ::memmove(&res[n*nSizeOf], res, (nItems - index)*nSizeOf);
            nItems += n;
            return res;
        }

        uint8_t *raw_cstorage::insert(size_t index, size_t n, const void *src)
        {
            if ((index < 0) || (index > nItems))
                return NULL;
            if ((nItems + n) > nCapacity)
            {
                size_t dn = nCapacity + n;
                if (!grow(dn + (dn >> 1)))
                    return NULL;
            }
            uint8_t *res = &vItems[index * nSizeOf];
            if (index < nItems)
                ::memmove(&res[n*nSizeOf], res, (nItems - index)*nSizeOf);
            ::memcpy(res, src, n * nSizeOf);

            nItems += n;
            return res;
        }

        void raw_cstorage::swap(raw_cstorage *src)
        {
            raw_cstorage tmp;
            tmp     = *this;
            *this   = *src;
            *src    = tmp;
        }

        void raw_cstorage::flush()
        {
            if (vItems != NULL)
            {
                ::free(vItems);
                vItems      = NULL;
            }
            nCapacity   = 0;
            nItems      = 0;
        }

        bool raw_cstorage::premove(const void *ptr, size_t n)
        {
            if (ptr == NULL)
                return false;
            uint8_t *src = static_cast<uint8_t *>(const_cast<void *>(ptr));
            if (src < vItems) // Pointer before array
                return false;

            size_t off  = src - vItems;
            size_t cap  = nItems * nSizeOf;
            if ((off >= cap) || (off % nSizeOf))
                return false;

            size_t last = off + n * nSizeOf;
            if (last < cap)
                ::memmove(&vItems[off], &vItems[last], cap - last);
            nItems     -= n;
            return true;
        }

        uint8_t *raw_cstorage::premove(const void *ptr, size_t n, void *dst)
        {
            if (ptr == NULL)
                return NULL;
            uint8_t *src = static_cast<uint8_t *>(const_cast<void *>(ptr));
            if (src < vItems) // Pointer before array
                return NULL;

            size_t off = src - vItems;
            size_t cap  = nItems * nSizeOf;

            if ((off >= cap) || (off % nSizeOf))
                return NULL;

            size_t last = off + n * nSizeOf;
            ::memmove(dst, src, n * nSizeOf);
            if (last < cap)
                ::memmove(src, &vItems[last], cap - last);
            nItems     -= n;
            return static_cast<uint8_t *>(dst);
        }

        uint8_t *raw_cstorage::premove(const void *ptr, size_t n, raw_cstorage *cs)
        {
            if (ptr == NULL)
                return NULL;
            uint8_t *src = static_cast<uint8_t *>(const_cast<void *>(ptr));
            if (src < vItems) // Pointer before array
                return NULL;
            size_t off = src - vItems;
            if ((off >= nItems) || (off % nSizeOf))
                return NULL;

            size_t cap  = nItems * nSizeOf;
            size_t last = off + n * nSizeOf;

            uint8_t *res = cs->append(n, src);
            if (res)
            {
                if (last < cap)
                    ::memmove(src, &vItems[last], cap - last);
                nItems     -= n;
            }
            return res;
        }

        bool raw_cstorage::iremove(size_t idx, size_t n)
        {
            size_t last = idx + n;
            if (last > nItems)
                return false;
            if (last < nItems)
                ::memmove(&vItems[idx * nSizeOf], &vItems[last * nSizeOf], (nItems - last) * nSizeOf);
            nItems     -= n;
            return true;
        }

        uint8_t    *raw_cstorage::iremove(size_t idx, size_t n, void *dst)
        {
            size_t last = idx + n;
            if (last > nItems)
                return NULL;

            uint8_t *src = &vItems[idx * nSizeOf];
            ::memmove(dst, src, n * nSizeOf);
            if (last < nItems)
                ::memmove(src, &vItems[last * nSizeOf], (nItems - last) * nSizeOf);
            nItems     -= n;
            return static_cast<uint8_t *>(dst);
        }

        uint8_t *raw_cstorage::iremove(size_t idx, size_t n, raw_cstorage *cs)
        {
            size_t last = idx + n;
            if (last > nItems)
                return NULL;

            uint8_t *src    = &vItems[idx * nSizeOf];
            uint8_t *res    = cs->append(n, src);
            if (res)
            {
                if (last < nItems)
                    ::memmove(src, &vItems[last * nSizeOf], (nItems - last) * nSizeOf);
                nItems     -= n;
            }
            return res;
        }

        uint8_t *raw_cstorage::pop(size_t n)
        {
            if (nItems < n)
                return NULL;

            nItems -= n;
            return &vItems[nItems * nSizeOf];
        }

        uint8_t *raw_cstorage::pop(size_t n, void *dst)
        {
            if (nItems < n)
                return NULL;

            nItems         -= n;
            size_t size     = nItems * nSizeOf;
            uint8_t *src    = &vItems[size];
            ::memcpy(dst, src, size);

            return static_cast<uint8_t *>(dst);
        }

        uint8_t *raw_cstorage::pop(size_t n, raw_cstorage *cs)
        {
            if (nItems < n)
                return NULL;

            size_t size     = nItems - n;
            uint8_t *res    = cs->append(n, &vItems[size]);
            if (res)
                nItems          = size;

            return res;
        }
    }
}
