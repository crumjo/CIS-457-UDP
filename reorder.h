/**
 * Takes an array of packets and ensures that they are in order.
 * Puts array of packets in order.
 *
 * @author Joshua Crum
 * @version 2/5/2018
 */

#pragma pack(1)
struct packet {
    int p_num;
    char buffer[1024];
};
#pragma pack(0)

#ifndef reorder_h
#define reorder_h

int check_order ( int len, struct packet *pack );
int order ( int len, struct packet *pack );

#endif /* reorder.h */
