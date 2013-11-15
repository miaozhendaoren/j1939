#include "dl.h"

static void build_single_pdu(PDU *ptr_pdu, const MSG *ptr_msg);
static INT32 get_pgn(const PDU *ptr_pdu);

extern void dl_init(void)
{
    return;
}

extern void build_pdu(PDU *ptr_pdu, INT32 *ptr_length, const MSG *ptr_msg)
{
    INT32 can_nums = 0;

    can_nums = get_need_can_nums(ptr_msg->data_length);
    assert(can_nums > 0);

    if(can_nums > 1)
    {
        printf("%s,%d:not Implement MUL PKGs(%d).\n", __FILE__, __LINE__, can_nums);
        exit(0);
    }

    build_single_pdu(ptr_pdu, ptr_msg);
    *ptr_length = 1;

    return;
}

extern void parse_pdu(MSG *ptr_msg, const PDU *ptr_pdu, INT32 length)
{
    INT32 i = 0;

    if(length > 1)
    {
        printf("%s,%d:not Implement MUL PKGs.\n", __FILE__, __LINE__);
        exit(0);
    }

    ptr_msg->data_length = 8;                                     /* 单包 8字节 */
    ptr_msg->dp = (0x01) & ptr_pdu->p_r_dp;
    ptr_msg->pf = ptr_pdu->pf;
    ptr_msg->ps = ptr_pdu->ps;
    ptr_msg->p = ptr_pdu->p_r_dp >> 2;
    ptr_msg->pgn = get_pgn(ptr_pdu);

    for(i=0;i<ptr_msg->data_length;i++)
    {
        ptr_msg->data[i] = ptr_pdu->data[i];
    }

    return;
}

extern BOOL need_follow_pdu(const PDU *ptr_pdu)
{
    INT32 pgn = 0;

    pgn = get_pgn(ptr_pdu);
    assert(pgn >= 0);

    if(TP_PGN == pgn)
    {
#if (DEBUG_DETAIL & PP_DEBUG)
        INT32 rst = 0;
        printf("%s,%d:get a TP PDU, the pdu content is:\n", __FILE__, __LINE__);
        rst = print_pdu(ptr_pdu);
        assert(0 == rst);
#endif
        return true;
    }
    else
    {
        return false;
    }
}

static void build_single_pdu(PDU *ptr_pdu, const MSG *ptr_msg)
{
    INT32 i = 0;
    INT32 length = 0;
#if (DEBUG_DETAIL & PP_DEBUG)
    INT32 rst = 0;
#endif

    ptr_pdu->p_r_dp = ptr_msg->p << 2 | 0/* R位恒为零,协议2.4 表2 */ << 1 | ptr_msg->dp;
    ptr_pdu->pf = ptr_msg->pf;
    ptr_pdu->ps = ptr_msg->ps;
    ptr_pdu->sa = PLATFORM_ADDR;

    length = ptr_msg->data_length;
    for(i=0;i<length;i++)
    {
        ptr_pdu->data[i] = ptr_msg->data[i];
    }

#if (DEBUG_DETAIL & PP_DEBUG)
    printf("%s,%d:build a pdu from j1939 msg,the j1939 msg content is:\n", __FILE__, __LINE__);
    rst = print_msg(ptr_msg); 
    assert(0 == rst);
    
    printf("%s,%d:the pdu content is:\n", __FILE__, __LINE__);
    rst = print_pdu(ptr_pdu);
    assert(0 == rst);
#endif

    return;
}

static INT32 get_pgn(const PDU *ptr_pdu)
{
    INT32 pgn = 0;

    pgn = ((0x03 & ptr_pdu->p_r_dp) << 16)
        | (ptr_pdu->pf << 8)
        | (ptr_pdu->ps);

#if (DEBUG_DETAIL & PP_DEBUG)
    printf("%s,%d:make a pgn from r dp pf ps,the p_r_dp is:\n", __FILE__, __LINE__);
    printf("%s,%d:p(4,3,2bit not pgn part)_r(1bit)_dp(0bit): 0x%02x\n", __FILE__, __LINE__, ptr_pdu->p_r_dp);
    printf("%s,%d:pf:0x%02x\n", __FILE__, __LINE__, ptr_pdu->pf);
    printf("%s,%d:ps:0x%02x\n", __FILE__, __LINE__, ptr_pdu->ps);
    printf("%s,%d:the pgn is:%d\n", __FILE__, __LINE__,pgn);
#endif

    return pgn;
}

extern void print_pdu(const PDU *ptr_pdu)
{
    INT32 i = 0;

    printf("p_r_dp:0x%02x\n", ptr_pdu->p_r_dp);
    printf("pf:%d\n", ptr_pdu->pf);
    printf("ps:%d\n", ptr_pdu->ps);
    printf("sa:%d\n", ptr_pdu->sa);

    for(i=0;i<8;i++)                                                    /* pdu 中无长度域 8字节全部打出 */
    {
        printf("data[%d]=0x%02x\n", i, ptr_pdu->data[i]);
    }

    return;
}

