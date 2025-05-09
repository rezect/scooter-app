#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

class Scooter {
  friend class ScooterPark;
  friend class Station;
  friend class User;

  uint32_t id;
  u_int16_t battery;

protected:
  bool is_free;
  uint32_t station_id;

  Scooter(uint32_t id, uint32_t station_id)
      : id(id), battery(100), is_free(true), station_id(station_id) {}

  uint32_t get_id() { return id; }

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

  uint32_t id;
  std::string name;
  std::map<uint32_t, Scooter *> scooters;

protected:
  Station(uint32_t id, std::string name,
          std::map<uint32_t, Scooter *> scooters = {})
      : id(id), name(name), scooters(scooters) {}

  void add_scooter(Scooter *s) {
    if (scooters.find(s->get_id()) != scooters.end()) {
      throw std::runtime_error(
          "Trying to add an existing scooter to statation!");
    }
    scooters[s->get_id()] = s;
  }

  void remove_scooter(uint32_t scid) {
    if (scooters.find(scid) == scooters.end()) {
      throw std::runtime_error(
          "Trying to remove not existing scooter from statation!");
    }
    scooters.erase(scid);
  }

  std::vector<uint32_t> get_scooters_id() {
    std::vector<uint32_t> scids = {};
    for (auto scooter_pair: scooters) {
      scids.push_back(scooter_pair.second->id);
    }
    return scids;
  }

  uint32_t get_id() { return id; }

  void show() {
    std::cout << "+------------------Station------------------+\n";
    std::cout << "ID:\t\t" << id << '\n' << "NAME:\t\t" << name << '\n';
  }
};

class User {
  friend class ScooterPark;

private:
  uint32_t id;
  std::string name;
  bool is_logged;
  double balance;

protected:
  bool is_riding;
  // std::vector<Scooter *> history;

  User(uint32_t id, std::string name)
      : id(id), name(name), is_logged(false), balance(0) {}

  uint32_t get_id() { return id; }

  void show() {
    std::cout << "+------------------User------------------+\n";
    std::cout << "ID:\t\t" << id << '\n' << "NAME:\t\t" << name << '\n';
  }

  ~User() {}
};

class ScooterPark {
  std::map<uint32_t, Scooter *> scooters;
  std::map<uint32_t, Station *> stations;
  std::map<uint32_t, User *> users;

public:
  ScooterPark() {}

  // Users
  void user_register(std::string name) {
    uint32_t id = 0;
    if (!users.empty()) {
      auto id_it = users.rbegin();
      id = id_it->second->get_id() + 1;
    }
    User *u = new User(id, name);
    users[id] = u;
  }

  // TODO: Реализовать более сложную логику входа в аккаунт
  bool user_log_in(uint32_t uid) { return users.find(uid) != users.end(); }

  void show_db() {
    std::cout << "\n+------------------UserDB------------------+\n";
    for (auto user_pair : users) {
      user_pair.second->show();
    }
  }

  // Scooters|Stations

  void add_station(std::string name) {
    uint32_t id = 0;
    if (!stations.empty()) {
      auto id_it = stations.rbegin();
      id = id_it->second->get_id() + 1;
    }
    Station *st = new Station(id, name);
    stations[id] = st;
  }

  void add_scooter(uint32_t station_id) {
    uint32_t id = 0;
    if (!scooters.empty()) {
      auto id_it = scooters.rbegin();
      id = id_it->second->get_id() + 1;
    }
    Scooter *s = new Scooter(id, station_id);
    stations[station_id]->add_scooter(s);
    scooters[id] = s;
  }

  std::vector<uint32_t> station_scooters(uint32_t stid) {
    if (stations.find(stid) == stations.end()) {
      throw std::runtime_error("No such station!");
    }
    Station *st = stations.find(stid)->second;
    return st->get_scooters_id();
  }

  uint32_t* scooter_info(uint32_t scid) {
    // array : [<id>, <battery>, <is_free>, <station_id>]
    uint32_t scinfo[4] = {0};
    Scooter* sc = scooters.find(scid)->second;
    scinfo[0] = sc->id;
    scinfo[1] = sc->battery;
    scinfo[2] = sc->is_free;
    scinfo[3] = sc->station_id;
    return scinfo;
  }

  // std::unordered_map<std::string, uint32_t> station_info(uint32_t stid) {
  //   std::unordered_map<std::string, uint32_t> stinfo = {};
  //   Station* st = stations.find(stid)->second;
  //   stinfo["id"] = st->id;
  //   stinfo["battery"] = st->battery;
  //   stinfo["is_free"] = st->is_free;
  //   stinfo["station_id"] = st->station_id;
  //   return scinfo;
  // }

  void show_park() {
    for (auto station_pair : stations) {
      station_pair.second->show();
    }
    for (auto scooter_pair : scooters) {
      scooter_pair.second->show();
    }
  }

  void take_scooter(uint32_t uid, uint32_t scid) {
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

  void return_scooter(uint32_t uid, uint32_t scid, uint32_t stid) {
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

  bool is_scooter_exists(uint32_t scid) {
    return scooters.find(scid) != scooters.end();
  }

  bool is_station_exists(uint32_t stid) {
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
