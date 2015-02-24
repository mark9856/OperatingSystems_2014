/* shared.h */

#define BUFFER_SIZE 8

struct shared_data
{
  int buffer[BUFFER_SIZE];   /* circular array */
  int in;   /* index of next available producer array slot */
  int out;  /* index of next array slot to consume from */
  int count; /* number of elements in the buffer */
};

