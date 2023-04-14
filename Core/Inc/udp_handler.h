#ifndef INC_UDP_HANDLER_H_
#define INC_UDP_HANDLER_H_

#include "ip4_addr.h"
#include "err.h"

err_t udp_create_socket(); ///< функция создания сокета
err_t udp_send_msg(); ///< функция отправки сообщения

#endif
