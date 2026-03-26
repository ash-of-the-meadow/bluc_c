#include <stdlib.h>
#include <stdarg.h>
#include "jms_vector.h"
#include "stdbool.h"

typedef struct jms_vec_chunk
{
    /**
     * NULL if not yet initialized
     */
    void* data;

    /**
     * Destructor for any given element of the data. If NULL, then no destructor is called.
     */
    void (*destructor) (void* self);
} jms_vec_chunk;


struct jms_vector
{
    /**
     * Size of each element (in bytes)
     */
    int32_t elemSize;
    int32_t lastElemIndex;

    /*
     * How many elements we currently have space for.
     * An array of pointers.
     */
    int32_t maxElems;
    jms_vec_chunk* elements;

    /**
     * Whether or not to run the destructor and free each element of the vector
     *  when the vector is deleted.
     */
    bool doFreeElementsOnDelete;

    /**
     * A vector storing the "queries" that have been run on this vector (such as .where(xxx)).
     *  This is used to free the queries when the vector is deleted. We only need to free the
     *  vector itself, not the elements, because the elements in the query are managed by this
     *  vector.
     */
    jms_vector* queryVectors;
};

/**
 * @brief Initializes the vector.
 * @param elemSize - the size of each element in the vector.
 * @param isQueryVector - whether or not this vector is a query vector (i.e. a slice of another vector).
 */
static struct jms_vector* jms_vec_init_internal(int32_t elemSize, bool isQueryVector)
{
    int32_t     startElemStorage = 10;
    jms_vector* vec = malloc(sizeof(struct jms_vector));

    vec->elemSize       = elemSize;
    vec->lastElemIndex  = -1;
    vec->maxElems       = 10;
    vec->elements       = malloc(startElemStorage * sizeof(jms_vec_chunk));

    for (int32_t i = 0; i < startElemStorage; i++)
    {
        vec->elements[i].data = NULL;
        vec->elements[i].destructor = NULL;
    }

    vec->doFreeElementsOnDelete = true;

    // Don't infinitely recurse.
    if (!isQueryVector)
    {
        vec->queryVectors = jms_vec_init_internal(sizeof(jms_vector*), true);
    }

    return vec;
}

struct jms_vector* jms_vec_init(int32_t elemSize)
{
    return jms_vec_init_internal(elemSize, false);
}

void jms_vec_del(jms_vector* self)
{
    if (self->doFreeElementsOnDelete)
    {
        for (i32 i = 0; i < self->lastElemIndex; i++)
        {
            JMS_BORROWED_PTR(jms_vec_chunk) chunk
                = &self->elements[i];

            if (chunk->data != NULL
                    && chunk->destructor != NULL)
            {
                chunk->destructor(chunk->data);
            }
        }
    }

    free(self->elements);
    self->elements = NULL;

    free(self);
}

void jms_vec_static_defaultDestructor(void* self)
{
    free(self);
}

int32_t jms_vec_elemCount(jms_vector* self)
{
    return self->lastElemIndex + 1;
}

int32_t jms_vec_capacity(jms_vector* self)
{
    return self->maxElems;
}

/**
 * @brief Allocates more space for the vector.
 *<b> PRECONDITIONS:<b/><br/>
 *  - self->elements[self->lastElemIndex] is not yet populated with data.
 */
static void jms_vec_allocMoreSpace(jms_vector* self)
{
    self->maxElems
        *= 2;
    self->elements
        = realloc(
            self->elements,
            self->maxElems * sizeof(jms_vec_chunk));

    for (i32 i = self->lastElemIndex; i < self->maxElems; i++)
    {
        self->elements[i].data = NULL;
    }
}

void jms_vec_add(jms_vector* self, void* element, void (*destructor) (void* self))
{
    self->lastElemIndex++;

    if (self->lastElemIndex > self->maxElems/2)
    {
        jms_vec_allocMoreSpace(self);
    }

    JMS_BORROWED_PTR(jms_vec_chunk) chunk
        = &self->elements[self->lastElemIndex];
    chunk->data
        = element;
    chunk->destructor
        = destructor;
}

void jms_vec_addAll(jms_vector* self, i32 count, void (*destructor) (void* self), ...)
{
    va_list args;
    va_start(args, destructor);

    for (i32 i = 0; i < count; i++)
    {
        void* currentArg = va_arg(args, void*);
        jms_vec_add(self, currentArg, destructor);
    }

    va_end(args);
}

void* jms_vec_get(jms_vector* self, int32_t index)
{
    if (index > self->lastElemIndex)
    {
        return NULL;
    }
    
    return self->elements[index].data;
}

void* jms_vec_find(jms_vector* self, void* searchCriteria, bool (*comparer)(void*, void*))
{
    for (int32_t i = 0; i <= self->lastElemIndex; i++)
    {
        void* actualElement = self->elements[i].data;
        bool isEqual = comparer(searchCriteria, actualElement);

        if (isEqual)
        {
            return actualElement;
        }
    }

    return NULL;
}

JMS_BORROWED_PTR(jms_vector) jms_vec_where(jms_vector* self, void* searchCriteria, bool (*comparer)(void*, void*))
{
    jms_vector* results = jms_vec_init(self->elemSize);
    
    // Don't free the elements of the "slice" vector --
    //  they are managed by the original vector.
    results->doFreeElementsOnDelete = false;

    for (int32_t i = 0; i <= self->lastElemIndex; i++)
    {
        void* actualElement = self->elements[i].data;
        bool isEqual = comparer(searchCriteria, actualElement);

        if (isEqual)
        {
            // Do not use the destructor found in the chunk, because when
            //  the current vector is deleted, the elements would have been
            //  free'd twice.
            jms_vec_add(results, actualElement, NULL);
        }
    }

    return results;
}

/**
 * @param index - the index to shift all of the elements into
 *      (i.e. the empty space left by deleting an element) 
 */
static void jms_vec_shiftLeft(jms_vector* self, int32_t index)
{
    for (int32_t i = index + 1; i <= self->lastElemIndex; i++)
    {
        self->elements[i - 1].data = self->elements[i].data;
    }

    // If we didn't "null" this, we'd just end up with a duplicate
    //  reference to the pointer in maxElems - 2.
    self->elements[self->maxElems - 1].data = NULL;
}

static void jms_vec_shrink(jms_vector* self)
{
    int32_t newEndIndex = self->maxElems/2;
    self->maxElems = newEndIndex;
    self->elements = realloc(self->elements, newEndIndex * 
        sizeof(jms_vec_chunk));
}

// Helper function to partition the array
static int32_t partition(jms_vector *self, int32_t low, int32_t high, jms_vec_comparerDelegate sorter) {
    void* pivot = self->elements[high].data; // Choose the last element as the pivot
    int32_t i = low - 1;

    for (int32_t j = low; j < high; j++) {
        if (sorter(self->elements[j].data, pivot) <= 0) {
            i++;
            // Swap elements[i] and elements[j]
            void* temp = self->elements[i].data;
            self->elements[i].data = self->elements[j].data;
            self->elements[j].data = temp;
        }
    }

    // Swap elements[i + 1] and elements[high] (pivot)
    void* temp = self->elements[i + 1].data;
    self->elements[i + 1].data = self->elements[high].data;
    self->elements[high].data = temp;

    return i + 1;
}

// Recursive quicksort function
static void quicksort(jms_vector *self, int32_t low, int32_t high, jms_vec_comparerDelegate sorter) {
    if (low < high) {
        int32_t pi = partition(self, low, high, sorter);

        // Recursively sort elements before and after partition
        quicksort(self, low, pi - 1, sorter);
        quicksort(self, pi + 1, high, sorter);
    }
}

void jms_vec_sort(jms_vector *self, jms_vec_comparerDelegate sorter) {
    if (self->lastElemIndex > 0) {
        quicksort(self, 0, self->lastElemIndex - 1, sorter);
    }
}

void jms_vec_rem(jms_vector* self, int32_t index)
{
    void* data = self->elements[index].data;
    if (data != NULL)
    {
        free(data);
    }

    jms_vec_shiftLeft(self, index);

    if (self->lastElemIndex < self->maxElems/2)
    {
        jms_vec_shrink(self);
    }

    // set data to NULL *after* array shrink in case
    //  realloc assigns a new memory location, giving
    //  it a junk value
    self->elements[index].data = NULL;
    self->lastElemIndex--;
}
