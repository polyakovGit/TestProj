# STM32 UDP client example
## Задание

1. Доработать udp_handler так, чтобы это был законченный функционал для работы с любым сокетом.
2. Сигнатуры функций должны выглядеть следующим образом:
```
struct udp_pcb* udp_create_socket(const ip4_addr_t ip_addr, const u16_t port, udp_recv_fn recv, void *recv_arg); 
err_t udp_send_msg(struct udp_pcb* upcb, const char* data);
```
   *udp_recv_fn* - указатель на колбэк-функцию приема пакета.

3. При приёме ответа от сервера должен менять состояние светодиод LD2.
4. Дополнительно: написать Desktop UDP-сервер, используя библиотеку Boost.Asio.
