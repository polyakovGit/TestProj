#include "udp_handler.h"
#include "string.h"

err_t udp_send_message(struct udp_pcb *upcb, const ip_addr_t *addr, u16_t port, const char *dataSource) {
	// если сокет не создался, то на выход с ошибкой
	if (upcb == NULL) {
		return ERR_ABRT;
	}
	u16_t dataLength = strlen(dataSource);
	// аллоцируем память под буфер с данными
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, dataLength, PBUF_RAM);
	if (p != NULL) {
		// кладём данные в аллоцированный буфер
		err_t err = pbuf_take(p, dataSource, dataLength);
		//очистить память от сообщения
		//free(data);
		if (ERR_OK != err) {
			// обязательно должны очистить аллоцированную память при ошибке
			pbuf_free(p);
			return err;
		}
		// отсылаем пакет
		err = udp_sendto(upcb, p, addr, port);
		if (ERR_OK != err) {
			// обязательно должны очистить аллоцированную память при ошибке
			pbuf_free(p);
			return err;
		}
		// очищаем аллоцированную память
		pbuf_free(p);
	}
	return ERR_OK;
}

void get_ifconfig(char *answer) {
	char local_ip[BUFFER_ARRAY_SIZE];
	char subnet_mask[BUFFER_ARRAY_SIZE];
	char gateway[BUFFER_ARRAY_SIZE];

	ip4addr_ntoa_r(netif_ip4_addr(&gnetif), local_ip, BUFFER_ARRAY_SIZE);
	ip4addr_ntoa_r(netif_ip4_netmask(&gnetif), subnet_mask, BUFFER_ARRAY_SIZE);
	ip4addr_ntoa_r(netif_ip4_gw(&gnetif), gateway, BUFFER_ARRAY_SIZE);
	sprintf(answer, "\n%s %s\n%s %s\n%s %s", "ip address: ", local_ip, "subnet mask:", subnet_mask, "gateway:    ",
			gateway);
}

void sed_leds(char *answer, char *received_message) {
	char led_answer[BUFFER_ARRAY_SIZE] = "leds { ";
	char led[LEDS_COUNT];
	struct GPIO_Attr gpio_outputs[] = { { GPIOB, GPIO_PIN_0 }, { GPIOB,
	GPIO_PIN_7 }, { GPIOB, GPIO_PIN_14 } };
	for (uint16_t i = 0; i < sizeof(gpio_outputs) / sizeof(gpio_outputs[0]); i++) {
		uint16_t led_command = received_message[i] - '0';
		if (led_command != 2 && led_command != HAL_GPIO_ReadPin(gpio_outputs[i].gpio, gpio_outputs[i].pin)) {
			HAL_GPIO_WritePin(gpio_outputs[i].gpio, gpio_outputs[i].pin, led_command);
			sprintf(led, "%d ", i + 1);
			strcat(led_answer, led);
		}
	}
	strcat(led_answer, "} switched");
	strcpy(answer, led_answer);
}

void udp_receive_message(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
	if (p != NULL) {
		/* send received packet back to sender */
		char received_message[BUFFER_ARRAY_SIZE];
		strncpy(received_message, p->payload, p->len);
		received_message[p->len] = '\0';
		pbuf_free(p);

		char answer[1024];
		if (strcmp(received_message, "echo") == 0) {
			strcpy(answer, received_message);
		} else if (strcmp(received_message, "ifconfig") == 0) {
			get_ifconfig(answer);
		} else if (strncmp(received_message + LEDS_COUNT, "leds", 4) == 0) {
			sed_leds(answer, received_message);
		} else {
			strcpy(answer, "Unknown command");
		}
		udp_send_message(pcb, addr, port, answer);
	}
}

void udp_create_socket(void) {
	struct udp_pcb *pcb;

	/* get new pcb */
	pcb = udp_new();
	if (pcb == NULL) {
		LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));
		return;
	}

	/* bind to any IP address on port 3333 */
	if (udp_bind(pcb, IP_ADDR_ANY, 3333) != ERR_OK) {
		LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
		return;
	}

	/* set udp_echo_recv() as callback function
	 for received packets */
	udp_recv(pcb, udp_receive_message, NULL);
}

//typedef struct {
//	char *response;
//	char *command;
//	char *data;
//} request_struct_sm;
//size_t serialize(const request_struct_sm *arch_sm, char *buf) {
//	size_t bytes = 0;
//	memcpy(buf + bytes, arch_sm->response, strlen(arch_sm->response) + 1);
//	bytes += strlen(arch_sm->response) + 1;
//	memcpy(buf + bytes, arch_sm->command, strlen(arch_sm->command) + 1);
//	bytes += strlen(arch_sm->command) + 1;
//	memcpy(buf + bytes, arch_sm->data, strlen(arch_sm->data) + 1);
//	bytes += strlen(arch_sm->data) + 1;
//	return bytes;
//}
//
//void deserialize(const char *buf, request_struct_sm *arch_sm) {
//	size_t offset = 0;
//	arch_sm->response = strdup(buf + offset);
//	offset += strlen(buf + offset) + 1;
//	arch_sm->command = strdup(buf + offset);
//	offset += strlen(buf + offset) + 1;
//	arch_sm->data = strdup(buf + offset);
//}

