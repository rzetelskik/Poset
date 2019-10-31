#ifndef POSET_POSET_H
#define POSET_POSET_H

#ifdef __cplusplus
#include <cstdio>

#ifndef NDEBUG
#include <iostream>
#endif

namespace jnp1 {
    extern "C" {
#else
#include <stdbool.h>
#include <stdio.h>
#endif

        /*
         * Creates a new poset and returns its id.
         */
        unsigned long poset_new(void);

        /*
         * If a poset with the specified id exists, deletes it.
         */
        void poset_delete(unsigned long id);

        /*
         * If a poset with the specified id exists, returns its size.
         * Otherwise returns 0.
         */
        size_t poset_size(unsigned long id);

        /*
         * If a poset with the specified id exists and it does not contain
         * element value, adds the value to the poset.
         * The newly added element is not related to any of the already existing elements.
         * Returns true if the element has been added successfully. Otherwise returns false.
         */
        bool poset_insert(unsigned long id, char const *value);

        /*
         * If a poset with the specified id exists and contains the element value,
         * removes value and all its relations from the poset.
         * Returns true if the element has been successfully removed. Otherwise returns false.
         */
        bool poset_remove(unsigned long id, char const *value);

        /*
         * If a poset with the specified id exists, contains elements value1 and value2
         * and the elements are not related, extends the relation in such a way
         * that the element value1 preceeds the element value2.
         * Returns true if the relation has been extended. Otherwise returns false.
         */
        bool poset_add(unsigned long id, char const *value1, char const *value2);

        /*
         * If a poset with the specified id exists, contains elements value1 and value2,
         * value1 is preceding value2 and removing the relation inbetween them does not
         * violate the poset conditions, removes said relation.
         * Returns true if the relation has been modified. Otherwise returns false.
         */
        bool poset_del(unsigned long id, char const *value1, char const *value2);

        /*
         * Returns true if a poset with the specified id exists, contains the elements
         * value1 and value2 and the element value1 preceeds the element value2.
         * Otherwise returns false.
         */
        bool poset_test(unsigned long id, char const *value1, char const *value2);

        /*
         * If a poset with the specified id exists, removes all its elements and their relations.
         */
        void poset_clear(unsigned long id);

#ifdef __cplusplus
    }
}
#endif



#endif //POSET_POSET_H
