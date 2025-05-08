#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

class Scooter {
  friend class ScooterPark;
  friend class Station;

  u_int32_t id;
  u_int16_t battery;
  bool is_free;
  u_int32_t station_id;

 protected:
  Scooter(u_int32_t id, u_int32_t station_id) : id(id), battery(100), is_free(true), station_id(station_id) {}

  u_int32_t get_id() {
    return id;
  }

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
  std::map<u_int32_t, Scooter*> scooters;

 protected:
  Station(u_int32_t id, std::string name, std::map<u_int32_t, Scooter*> scooters = {}) : id(id), name(name), scooters(scooters) {}

  void add_scooter(Scooter* s) {
    if (scooters.find(s->get_id()) != scooters.end()) {
      throw std::runtime_error("Trying to add an existing scooter to statation!");
    }
    scooters[s->get_id()] = s;
  }

  u_int32_t get_id() {
    return id;
  }

  void show() {
    std::cout << "+------------------Station------------------+\n";
    std::cout << "ID:\t\t" << id << '\n'
      << "NAME:\t\t" << name << '\n';
  }
};

class ScooterPark {
  std::map<u_int32_t, Scooter*> scooters;
  std::map<u_int32_t, Station*> stations;

 public:
  ScooterPark() {}

  void add_station(std::string name) {
    u_int32_t id = 0;
    if (!stations.empty()) {
      auto id_it = --stations.end();
      id = (*id_it).second->get_id() + 1;
    }
    Station* st = new Station(id, name);
    stations[id] = st;
  }

  void add_scooter(u_int32_t station_id) {
    u_int32_t id = 0;
    if (!scooters.empty()) {
      auto id_it = --scooters.end();
      id = (*id_it).second->get_id() + 1;
    }
    Scooter* s = new Scooter(id, station_id);
    stations[station_id]->add_scooter(s);
    scooters[id] = s;
  }

  void show_park() {
    for (auto station_pair: stations) {
      station_pair.second->show();
    }
    for (auto scooter_pair: scooters) {
      scooter_pair.second->show();
    }
  }
};

int main() {
  ScooterPark moscow_park;
  moscow_park.add_station("Medvedkovo1");
  moscow_park.add_station("Medvedkovo2");
  moscow_park.add_scooter(0);
  moscow_park.add_scooter(0);
  moscow_park.add_scooter(0);
  moscow_park.add_scooter(1);
  moscow_park.show_park();
  return 0;
}