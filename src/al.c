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

/*
 * 实现协议         PGN          byte       精度
3、转速             62980        5-6        0.125(rpm) / bit
4、水温             62980        3-4        1(℃) / bit

5、剩余油量         62980        0          (百分数) / bit
6、即时燃油经济性   62978        3-4        1/512 (km/kg) /bit
7、燃油消耗速度     62978        5-6        0.05(L/h) / bit
8、累计油耗         62981        0-3        0.5(L) / bit

9、机油压力         62979        0-1        4(kpa) / bit
*/

static INT32 parse_msg(RECVED_MSG *ptr_recved_msg, const MSG *ptr_msg)
{
    switch(ptr_msg->pgn)
    {
        case REQUEST_PGN:
            {
                break;
            }
        case DECLARE_ADDR_PGN:
            {
                break;
            }
        case EEC1_PGN:
            {
                break;
            }
        case TP_PGN:
            {
                break;
            }
        case DISTANCE_PGN:
            {

                /* 1、里程             62977        0-3        0.1(km)/bit */
                F32 distance = 0;
                distance = ptr_msg->data[3] << 24;
                distance += ptr_msg->data[2] << 16;
                distance += ptr_msg->data[1] << 8;
                distance += ptr_msg->data[0];
                ptr_recved_msg->total_distance = distance;
                break;
            }
        case SPEED_PGN:
            {
                /* 2、车速             62980        1-2        1/256(km/h) / bit */
                F32 speed = 0;
                speed = ptr_msg->data[1] << 8;
                speed += ptr_msg->data[0];
                speed /= 256.0;
                ptr_recved_msg->speed = speed;

                /* 3、转速             62980        5-6        0.125(rpm) / bit */
                F32 roate_speed = 0;
                roate_speed = ptr_msg->data[6] << 8;
                roate_speed += ptr_msg->data[5];
                roate_speed /= 8.0;
                ptr_recved_msg->roate_speed = roate_speed;

                /* 4、水温             62980        3-4        1(℃) / bit */
                INT32 water_temperature = 0;
                water_temperature = ptr_msg->data[4] << 8;
                water_temperature += ptr_msg->data[3];
                ptr_recved_msg->water_temperature = water_temperature;

                /* 5、剩余油量         62980        0          (百分数) / bit */
                UINT8 oil_left = 0;
                oil_left = ptr_msg->data[0];
                ptr_recved_msg->oil_left = oil_left;

                break;
            }
        case OIL_PGN:
            {
                /* 6、即时燃油经济性   62978        3-4        1/512 (km/kg) /bit */
                F32 oil_valid;
                oil_valid = ptr_msg->data[4] << 8;
                oil_valid += ptr_msg->data[3];
                oil_valid /= 512;
                ptr_recved_msg->oil_valid = oil_valid;
               
                /* 7、燃油消耗速度     62978        5-6        0.05(L/h) / bit */
                F32 oil_used;
                oil_used = ptr_msg->data[6] << 8;
                oil_used += ptr_msg->data[5];
                oil_used /= 20.0;
                ptr_recved_msg->oil_used = oil_used;

                break;
            }
        case OIL_USED_PGN:
            {
                /* 8、累计油耗         62981        0-3        0.5(L) / bit */
                F32 total_oil_use;
                total_oil_use = ptr_msg->data[3] << 24;
                total_oil_use += ptr_msg->data[2] << 16;
                total_oil_use += ptr_msg->data[1] << 8;
                total_oil_use += ptr_msg->data[0];
                total_oil_use /= 2.0;
                ptr_recved_msg->total_oil_use = total_oil_use;

                /* 9、机油压力         62979        0-1        4(kpa) / bit */
                INT32 oil_press;
                oil_press = ptr_msg->data[1] << 8;
                oil_press += ptr_msg->data[0];
                oil_press *= 4;
                ptr_recved_msg->oil_press = oil_press;

                break;
            }
        case OIL_PRESS_PGN:
            {
                break;
            }
        default:
            {
                /* TODO: 当有符合应用层标准的数据时，加入错误检查 */
                // printf("%s,%d:not Implement this msg pgn:%d\n", __FILE__, __LINE__, ptr_msg->pgn); 
                return 0;
            }
    }

    return 0;
}

extern void print_recved_msg(const RECVED_MSG *ptr_recved_msg)
{
    printf("1:里程:%.2f\n", ptr_recved_msg->total_distance);
    printf("2:车速:%.2f\n", ptr_recved_msg->speed);
    printf("3:转速:%.2f\n", ptr_recved_msg->roate_speed);
    printf("4:水温:%d℃ \n", ptr_recved_msg->water_temperature);
    printf("5:剩余油量:%d%%\n", ptr_recved_msg->oil_left);
    printf("6:瞬时油效:%.2f\n", ptr_recved_msg->oil_valid);
    printf("7:油耗速度:%.2f\n", ptr_recved_msg->oil_used);
    printf("8:累计油耗:%.2f\n", ptr_recved_msg->total_oil_use);
    printf("8:机油压力:%d\n\n", ptr_recved_msg->oil_press);
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

