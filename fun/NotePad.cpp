#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"
#include <iostream>
#include <string>

using namespace sf;

int main() {

	RenderWindow window(VideoMode(800, 800), "Project4");
	sf::Font font;

	if (!font.loadFromFile("D:\\projects\\Project4\\malgunsl.ttf"))
		return EXIT_FAILURE;
	std::string str = "";
	sf::Text text(str,font, 50);

	text.setFillColor(Color::Black);
	int k = 1;

	while (window.isOpen()) {
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			switch (event.type)
			{
			case (Event::TextEntered):
			{							  
				if (static_cast<float>(str.length()) + text.getGlobalBounds().width > k * window.getSize().x)
				{
					k++;
					str.push_back(event.text.unicode);
					str.push_back('\n');
				}
				else {

					str.push_back(event.text.unicode);
				}
				text.setString(str);
				break;
			}
			case(Keyboard::Delete):
			{
				str.pop_back();
				text.setString(str);
				break;
			}
			default:
				break;
			}
		}

		window.clear(Color::White);
		window.draw(text);
		window.display();}



	return 0;
}
