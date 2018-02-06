/**
 * Takes an array of packets and ensures that they are in order.
 * Puts array of packets in order.
 *
 * @author Joshua Crum
 * @version 2/5/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include "reorder.h"



/**
 * Checks the order of the array of packets
 * by comparing the packet num to the loop variable.
 *
 * @param len the length of the buffer.
 * @param pack a buffer containing packet structs.
 * @return 1 if in order, 0 otherwise.
 */
int check_order ( int len, struct packet *pack )
{
    for (int i = 0; i < len; i++)
    {
        if (pack[i].p_num != i) {
            return 0;
        }
    }
    return 1;
}


/**
 * Puts the buffer of packets in order.
 *
 * @param len the length of the buffer.
 * @param pack a buffer containing packet structs.
 * @return 0 once the program is complete.
 */
int order ( int len, struct packet *pack )
{
    for (int i = 0; i < len; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if ((pack[i].p_num) > (pack[j].p_num))
            {
                struct packet tmp = pack[i];
                pack[i] = pack[j];
                pack[j] = tmp;
            }
        }
    }
    return 0;
}
