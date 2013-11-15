#include "pl.h"

static void build_and_enqueue_a_can(const PDU *ptr_pdu);
static void build_a_can(CAN *ptr_can, const PDU *ptr_pdu);

static void parse_a_can(PDU *ptr_pdu, const CAN *ptr_can);

extern void pl_init(void)
{
    return;
}

extern void build_and_enqueue_cans(const PDU *ptr_pdu, INT32 length)
{
    CAN *ptr_cans = NULL;
    INT32 i = 0;

    ptr_cans = (CAN *) malloc (sizeof(CAN) * length);
    for(i=0;i<length;i++)
    {
        build_and_enqueue_a_can(&ptr_pdu[i]);
    }

    free(ptr_cans);
    ptr_cans=NULL;

    return;
}

extern void dequeue_and_parse_cans(PDU *ptr_pdu, INT32 *ptr_length)
{
    INT32 i = 0;
    CAN can = {0};

    while(true)
    {
        dequeue_can_from_rx_fifo(&can);

        if(need_follow_pdu(ptr_pdu))
        {
            printf("%s,%d:not Implement MUL PKGs.\n", __FILE__, __LINE__);
            exit(1);
        }
        else
        {
            parse_a_can(ptr_pdu, &can);
            *ptr_length = 1;
            break;
        }
        i++;
    }

    return;
}

static void build_and_enqueue_a_can(const PDU *ptr_pdu)
{
    CAN can = {0};

    build_a_can(&can, ptr_pdu);

    enqueue_can_to_tx_fifo(&can);

    return;
}

static void build_a_can(CAN *ptr_can, const PDU *ptr_pdu)
{
    INT32 i = 0;

    ptr_can->dlc = 8;                                           /* PDU中未实现长度位,故使用最长的CAN包 */

    ptr_can->id = (ptr_pdu->p_r_dp<<24)
        | (ptr_pdu->pf << 16)
        | (ptr_pdu->ps << 8)
        | (ptr_pdu->sa);

    ptr_can->id |= 0x80000000;                                  /* Linux kernel 扩展帧标记 */

    for(i=0;i<(ptr_can->dlc);i++)
    {
        ptr_can->data[i] = ptr_pdu->data[i];
    }

#if (DEBUG_DETAIL & PP_DEBUG)
    printf("%s,%d:buidl a can from a pdu,the pdu is:\n", __FILE__, __LINE__);
    print_pdu(ptr_pdu);
    printf("%s,%d:the can is:\n", __FILE__, __LINE__);
    print_can(ptr_can);
#endif

    return;
}

static void parse_a_can(PDU *ptr_pdu, const CAN *ptr_can)
{
    INT32 i;

    ptr_pdu->p_r_dp = (UINT8)((ptr_can->id >> 24) & 0x0000000F);
    ptr_pdu->pf = (UINT8)((ptr_can->id >> 16) & 0x000000FF);
    ptr_pdu->ps = (UINT8)((ptr_can->id >> 8) & 0x000000FF);
    ptr_pdu->sa = (UINT8)(ptr_can->id & 0x000000FF);

    for(i=0;i<8/*(ptr_can->dlc)*/;i++)
    {
        ptr_pdu->data[i] = ptr_can->data[i];
    }

#if (DEBUG_DETAIL & PP_DEBUG)
    printf("%s,%d:parse a pdu from a can,the can is:\n", __FILE__, __LINE__);
    print_can(ptr_can);
    printf("%s,%d:the pdu is:\n", __FILE__, __LINE__);
    print_pdu(ptr_pdu);
#endif

    return;
}

extern void print_can(const CAN *ptr_can)
{
    INT32 i = 0;
    INT32 length = 0;

    printf("id:0x%08x\n", ptr_can->id);
    printf("dlc:%d\n", ptr_can->dlc);

    length = 8;
    for(i=0;i<length;i++)
    {
        printf("data[%d]=0x%02x\n", i, ptr_can->data[i]);
    }

    return;
}

