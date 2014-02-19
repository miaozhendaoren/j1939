#include "al.h"

static INT32 build_msg(MSG *ptr_msg, SENDDED_MSG_TYPE type);
static INT32 parse_msg(RECVED_MSG *ptr_recved_msg, const MSG *ptr_msg);

extern void al_init(void)
{
    return;
}

extern void push_msg_to_stack(SENDDED_MSG_TYPE type)
{ 
    MSG msg = {0};
    PDU *ptr_pdu = NULL;
    INT32 length = 0;
    INT32 rst = 0;

    rst = build_msg(&msg, type);
    assert(0 == rst);

    ptr_pdu = (PDU *)malloc(sizeof(PDU)*MAX_MUL_CAN_NUMS);
    assert(NULL != ptr_pdu);

    build_pdu(ptr_pdu, &length, &msg);

    build_and_enqueue_cans(ptr_pdu, length);

    free(ptr_pdu);
    ptr_pdu = NULL;

    return;
}

extern void pull_msg_from_stack(RECVED_MSG *ptr_recved_msg)
{

    INT32 rst = 0;
    PDU *ptr_pdu = NULL;
    MSG msg = {0};
    INT32 length = 0;

    ptr_pdu = (PDU *)malloc(sizeof(PDU)*MAX_MUL_CAN_NUMS);
    assert(NULL != ptr_pdu);

    dequeue_and_parse_cans(ptr_pdu, &length);

    parse_pdu(&msg, ptr_pdu, length);

    rst = parse_msg(ptr_recved_msg, &msg);
    assert(0 == rst);

    free(ptr_pdu);
    ptr_pdu = NULL;

    return;
}

extern INT32 get_need_can_nums(INT32 data_length)
{
    int nums = 0;

    if(data_length <= 8)
    {
        return 1;
    }
    else
    {
        nums = data_length / 7;
        nums += 1;
#if (DEBUG_DETAIL & PP_DEBUG)
        printf("%s,%d:need %d cans, from data length %d.\n", __FILE__, __LINE__, nums, data_length);
#endif
        return nums;
    }

    return 0;
}

static INT32 build_msg(MSG *ptr_msg, SENDDED_MSG_TYPE type)
{
#if (DEBUG_DETAIL & PP_DEBUG)
    INT32 rst = 0;
#endif

    switch(type)
    {
        case tx_req_addr:
            {
                ptr_msg->data_length = 3;
                ptr_msg->dp = 0;
                ptr_msg->pf = 234;
                ptr_msg->ps = PLATFORM_ADDR;
                ptr_msg->p = 6;
                ptr_msg->pgn = REQUEST_PGN;
                ptr_msg->data[0] = (UINT8)0x00;
                ptr_msg->data[1] = (UINT8)0xEE;
                ptr_msg->data[2] = (UINT8)0x00;

                break;
            }
        case tx_declare_addr:
            {
                ptr_msg->data_length = 8;
                ptr_msg->dp = 0;
                ptr_msg->pf = 238;
                ptr_msg->ps = 255;
                ptr_msg->p = 6;
                ptr_msg->pgn = DECLARE_ADDR_PGN;
                /* 不同的ECU需要做相应的修改 */
                ptr_msg->data[0] = (UINT8)2;
                ptr_msg->data[1] = (UINT8)3;
                ptr_msg->data[2] = (UINT8)2;
                ptr_msg->data[3] = (UINT8)3;
                ptr_msg->data[4] = (UINT8)2;
                ptr_msg->data[5] = (UINT8)2;
                ptr_msg->data[6] = (UINT8)2;
                ptr_msg->data[7] = (UINT8)2;

                break;
            }
        case tx_req_engine_speed:
            {
                ptr_msg->data_length = 8;
                ptr_msg->dp = 0;
                ptr_msg->pf = 234;
                ptr_msg->ps = 4;                                      /* 向引擎请求 EEC1 */
                ptr_msg->p = 6;
                ptr_msg->pgn = REQUEST_PGN;
                ptr_msg->data[0] = (UINT8)0x04;
                ptr_msg->data[1] = (UINT8)0xF0;
                ptr_msg->data[2] = (UINT8)0x00;

                break;
            }
            /* 引擎EEC1发送该消息 */
        case tx_engine_speed:
            {
                ptr_msg->data_length = 8;
                ptr_msg->dp = 0;
                ptr_msg->pf = 240;
                ptr_msg->ps = 4;
                ptr_msg->p = 3;
                ptr_msg->pgn = DECLARE_ADDR_PGN;
                ptr_msg->pgn = REQUEST_PGN;
                ptr_msg->pgn = EEC1_PGN;
                /* 参见J1939协议2.5 */
                ptr_msg->data[3] = (UINT8)0x11;
                ptr_msg->data[4] = (UINT8)0x22;

                break;
            }
        default:
            {
                printf("%s,%d:not Implement this msg type:%d\n", __FILE__, __LINE__, type);
                return EINVAL;
            }
    }

#if (DEBUG_DETAIL & PP_DEBUG)
    printf("%s,%d:build j1939 msg from msg type(%d):%s,the msg content is:\n",
            __FILE__, __LINE__, type, get_sendded_msg(type));
    rst = print_msg(ptr_msg);
    assert(0 == rst);
#endif

    return 0;
}


static INT32 parse_msg(RECVED_MSG *ptr_recved_msg, const MSG *ptr_msg)
{
    switch(ptr_msg->pgn)
    {
        case EEC1_PGN:
            {
                /* 定义见SPN190 */
                ptr_recved_msg->engine_speed = 32 * ptr_msg->data[4]
                    + 0.125 * ptr_msg->data[3];

#if (DEBUG_DETAIL & PP_DEBUG)
                printf("%s,%d:parse a EEC1_PGN, get engine speed is:%.3f\n",
                        __FILE__, __LINE__, ptr_recved_msg->engine_speed);
#endif

                break;
            }
        case REQUEST_PGN:
        case DECLARE_ADDR_PGN:
        case TP_PGN:
        default:
            {
                printf("%s,%d:not Implement this msg pgn:%d\n", __FILE__, __LINE__, ptr_msg->pgn);
                return 0;
            }
    }

    return 0;
}

extern void print_recved_msg(const RECVED_MSG *ptr_recved_msg)
{
    printf("engine speed:          %.3f\n", ptr_recved_msg->engine_speed);
    printf("fule press:            %d\n", ptr_recved_msg->fule_press);
    printf("total distance:        %d\n", ptr_recved_msg->total_distance);
    printf("total fule use:        %d\n", ptr_recved_msg->total_fule_use);
    printf("water temperature:     %d\n", ptr_recved_msg->water_temperature);
    printf("oil press:             %d\n", ptr_recved_msg->oil_press);
    printf("speed:                 %d\n", ptr_recved_msg->speed);
    printf("instantaneous fule use:%d\n", ptr_recved_msg->instan_fule_use);
    printf("air press:             %d\n", ptr_recved_msg->air_press);
    printf("dtc:\n");
    printf("dtc.spn:               %d\n", ptr_recved_msg->dtc.spn);
    printf("dtc.fmi:               %d\n", ptr_recved_msg->dtc.fmi);
    printf("dtc.oc:                %d\n", ptr_recved_msg->dtc.oc);

    return;
}

extern INT8 *get_sendded_msg(const SENDDED_MSG_TYPE sended_msg)
{
    switch(sended_msg)
    {
        case (tx_req_addr):
            {
                return "tx_req_addr.";
            }
        case (tx_declare_addr):
            {
                return "tx_declare_addr.";
            }
        case (tx_unable_declare_addr):
            {
                return "tx_unable_declare_addr.";
            }
        case (tx_req_engine_speed):
            {
                return "tx_req_engine_speed";
            }
        case (tx_engine_speed):
            {
                return "tx_engine_speed";
            }
        default:
            {
                return NULL;
            }
    }
}

extern void print_msg(const MSG *ptr_msg)
{
    INT32 i = 0;
    INT32 length = ptr_msg->data_length;

    printf("data_length:%d\n", length);
    printf("dp:%d\n", ptr_msg->dp);
    printf("pf:%d\n", ptr_msg->pf);
    printf("ps:%d\n", ptr_msg->ps);
    printf("p:%d\n", ptr_msg->p);
    printf("pgn:%d\n", ptr_msg->pgn);

    for(i=0;i<length;i++)
    {
        printf("data[%d]=0x%02x\n", i, ptr_msg->data[i]);
    }

    return;
}

