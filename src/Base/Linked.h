#ifndef __LINKED_H__
#define __LINKED_H__

/// Simple linked-list container
/*! Sometimes, the standard containers cannot be used: e.g. in statically initialized instances,
    if the container also uses statically initialized members, it can lead to race condition.
    \note   Such a container is initialized once, and cannot be re-used. */
template <class T>
class Linked
{
 public:
    /*! \warning    This constructor must not initialize the only member 'next' to prevent
        race condition. It is initialized to zero by the memory handling sysytem first time.
        So, if it is used on the stack, call the member function Linked::Reset() to do it. */
    Linked(void)
    {
    }

    void Reset(void)
    {
        first = 0;
    }

    void insert(T * p)
    {
        p->next = first;
        first = p;
    }

    class iterator
    {
     friend class Linked;

        iterator(T * p):
            ptr(p)
        {
        }

     public:
        iterator(const iterator & other):
            ptr(other.ptr)
        {
        }

        T * operator->()
        {
            return ptr;
        }

        T & operator*()
        {
            return *ptr;
        }

        iterator & operator++()
        {
            if (ptr)
                ptr = ptr->next;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==(const iterator & other) const
        {
            return ptr == other.ptr;
        }

        bool operator!=(const iterator & other) const
        {
            return ptr != other.ptr;
        }

     private:
        T * ptr;
    };

    iterator begin(void)
    {
        return first;
    }

    iterator end(void)
    {
        return 0;
    }

 private:
    T * first;
};

#endif /* __LINKED_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
