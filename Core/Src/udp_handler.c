#include "udp_handler.h"

#include <string.h>

static struct udp_pcb *upcb = NULL;
char str1[128];

void udp_receive_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
		const ip_addr_t *addr, u16_t port) {
	// в этой функции обязательно должны очистить p, иначе память потечёт
	strncpy(str1, p->payload, p->len);
	str1[p->len] = 0;
	pbuf_free(p);
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
}

//void my_udp_connect(){
//	ip4_addr_t dest;
//	IP4_ADDR(&dest, 10, 102, 2, 186);
//	udp_create_socket(dest, 3333,udp_receive_callback,NULL);
//}

struct udp_pcb* udp_create_socket(const ip4_addr_t ip_addr, const u16_t port,
		udp_recv_fn recv, void *recv_arg) {
	// проверяем, что не инициализировали сокет еще
	if (upcb == NULL) {
		// создание сокета
		upcb = udp_new();
		// если не удалось создать сокет, то на выход с ошибкой
		if (upcb == NULL) {
			return upcb;
		}
	}

	//ip4_addr_t dest;
	//IP4_ADDR(&dest, 10, 102, 2, 186);//192, 168, 0, 11, сервер для теста 10, 102, 2, 186
	// коннектимся к удаленному серверу по ИП и порту (сервер должен быть настроен именно на так)
//	err_t err = udp_connect(upcb, &dest, 3333);
	err_t err = udp_connect(upcb, &ip_addr, port);
	if (ERR_OK != err) {
		udp_remove(upcb);
		return NULL;
	}
	// регистрируем колбэк на прием пакета
	udp_recv(upcb, recv, NULL);
	return upcb;
}

err_t udp_send_msg(struct udp_pcb *upcb, const char *dataSource) {
	// если сокет не создался, то на выход с ошибкой
	if (upcb == NULL) {
		return ERR_ABRT;
	}
	u16_t dataLength = strlen(dataSource) + 1;
	// аллоцируем память под буфер с данными
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, dataLength, PBUF_RAM);
	if (p != NULL) {
		//char data[5] = "Test";
		//аллокация под сообщение для передачи
		//char* data=strdup(dataSource);
//		char *data =  malloc(dataLength);
//		if (data == NULL) {
//			return ERR_ABRT;
//		}
//		strncpy(data, dataSource, dataLength);
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
		err = udp_send(upcb, p);
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
