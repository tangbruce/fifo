#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
 
/**@brief   A FIFO instance structure.
 * @details Keeps track of which bytes to read and write next.
 *          Also, it keeps the information about which memory is allocated for the buffer
 *          and its size. This structure must be initialized by app_fifo_init() before use.
 */
typedef struct
{
    uint8_t			*p_buf;				/**< Pointer to FIFO buffer memory.                      */
    uint16_t		buf_size_mask;   	/**< Read/write index mask. Also used for size checking. */
    uint32_t		read_pos;        	/**< Next read position in the FIFO buffer.              */
    uint32_t		write_pos;       	/**< Next write position in the FIFO buffer.             */
}fifo_t;

extern uint8_t fifo_init(fifo_t *p_fifo, uint8_t *p_buf, uint16_t buf_size);

extern uint8_t fifo_put(uint8_t byte);

extern uint8_t fifo_get(uint8_t *p_byte);

extern uint8_t fifo_peek(uint16_t index, uint8_t *p_byte);

extern void fifo_flush(void );

extern uint8_t fifo_read(uint8_t *p_byte_array, uint16_t *p_size);

extern uint8_t fifo_write(uint8_t const *p_byte_array, uint16_t *p_size);

extern  uint32_t fifo_get_length(void );
#ifdef __cplusplus
}
#endif

#endif 

/** @} */

