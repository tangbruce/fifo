#include <stdint.h>
#include <stdbool.h>
#include "fifo.h"


#define  exit_irq_unlock()		   				            //????????
#define  enter_irq_lock()				 		            //????????

#define __INLINE    inline


static fifo_t *sp_fifo = NULL;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static __INLINE uint32_t fifo_length(fifo_t * p_fifo)
{
	uint32_t tmp = p_fifo->read_pos;
	return p_fifo->write_pos - tmp;
}


#define FIFO_LENGTH() fifo_length(sp_fifo)  /**< Macro for calculating the FIFO length. */


uint32_t fifo_get_length(void )
{
	return FIFO_LENGTH();
}


/**@brief Put one byte to the FIFO. */
static __INLINE void __fifo_put(fifo_t * p_fifo, uint8_t byte)
{
	p_fifo->p_buf[p_fifo->write_pos % p_fifo->buf_size_mask] = byte;
	p_fifo->write_pos++;
}


/**@brief Look at one byte in the FIFO. */
static __INLINE void __fifo_peek(fifo_t * p_fifo, uint16_t index, uint8_t * p_byte)
{
	*p_byte = p_fifo->p_buf[(p_fifo->read_pos + index) % p_fifo->buf_size_mask];
}


/**@brief Get one byte from the FIFO. */
static __INLINE void __fifo_get(fifo_t * p_fifo, uint8_t * p_byte)
{
	__fifo_peek(p_fifo, 0, p_byte);
	p_fifo->read_pos++;
}


uint8_t fifo_init(fifo_t *p_fifo, uint8_t *p_buf, uint16_t buf_size)
{
	// Check buffer for null pointer.
	if (p_buf == NULL)
	{
		return 1;
	}

	sp_fifo				   = p_fifo; 
	sp_fifo->p_buf         = p_buf;
	sp_fifo->buf_size_mask = buf_size - 1;
	sp_fifo->read_pos      = 0;
	sp_fifo->write_pos     = 0;
	return 0;
}

uint8_t fifo_put(uint8_t byte)
{		
	if (!sp_fifo)
		return 1;
	
	if (FIFO_LENGTH() <= sp_fifo->buf_size_mask)
	{
		__fifo_put(sp_fifo, byte);
		return 0;
	}
	
	return 1;
}

uint8_t fifo_get(uint8_t * p_byte)
{
	if (!sp_fifo)
		return 1;
	
    if (FIFO_LENGTH() != 0)
    {
        __fifo_get(sp_fifo, p_byte);
        return 0;
	}
    return 1;

}

uint8_t fifo_peek(uint16_t index, uint8_t * p_byte)
{
	if (!sp_fifo)
		return 1;

	if (FIFO_LENGTH() > index)
	{
		__fifo_peek(sp_fifo, index, p_byte);
		return 0;
	}

	return 1;  
}

void fifo_flush(void )
{
	if (!sp_fifo)
		return ;
		
	sp_fifo->read_pos = sp_fifo->write_pos;
}

uint8_t fifo_read(uint8_t *p_byte_array, uint16_t *p_size)
{
	
	const uint16_t requested_len = (*p_size);
	uint16_t       index = 0, byte_count, read_size;

	if (!sp_fifo)
		return 1;

	byte_count = fifo_length(sp_fifo);
	read_size  = MIN(requested_len, byte_count);

	(*p_size) = byte_count;

	// Check if the FIFO is empty.
	if (byte_count == 0)
	{
		return 1;
	}

	// Check if application has requested only the size.
	if (p_byte_array == NULL)
	{
		return 0;
	}

	// Fetch bytes from the FIFO.
	//enter_irq_lock();
	while (index < read_size)
	{
		__fifo_get(sp_fifo, &p_byte_array[index++]);
	}
	//exit_irq_unlock();

	(*p_size) = read_size;
	
	return 0;
}

uint8_t fifo_write(uint8_t const * p_byte_array, uint16_t * p_size)
{

	const uint16_t requested_len   = (*p_size);
	uint16_t       index           = 0, available_count, write_size;

	if (!sp_fifo)
		return 1;
	
	available_count = sp_fifo->buf_size_mask - fifo_length(sp_fifo) + 1;
	write_size      = MIN(requested_len, available_count);
	(*p_size) = available_count;

	// Check if the FIFO is FULL.
	if (available_count == 0)
	{
		return 1;
	}

	// Check if application has requested only the size.
	if (p_byte_array == NULL)
	{
		return 0;
	}

	//Fetch bytes from the FIFO.
	while (index < write_size)
	{
		__fifo_put(sp_fifo, p_byte_array[index++]);
	}

	(*p_size) = write_size;

	return 0;
}



