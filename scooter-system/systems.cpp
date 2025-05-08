#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class Scooter {
  friend class ScooterPark;
  friend class Station;
  friend class User;

  u_int32_t id;
  u_int16_t battery;

protected:
  bool is_free;
  u_int32_t station_id;

  Scooter(u_int32_t id, u_int32_t station_id)
      : id(id), battery(100), is_free(true), station_id(station_id) {}

  u_int32_t get_id() { return id; }

  void show() {
    std::cout << "+------------------Scooter------------------+\n";
    std::cout << "ID:\t\t" << id << '\n'
              << "BATTERY:\t" << battery << '\n'
              << "IS_FREE:\t" << is_free << '\n'
              << "STATION_ID:\t" << station_id << '\n';
  }
};

class Station {
  friend class ScooterPark;
  friend class Scooter;

  u_int32_t id;
  std::string name;
  std::map<u_int32_t, Scooter *> scooters;

protected:
  Station(u_int32_t id, std::string name,
          std::map<u_int32_t, Scooter *> scooters = {})
      : id(id), name(name), scooters(scooters) {}

  void add_scooter(Scooter *s) {
    if (scooters.find(s->get_id()) != scooters.end()) {
      throw std::runtime_error(
          "Trying to add an existing scooter to statation!");
    }
    scooters[s->get_id()] = s;
  }

  void remove_scooter(u_int32_t scid) {
    if (scooters.find(scid) == scooters.end()) {
      throw std::runtime_error(
          "Trying to remove not existing scooter from statation!");
    }
    scooters.erase(scid);
  }

  u_int32_t get_id() { return id; }

  void show() {
    std::cout << "+------------------Station------------------+\n";
    std::cout << "ID:\t\t" << id << '\n' << "NAME:\t\t" << name << '\n';
  }
};

class User {
  friend class ScooterPark;

private:
  u_int32_t id;
  std::string name;
  bool is_logged;
  double balance;

protected:
  bool is_riding;
  // std::vector<Scooter *> history;

  User(u_int32_t id, std::string name)
      : id(id), name(name), is_logged(false), balance(0) {}

  u_int32_t get_id() { return id; }

  void show() {
    std::cout << "+------------------User------------------+\n";
    std::cout << "ID:\t\t" << id << '\n' << "NAME:\t\t" << name << '\n';
  }

  ~User() {}
};

class ScooterPark {
  std::map<u_int32_t, Scooter *> scooters;
  std::map<u_int32_t, Station *> stations;
  std::map<u_int32_t, User *> users;

public:
  ScooterPark() {}

  // Users
  void user_register(std::string name) {
    u_int32_t id = 0;
    if (!users.empty()) {
      auto id_it = users.rbegin();
      id = id_it->second->get_id() + 1;
    }
    User *u = new User(id, name);
    users[id] = u;
  }

  // TODO: Реализовать более сложную логику входа в аккаунт
  bool user_log_in(u_int32_t uid) { return users.find(uid) != users.end(); }

  void show_db() {
    std::cout << "\n+------------------UserDB------------------+\n";
    for (auto user_pair : users) {
      user_pair.second->show();
    }
  }

  // Scooters|Stations

  void add_station(std::string name) {
    u_int32_t id = 0;
    if (!stations.empty()) {
      auto id_it = stations.rbegin();
      id = id_it->second->get_id() + 1;
    }
    Station *st = new Station(id, name);
    stations[id] = st;
  }

  void add_scooter(u_int32_t station_id) {
    u_int32_t id = 0;
    if (!scooters.empty()) {
      auto id_it = scooters.rbegin();
      id = id_it->second->get_id() + 1;
    }
    Scooter *s = new Scooter(id, station_id);
    stations[station_id]->add_scooter(s);
    scooters[id] = s;
  }

  void show_park() {
    for (auto station_pair : stations) {
      station_pair.second->show();
    }
    for (auto scooter_pair : scooters) {
      scooter_pair.second->show();
    }
  }

  void take_scooter(u_int32_t uid, u_int32_t scid) {
    if (users.find(uid) == users.end() ||
        scooters.find(scid) == scooters.end()) {
      throw std::runtime_error("There are no such scooter or user!");
    }

    User *u = users.find(uid)->second;
    Scooter *sc = scooters.find(scid)->second;
    Station *st = stations.find(sc->station_id)->second;

    u->is_riding = true;
    sc->is_free = false;
    st->remove_scooter(sc->get_id());
  }

  void return_scooter(u_int32_t uid, u_int32_t scid, u_int32_t stid) {
    if (users.find(uid) == users.end() ||
        scooters.find(scid) == scooters.end()) {
      throw std::runtime_error("There are no such scooter or user!");
    }

    User *u = users.find(uid)->second;
    Scooter *sc = scooters.find(scid)->second;
    Station *st = stations.find(stid)->second;

    u->is_riding = false;
    sc->is_free = true;
    sc->station_id = stid;
    st->add_scooter(sc);
  }

  bool is_scooter_exists(u_int32_t scid) {
    return scooters.find(scid) != scooters.end();
  }

  bool is_station_exists(u_int32_t stid) {
    return stations.find(stid) != stations.end();
  }

  ~ScooterPark() {
    for (auto scooter_pair : scooters) {
      delete scooter_pair.second;
    }
    for (auto station_pair : stations) {
      delete station_pair.second;
    }
    for (auto user_pair : users) {
      delete user_pair.second;
    }
  }
};
