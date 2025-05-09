#include "scooter-system/parser.cpp"
#include "scooter-system/systems.cpp"
#include <zmq.hpp>

int main() {
  ScooterPark park;
  park.add_station("Medvedkovo");
  park.add_station("Sviblovo");
  park.add_station("Otradnoe");

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      park.add_scooter(i);
    }
  }

  park.show_park();
  park.user_register("Vanya");
  park.user_register("Katya");
  park.show_db();

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REP);
  socket.bind("tcp://*:5555");

  std::cout << "Socket binded" << '\n';

  while (true) {
    zmq::message_t request;
    socket.recv(request);
    std::string msg(static_cast<char *>(request.data()), request.size());

    std::cout << "Recieved: " << msg << '\n';

    std::vector<std::string> comand_args = parse_comands(msg);
    std::string reply_str = "";

    if (comand_args[0] == "lin") {
      // login user with given ID {lin, <user_id>}
      uint32_t uid = std::stoi(comand_args[1]);
      if (park.user_log_in(uid)) {
        reply_str = "ok";
      } else {
        reply_str = "er";
      }
    } else if (comand_args[0] == "tsc") {
      // take scooter with given ID {"tsc", <user_id>, <scooter_id>}
      park.take_scooter(std::stoi(comand_args[1]), std::stoi(comand_args[2]));
      reply_str = "ok";
    } else if (comand_args[0] == "rsc") {
      // return scooter with given ID to station with given ID
      // {"rsc", <user_id>, <scooter_id>, <station_id>}
      park.return_scooter(std::stoi(comand_args[1]), std::stoi(comand_args[2]),
                          std::stoi(comand_args[3]));
      reply_str = "ok";
    } else if (comand_args[0] == "shw") {
      // show park info
      park.show_park();
      reply_str = "ok";
    } else if (comand_args[0] == "stsc") {
      // show availible scooters in the station
      // {"stsc", <station_id>}
      std::vector<uint32_t> scids = park.station_scooters(std::stoi(comand_args[1]));
      for (auto scid: scids) {
        std::string scid_cstr = std::to_string(scid);
        reply_str.insert(reply_str.size(), scid_cstr);
        if (scid != scids[scids.size() - 1]) {
          reply_str.push_back(':');
        }
      }
    }

    std::cout << "Sended back: " << reply_str << '\n';
    zmq::message_t reply(reply_str.c_str(), reply_str.size());
    socket.send(reply, zmq::send_flags::none);
    std::cout << "+------------------LOOP END------------------+\n";
  }

  return 0;
}