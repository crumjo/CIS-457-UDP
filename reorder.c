/**
 * Takes an array of packets and ensures that they are in order.
 *
 * @author Joshua Crum
 */

#include <stdio.h>
#include <stdlib.h>
#include "reorder.h"



/* Returns a 1 if the packets are in order. */
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


int order ( int len, struct packet *pack )
{
    
    for (int i = 0; i < len; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
//            printf ("Pack i: %d \t pack j: %d\n", pack[i] -> p_num, pack[j] -> p_num);
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
