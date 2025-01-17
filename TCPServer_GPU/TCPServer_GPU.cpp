#include <asio.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <thread>

class TcpServer {
public:
    TcpServer(asio::io_context& io_context, short port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
        start_accept();
    }

    void start_accept() {
        auto socket = std::make_shared<asio::ip::tcp::socket>(acceptor_.get_executor());  // Use io_context directly
        acceptor_.async_accept(*socket, [this, socket](const asio::error_code& error) {
            if (!error) {
                std::cout << "Client connected\n";
                start_read(socket);
            }
            else {
                std::cout << "Error on accept: " << error.message() << std::endl;
            }
            start_accept();  // Continue accepting new connections
            });
    }

    void start_read(std::shared_ptr<asio::ip::tcp::socket> socket) {
        asio::async_read_until(*socket, asio::dynamic_buffer(data_), '\n',
            [this, socket](const asio::error_code& error, std::size_t length) {
                if (!error) {
                    std::string command = data_.substr(0, length - 1);//移除末尾的换行符
                    data_.erase(0, length);
                    std::cout << "Received command: " << command << std::endl;
                    // Process the command here
                    handle_command(socket, command);
                    start_read(socket);  // Continue reading from the client
                }
                else if (error == asio::error::eof) {
                    std::cout << "Connection closed by client (EOF)" << std::endl;
                    socket->close();  // 关闭socket
                    //start_accept();   // 继续接受新的连接
                }
                else {
                    std::cout << "Error on read: " << error.message() << std::endl;
                    socket->close();
                }
            });
    }

    void handle_command(std::shared_ptr<asio::ip::tcp::socket> socket, const std::string& command) {
        if (command == "GetParam") {
            // Respond to GetParam command
            asio::write(*socket, asio::buffer("GetParam response\n"));
        }
        else if (command == "GetPhoto") {
            // Respond to GetPhoto command
            asio::write(*socket, asio::buffer("GetPhoto response\n"));
        }
        else if (command == "Infer") {
            // Respond to Infer command
            asio::write(*socket, asio::buffer("Infer response\n"));
        }
        else {
            asio::write(*socket, asio::buffer("Unknown command\n"));
        }
    }

private:
    asio::ip::tcp::acceptor acceptor_;
    std::string data_;
};

int main() {
    try {
        asio::io_context io_context;
        TcpServer server(io_context, 12345);
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
