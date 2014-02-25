/**********************************************************************************************************
 *
 * 使用j1939.dat中数据(从安凯录制) 解析j1939协议
 *
 **********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>

#include "typedef.h"
#include "j1939.h"

#define J1939_BUS   ("j1939.dat")

static void parse_a_line(const char *buf_line);
static void make_can_from_text(CAN *can, const char *buf_line);

static float randf(void);
static void make_simulating_data(RECVED_MSG *recv_msg);

extern int main(void)
{
    char buf_line[1024];
    FILE *j1939_bus = NULL;
    j1939_bus = fopen(J1939_BUS, "r");

    j1939_init();

    do
    {
        bzero(buf_line, 1024);
        fgets(buf_line, 1024, j1939_bus);
        if(feof(j1939_bus))
        {
            /* 回滚 */
            fseek(j1939_bus, 0l, SEEK_SET);

            /* break; */
        } 
        parse_a_line(buf_line); 
        usleep(1000 * 1000); /* 1 s */
    }while(1);

    fclose(j1939_bus);
    
    return 0;
}

static void parse_a_line(const char *buf_line)
{
    CAN can;
    RECVED_MSG recv_msg = {0};

    make_can_from_text(&can, buf_line); 
    enqueue_can_to_rx_fifo(&can); 
    
    pull_msg_from_stack(&recv_msg); 

    /* TODO：使用真实的CAN数据而非此处模拟的数据 */
    make_simulating_data(&recv_msg);
    print_recved_msg(&recv_msg);
}

static void make_can_from_text(CAN *can, const char *buf_line)
{
    int i = 0;
    int canNum = 0;
    unsigned int id = 0;
    int dlc = 0;
    unsigned int data[8];

    bzero(data, sizeof(int) * 8);

    sscanf(buf_line, "  can%d  %x   [%d]  %x %x %x %x %x %x %x %x %x\n", 
            &canNum, &id, &dlc, 
            &data[0],
            &data[1],
            &data[2],
            &data[3],
            &data[4],
            &data[5],
            &data[6],
            &data[7]);

    can->id = id;
    can->dlc = dlc;

    /* 小端 */
    for(i=0;i<8;i++)
    {
        can->data[i] = (unsigned char)(data[7 - i]);
    } 
    
    //print_can(can);
}

static void make_simulating_data(RECVED_MSG *recv_msg)
{
    static F32 total_distance = 1000;
    static F32 total_oil_use = 500;
    static F32 oil_left = 80;

    total_distance += randf() + 1;
    total_oil_use += 5 * (randf() + 1);
    oil_left -= 0.11 * (randf() + 1);

    recv_msg->total_distance = total_distance;
    recv_msg->speed = 90 + 10 * randf();
    recv_msg->roate_speed = 50 + 10 * randf();
    recv_msg->water_temperature = 70 + 5 * randf();
    recv_msg->oil_left = oil_left;
    recv_msg->oil_valid = 60 + 10 * randf();
    recv_msg->oil_used = 1 + randf();
    recv_msg->total_oil_use = total_oil_use;
    recv_msg->oil_press = 100 + 5 * randf();
}

static float randf(void)
{
    float randf = 0;

    randf = 2.0 * rand() / RAND_MAX - 1;

    return randf;
}

