/*
 * cstorage.h
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_LLTL_CSTORAGE_H_
#define LSP_PLUG_IN_LLTL_CSTORAGE_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace lsp
{
    namespace lltl
    {
        struct raw_cstorage
        {
            public:
                uint8_t    *vItems;
                size_t      nCapacity;
                size_t      nItems;
                size_t      nSizeOf;

            public:
                ~raw_cstorage();

            public:
                void        init(size_t n_sizeof);
                bool        grow(size_t capacity);
                bool        truncate(size_t capacity);
                void        swap(raw_cstorage *src);
                bool        xswap(size_t i1, size_t i2);
                void        uswap(size_t i1, size_t i2);
                void        flush();

                uint8_t    *slice(size_t idx, size_t size);

                uint8_t    *set(size_t n, const void *src);
                uint8_t    *append(size_t n);
                uint8_t    *append(size_t n, const void *src);
                uint8_t    *insert(size_t index, size_t n);
                uint8_t    *insert(size_t index, size_t n, const void *src);

                uint8_t    *pop(size_t n);
                uint8_t    *pop(size_t n, void *dst);
                uint8_t    *pop(size_t n, raw_cstorage *cs);
                bool        premove(const void *ptr, size_t n);
                bool        premove(const void *ptr, size_t n, void *dst);
                bool        premove(const void *ptr, size_t n, raw_cstorage *cs);
                bool        iremove(size_t idx, size_t n);
                bool        iremove(size_t idx, size_t n, void *dst);
                bool        iremove(size_t idx, size_t n, raw_cstorage *cs);
        };

        template <class T>
            class cstorage
            {
                private:
                    cstorage(const cstorage<T> &src);                               // Disable copying
                    cstorage<T> & operator = (const cstorage<T> & src);             // Disable copying

                private:
                    mutable raw_cstorage    v;

                    inline T *cast(void *ptr)                                       { return reinterpret_cast<T *>(ptr); }
                    inline const T *ccast(const void *ptr)                          { return reinterpret_cast<const T *>(ptr); }

                public:
                    explicit inline cstorage()                                      { v.init(sizeof(T));                }
                    ~cstorage() {};

                public:
                    // Size and capacity
                    size_t size() const                                             { return v.nItems;                  }
                    size_t capacity() const                                         { return v.nCapacity;               }

                public:
                    // Whole manipulations
                    inline void clear()                                             { v.nItems  = 0;                    }
                    inline void flush()                                             { v.flush();                        }
                    inline void truncate()                                          { v.flush();                        }
                    inline void truncate(size_t size)                               { v.truncate(size);                 }
                    inline void reserve(size_t capacity)                            { v.grow(capacity);                 }
                    inline void swap(cstorage<T> &src)                              { v.swap(&src.v);                   }
                    inline void swap(cstorage<T> *src)                              { v.swap(&src->v);                  }

                public:
                    // Accessing elements (non-const)
                    inline T *get(size_t idx)                                       { return (idx < v.nItems) ? cast(&v.vItems[idx * v.nSizeOf]) : NULL; }
                    inline T *uget(size_t idx)                                      { return cast(&v.vItems[idx * v.nSizeOf]);  }
                    inline T *first()                                               { return (v.nItems > 0) ? cast(v.vItems) : NULL; }
                    inline T *last()                                                { return (v.nItems > 0) ? cast(&v.vItems[(v.nItems - 1) * v.nSizeOf]) : NULL;   }
                    inline T *array()                                               { return cast(v.vItems); }
                    inline T *slice(size_t idx, size_t size)                        { return cast(v.slice(idx, size));  }

                    inline const T *get(size_t idx) const                           { return (idx < v.nItems) ? ccast(&v.vItems[idx * v.nSizeOf]) : NULL; }
                    inline const T *at(size_t idx) const                            { return ccast(&v.vItems[idx * v.nSizeOf]); }
                    inline const T *first() const                                   { return (v.nItems > 0) ? ccast(v.vItems) : NULL; }
                    inline const T *last() const                                    { return (v.nItems > 0) ? ccast(&v.vItems[(v.nItems - 1) * v.nSizeOf]) : NULL;  }
                    inline const T *array() const                                   { return ccast(v.vItems); }
                    inline const T *slice(size_t idx, size_t size) const            { return cast(v.slice(idx, size));  }

                public:
                    // Single modifications
                    inline T *append()                                              { return cast(v.append(1));         }
                    inline T *add()                                                 { return cast(v.append(1));         }
                    inline T *push()                                                { return cast(v.append(1));         }
                    inline T *prepend()                                             { return cast(v.insert(0, 1));      }
                    inline T *insert(size_t idx)                                    { return cast(v.insert(idx, 1));    }

                    inline T *pop()                                                 { return cast(v.pop(1));            }
                    inline bool remove(size_t idx)                                  { return v.iremove(idx, 1);         }
                    inline bool premove(const T *ptr)                               { return v.premove(ptr, 1);         }

                    inline bool xswap(size_t i1, size_t i2)                         { return v.xswap(i1, i2);           }
                    inline void uswap(size_t i1, size_t i2)                         { v.uswap(i1, i2);                  }

                public:
                    // Multiple modifications
                    inline T *append_n(size_t n)                                    { return cast(v.append(n));         }
                    inline T *add_n(size_t n)                                       { return cast(v.append(n));         }
                    inline T *push_n(size_t n)                                      { return cast(v.append(n));         }
                    inline T *prepend_n(size_t n)                                   { return cast(v.insert(0, n));      }
                    inline T *insert_n(size_t idx, size_t n)                        { return cast(v.insert(idx, n));    }

                    inline T *pop_n(size_t n)                                       { return cast(v.pop(n));            }
                    inline bool remove_n(size_t idx, size_t n)                      { return v.iremove(idx, n);         }
                    inline bool premove_n(const T *ptr, size_t n)                   { return v.premove(ptr, n);         }

                public:
                    // Single modifications with data copying (pointer source)
                    inline T *append(const T *x)                                    { return cast(v.append(1, x));      }
                    inline T *add(const T *x)                                       { return cast(v.append(1, x));      }
                    inline T *push(const T *x)                                      { return cast(v.append(1, x));      }
                    inline T *prepend(const T *x)                                   { return cast(v.insert(0, 1, x));   }
                    inline T *insert(size_t idx, const T *x)                        { return cast(v.insert(idx, 1, x)); }

                    inline T *pop(T *x)                                             { return cast(v.pop(1, x));         }
                    inline bool remove(size_t idx, T *x)                            { return v.iremove(idx, 1, x);      }
                    inline bool premove(const T *ptr, T *x)                         { return v.premove(ptr, 1, x);      }

                public:
                    // Single modifications with data copying (reference source)
                    inline T *append(const T &v)                                    { return append(&v);                }
                    inline T *add(const T &v)                                       { return add(&v);                   }
                    inline T *push(const T &v)                                      { return push(&v);                  }
                    inline T *prepend(const T &v)                                   { return prepend(&v);               }
                    inline T *insert(size_t idx, const T &v)                        { return insert(idx, &v);           }

                    inline T *pop(T &v)                                             { return pop(&v);                   }

                public:
                    // Multiple modifications with data copying
                    inline T *append_n(size_t n, const T *x)                        { return cast(v.append(n, x));      }
                    inline T *add_n(size_t n, const T *x)                           { return cast(v.append(n, x));      }
                    inline T *push_n(size_t n, const T *x)                          { return cast(v.append(n, x));      }
                    inline T *prepend_n(size_t n, const T *x)                       { return cast(v.insert(0, n, x));   }
                    inline T *insert_n(size_t idx, size_t n, const T *x)            { return cast(v.insert(idx, n, x)); }

                    inline T *pop_n(size_t n, T *x)                                 { return cast(v.pop(n, x));         }
                    inline bool remove_n(size_t idx, size_t n, T *x)                { return v.iremove(idx, n, x);      }
                    inline bool premove_n(const T *ptr, size_t n, T *x)             { return v.premove(ptr, n, x);      }

                public:
                    // Collection-based modifications (pointer-based)
                    inline T *set(const cstorage<T> *x)                             { return cast(v.set(x->v.nItems, x->v.vItems));             }
                    inline T *append(const cstorage<T> *x)                          { return cast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T *add(const cstorage<T> *x)                             { return cast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T *push(const cstorage<T> *x)                            { return cast(v.append(x->v.nItems, x->v.vItems));          }
                    inline T *prepend(const cstorage<T> *x)                         { return cast(v.insert(0, x->v.nItems, x->v.vItems));       }
                    inline T *insert(size_t idx, const cstorage<T> *x)              { return cast(v.insert(idx, x->v.nItems, x->v.vItems));     }

                    inline T *pop(cstorage<T> *x)                                   { return cast(v.pop(1, &x->v));                             }
                    inline bool remove(size_t idx, cstorage<T> *x)                  { return v.iremove(idx, 1, &x->v);                          }
                    inline bool premove(const T *ptr, cstorage<T> *x)               { return v.premove(ptr, 1, &x->v);                          }

                    inline T *pop_n(size_t n, cstorage<T> *x)                       { return cast(v.pop(n, &x->v));                             }
                    inline bool remove_n(size_t idx, size_t n, cstorage<T> *x)      { return v.iremove(idx, n, &x->v);                          }
                    inline bool premove(const T *ptr, size_t n, cstorage<T> *x)     { return v.premove(ptr, n, &x->v);                          }

                public:
                    // Operators
                    inline T *operator[](size_t idx)                                { return get(idx);                  }
                    inline const T *operator[](size_t idx) const                    { return get(idx);                  }
            };
    }
}

#endif /* LSP_PLUG_IN_LLTL_CSTORAGE_H_ */
