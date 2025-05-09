#include <iostream>
#include <zmq.hpp>

class ClientUser {
  bool is_logged;
  uint32_t current_user_id;

public:
  ClientUser() : is_logged(false), current_user_id(0) {}

  void log_in(uint32_t uid, zmq::socket_t &socket) {
    std::string uid_str = std::to_string(uid);
    char *msg = new char[4 + uid_str.size() + 1];
    msg[0] = '\0';
    std::strcat(msg, "lin:");
    std::strcat(msg, uid_str.c_str());

    std::cout << "Sended request: " << msg << '\n';

    zmq::message_t request(msg, strlen(msg));
    socket.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply);
    std::string reply_str(static_cast<char *>(reply.data()), reply.size());

    std::cout << "Received: " << reply_str << '\n';

    if (reply_str == "ok") {
      is_logged = true;
      current_user_id = uid;
      std::cout << "User logged in" << '\n';
    }

    delete[] msg;
  }

  // scid = scooter_id
  void take_scooter(uint32_t scid, zmq::socket_t &socket) {
    if (!is_logged) {
      throw std::runtime_error("You are bot logged in!");
    }

    std::string uid_str = std::to_string(current_user_id);
    std::string scid_str = std::to_string(scid);
    char *msg = new char[4 + scid_str.size() + uid_str.size() +
                         2]; // "tsc:<user_id>:<scooter_id>\0"
    msg[0] = '\0';
    std::strcat(msg, "tsc:");
    std::strcat(msg, uid_str.c_str());
    std::strcat(msg, ":");
    std::strcat(msg, scid_str.c_str());

    zmq::message_t request(msg, strlen(msg));
    socket.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply);
    std::string reply_str(static_cast<char *>(reply.data()), reply.size());

    std::cout << "Received: " << reply_str << '\n';
  }

  void return_scooter(uint32_t scid, uint32_t stid, zmq::socket_t &socket) {
    if (!is_logged) {
      throw std::runtime_error("You are not logged in!");
    }

    std::string uid_str = std::to_string(current_user_id);
    std::string scid_str = std::to_string(scid);
    std::string stid_str = std::to_string(stid);
    char *msg =
        new char[4 + uid_str.size() + scid_str.size() + stid_str.size() +
                 3]; // "rsc:<user_id>:<scooter_id>:<station_id>\0"
    msg[0] = '\0';
    std::strcat(msg, "rsc:");
    std::strcat(msg, uid_str.c_str());
    std::strcat(msg, ":");
    std::strcat(msg, scid_str.c_str());
    std::strcat(msg, ":");
    std::strcat(msg, stid_str.c_str());

    zmq::message_t request(msg, strlen(msg));
    socket.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply);
    std::string reply_str(static_cast<char *>(reply.data()), reply.size());

    std::cout << "Received: " << reply_str << '\n';
  }

  void show_park(zmq::socket_t &socket) {
    char *msg =
        new char[3 + 1];
    msg[0] = '\0';
    std::strcat(msg, "shw");

    zmq::message_t request(msg, strlen(msg));
    socket.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply);
    std::string reply_str(static_cast<char *>(reply.data()), reply.size());

    std::cout << "Received: " << reply_str << '\n';
  }

  void send_comand(const std::string& s, zmq::socket_t &socket) {
    zmq::message_t request(s.c_str(), s.size());
    socket.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    socket.recv(reply);
    std::string reply_str(static_cast<char *>(reply.data()), reply.size());

    std::cout << "Received: " << reply_str << '\n';
  }

  ~ClientUser() {}
};

int main() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);
  socket.connect("tcp://localhost:5555");

  ClientUser cu;
  std::string comand = "";
  while (std::cin >> comand) {
    cu.send_comand(comand, socket);
  }

  return 0;
}