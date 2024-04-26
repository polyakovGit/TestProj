#ifndef INC_UDP_HANDLER_H_
#define INC_UDP_HANDLER_H_

#include <udp.h>

#define LEDS_COUNT 3
#define BUFFER_ARRAY_SIZE 32

extern struct netif gnetif;

struct GPIO_Attr {
	GPIO_TypeDef *gpio;
	uint16_t pin;
};

err_t udp_send_message(struct udp_pcb *upcb,const ip_addr_t *addr, u16_t port, const char *dataSource);
void get_ifconfig(char *answer);
void sed_leds(char *answer, char *received_message);
void udp_receive_message(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udp_create_socket(void);
#endif
