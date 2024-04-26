//telnet 192.168.0.10:3333
//./start_leds led1 led2 led3
//./stop_leds led1 led2 led3

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#define LEDS_COUNT 3

using boost::asio::ip::udp;

void message_from_server(std::string host_ip, std::string message) {
	std::cout << host_ip << "# " << message << std::endl;
}

void clear_console_input() {
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//typedef struct
//{
//	char* response;
//	char* command;
//	char* data;
//} request_struct_sm;
//
//size_t serialize(const request_struct_sm* arch_sm, char* buf)
//{
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
//void deserialize(const char* buf, request_struct_sm* arch_sm)
//{
//	size_t offset = 0;
//	arch_sm->response = _strdup(buf + offset);
//	offset += strlen(buf + offset) + 1;
//	arch_sm->command = _strdup(buf + offset);
//	offset += strlen(buf + offset) + 1;
//	arch_sm->data = _strdup(buf + offset);
//}

std::string parse_command() {
	std::string command;
	std::cin >> command;

	if (command == "exit") {
		return "exit";
	}
	if (command == "ifconfig") {
		return "ifconfig";
	}
	if (command == "echo") {
		return "echo";
	}
	try {
		if (command == "./start_leds" || command == "./stop_leds") {
			bool led_state = (command == "./start_leds" ? 1 : 0);
			size_t pos_num_led = 3;
			std::string leds(LEDS_COUNT, '2');
			std::string led_Controls, tokens, token;
			std::getline(std::cin, tokens, '\n');
			std::stringstream command_stream(tokens);
			while (command_stream >> token) {
				leds[token[pos_num_led] - '1'] = '0' + led_state;
			}
			return leds.append("leds");
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	clear_console_input();
	return "Unknown command";
}

int main() {
	try {
		std::string command, ip, port, input_line;
		int command_error_count = 0;
		while (1) {
			do {
				std::cout << "to connect to the server enter telnet <IP_add>:<port>" << std::endl;
				std::getline(std::cin, command, ' ');
				if (command != "telnet") {
					std::cout << "unknown command" << std::endl;
					clear_console_input();
					++command_error_count;
					continue;
				}
				std::getline(std::cin, ip, ':');
				std::cin >> port;
			} while (command != "telnet" && command_error_count != 3);
			if (command_error_count == 3)
				return 0;
			command_error_count = 0;

			std::cout << "establishing a connection with " << ip << std::endl;//change to TCP
			boost::asio::io_service io_service;
			udp::resolver resolver(io_service);
			udp::resolver::query query(udp::v4(), ip, port);
			udp::endpoint receiver_endpoint = *resolver.resolve(query);
			udp::socket socket(io_service);
			socket.open(udp::v4());
			//static std::string command;
			//boost::array<char, 64> send_buf = {"Test\n"};
			boost::array<char, 1024> recv_buf;
			//boost::array<char, 1024> send_buf;
			for (; command_error_count != 3;) {
				//request_struct_sm server_message;
				//request_struct_sm client_message;
				//server_message.response = _strdup("RESPONSE");
				//server_message.command = _strdup("COMMAND");
				//server_message.data = _strdup("DATA");
				std::cout << ip << "# ";
				command = parse_command();
				//clear_console_input();
				if (command == "exit") {
					message_from_server(ip, "end telnet session");
					break;
				}
				socket.send_to(boost::asio::buffer(command), receiver_endpoint);
				udp::endpoint sender_endpoint;
				size_t len = socket.receive_from(
					boost::asio::buffer(recv_buf), sender_endpoint);
				std::string data(recv_buf.data(), len);
				message_from_server(ip, data);
				std::string test = "Unknown command";
				if (data == "Unknown command")
					++command_error_count;
			}
			if (command_error_count == 3)
				return 0;
			clear_console_input();
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

