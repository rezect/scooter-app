#include "scooter-system/systems.cpp"

int main() {
  ScooterPark park;
  UserDB user_db;
  park.add_station("Medvedkovo");
  park.add_station("Sviblovo");
  park.add_station("Otradnoe");
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 10; ++j) {
      park.add_scooter(i);
    }
  }
  park.show_park();
  user_db.user_register("Vanya");
  user_db.user_register("Katya");
  user_db.show_db();
  return 0;
}