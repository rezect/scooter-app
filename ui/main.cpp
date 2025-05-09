#include "client_user.cpp"
#include <SFML/Graphics.hpp>
#include <zmq.hpp>

sf::Font font("arialmt.ttf");

struct Station {
  sf::CircleShape station;
  sf::CircleShape station_area;
  // sf::Text label;
  uint32_t station_id = 0;
  sf::Vector2f pos = {0, 0};

  Station(uint32_t station_id, sf::Vector2f pos)
      : station_id(station_id), pos(pos) {
    station = sf::CircleShape(5.f);
    station.setOrigin(station.getGeometricCenter());
    station.setFillColor(sf::Color::Red);
    station.setPosition(pos);

    station_area = sf::CircleShape(40.f);
    station_area.setOrigin(station_area.getGeometricCenter());
    station_area.setFillColor(sf::Color::Black);
    station_area.setPosition(pos);
    station_area.setOutlineThickness(1);
  }

  void draw(sf::RenderWindow &window) const {
    window.draw(station_area);
    window.draw(station);
    // window.draw(label);
  }

  bool is_clicked(sf::Vector2f mouse_pos) const {
    return station_area.getGlobalBounds().contains(mouse_pos);
  }
};

struct ScooterButton {
  sf::RectangleShape shape;
  sf::Text label = sf::Text(font);
  uint32_t scooter_id = 0;
  int local_pos = 0;

  ScooterButton(uint32_t scid, int local_pos) : local_pos(local_pos) {
    scooter_id = scid;

    shape.setSize({200, 40});
    shape.setFillColor(sf::Color(100, 100, 255));
    shape.setPosition({550, 50 + local_pos * 50});

    label.setString("Scooter ID: " + std::to_string(scid));
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::White);
    label.setPosition(shape.getPosition() + sf::Vector2f{10, 5});
  }

  void decreese_local_pos() {
    --local_pos;
    shape.setPosition({550, 50 + local_pos * 50});
    label.setPosition(shape.getPosition() + sf::Vector2f{10, 5});
  }

  void draw(sf::RenderWindow &window) const {
    window.draw(shape);
    window.draw(label);
  }

  bool is_clicked(sf::Vector2f mouse_pos) const {
    return shape.getGlobalBounds().contains(mouse_pos);
  }
};

void parse_and_show(std::string &reply,
                    std::vector<ScooterButton> &scooter_buttons) {
  std::vector<std::string> scids = parse_comands(reply);
  for (int i = 0; i < scids.size(); ++i) {
    ScooterButton b(std::stoi(scids[i]), i);
    scooter_buttons.push_back(b);
    std::cout << "added scooter with id: " << scids[i] << '\n';
  }
}

int main() {
  // ZeroMQ stuff
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);
  socket.connect("tcp://localhost:5555");

  ClientUser cu;
  cu.log_in(0, socket);
  std::string comand = "";

  // UI stuff
  sf::RenderWindow window(sf::VideoMode({800, 800}), "SFML works!");
  std::vector<ScooterButton> scooter_buttons = {};

  // User circle
  sf::CircleShape user(2.f);
  user.setOrigin(user.getGeometricCenter());
  user.setFillColor(sf::Color::Green);

  // Station1 circle
  Station st1(0, {200, 300});
  Station st2(1, {400, 600});

  // Text
  sf::Font font("arialmt.ttf");
  sf::Text text(font);

  // Movement speed
  float speed = 100.f; // pixels per second
  sf::Clock clock;

  while (window.isOpen()) {
    sf::Time dt = clock.restart();

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto *but_pressed =
              event->getIf<sf::Event::MouseButtonPressed>()) {
        if (but_pressed->button == sf::Mouse::Button::Left) {
          sf::Vector2f mouse_pos =
              window.mapPixelToCoords(sf::Mouse::getPosition(window));

          if (st1.is_clicked(mouse_pos)) {
            scooter_buttons.clear();
            std::string reply = cu.send_comand("stsc:0", socket);
            parse_and_show(reply, scooter_buttons);
          }
          if (st2.is_clicked(mouse_pos)) {
            scooter_buttons.clear();
            std::string reply = cu.send_comand("stsc:1", socket);
            parse_and_show(reply, scooter_buttons);
          }
          for (auto it = scooter_buttons.begin(); it != scooter_buttons.end(); ++it) {
            if (it->is_clicked(mouse_pos)) {
              cu.take_scooter(it->scooter_id, socket);
              for (auto it_ch = it + 1; it_ch != scooter_buttons.end(); ++it_ch) {
                it_ch->decreese_local_pos();
              }
              scooter_buttons.erase(it);
              break;
            }
          }
        }
      }
    }

    // Continuous movement
    sf::Vector2f movement{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
      movement.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
      movement.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
      movement.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
      movement.x += 1.f;

    user.move(movement * speed * dt.asSeconds());

    sf::Vector2f pos = user.getPosition();
    text.setString("Position: " + std::to_string((int)pos.x) + ", " +
                   std::to_string((int)pos.y));

    window.clear();
    st1.draw(window);
    st2.draw(window);
    window.draw(text);
    window.draw(user);
    for (const auto &btn : scooter_buttons) {
      btn.draw(window);
    }
    window.display();
  }
}