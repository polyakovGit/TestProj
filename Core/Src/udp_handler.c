#include "udp_handler.h"

#include "udp.h"

#include <string.h>

static struct udp_pcb* upcb = NULL;

static void udp_receive_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
	    const ip_addr_t *addr, u16_t port)
{
	// в этой функции обязательно должны очистить p, иначе память потечёт
	pbuf_free(p);
}

err_t udp_create_socket()
{
	// проверяем, что не инициализировали сокет еще
	if (upcb == NULL)
	{
		// создание сокета
		upcb = udp_new();

		// если не удалось создать сокет, то на выход с ошибкой
		if (upcb == NULL)
		{
			return ERR_ABRT;
		}
	}

	ip4_addr_t dest;
	IP4_ADDR(&dest, 192, 168, 0, 11);
	// коннектимся к удаленному серверу по ИП и порту (сервер должен быть настроен именно на так)
	err_t err = udp_connect(upcb, &dest, 3333);
	if (ERR_OK != err)
	{
		return err;
	}

	// регистрируем колбэк на прием пакета
	udp_recv(upcb, udp_receive_callback, NULL);
	return ERR_OK;
}

err_t udp_send_msg()
{
	// если сокет не создался, то на выход с ошибкой
	if (upcb == NULL)
	{
		return ERR_ABRT;
	}
	// аллоцируем память под буфер с данными
	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, 5, PBUF_RAM);
	if (p != NULL)
	{
		char data[5] = "Test";
		// кладём данные в аллоцированный буфер
		err_t err = pbuf_take(p, data, 5);
		if (ERR_OK != err)
		{
			// обязательно должны очистить аллоцированную память при ошибке
			pbuf_free(p);
			return err;
		}

		// отсылаем пакет
		err = udp_send(upcb, p);
		if (ERR_OK != err)
		{
			// обязательно должны очистить аллоцированную память при ошибке
			pbuf_free(p);
			return err;
		}
		// очищаем аллоцированную память
		pbuf_free(p);
	}
	return ERR_OK;
}
