#include "header/CommonHF.h"

int main()
{
    Vec2 p1(100, 200);
    Vec2 p2(100, 300);

    Vec2 p3 = p1 + p2;
    p3.scale(2).add(10).add(10);


    std::cout << p3;

    if (p1 == p2)
    {
        std::cout << "Vectors are equal" << "\n";
    }
    else
    {
        std::cout << "Vectors are NOT equal" << "\n";
    }

    try
    {
        // get anti-aliased shapes 
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;

        std::ifstream inputFile("config.txt");

        // Check if the file is successfully opened 
        if (!inputFile.is_open()) {
            std::cerr << "Error opening the file!" << "\n";
            return 1;
        }

        int winWidth, winHeight;
        std::string fontFileName;
        int fontSize, fontR, fontG, fontB;

        std::vector<std::shared_ptr<sf::Shape>> ShapesContainer;
        std::vector<std::string> shapeNames;
        std::vector<std::pair<float, float>> shapeVelocities;

        std::string line;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "Windows") {
                iss >> winWidth >> winHeight;
            }
            else if (type == "Font") {
                iss >> fontFileName >> fontSize >> fontR >> fontG >> fontB;
            }
            else if (type == "Circle") {
                float x, y, vx, vy, rad;
                int r, g, b;

                std::string shapeName;
                iss >> shapeName;
                shapeNames.push_back(shapeName);
                iss >> x >> y >> vx >> vy >> r >> g >> b >> rad;

                std::shared_ptr<sf::Shape> circle = std::make_shared<sf::CircleShape>(rad);
                circle->setPosition(x, y);
                circle->setFillColor(sf::Color(r, g, b));
                shapeVelocities.push_back(std::make_pair(vx, vy));
                ShapesContainer.push_back(circle);
            }
            else if (type == "Rectangle") {
                float x, y, vx, vy, size1, size2;
                int r, g, b;

                std::string shapeName;
                iss >> shapeName;
                shapeNames.push_back(shapeName);
                iss >> x >> y >> vx >> vy >> r >> g >> b >> size1 >> size2;

                std::shared_ptr<sf::Shape> rectangle = std::make_shared<sf::RectangleShape>(sf::Vector2f(size1, size2));
                rectangle->setPosition(x, y);
                rectangle->setFillColor(sf::Color(r, g, b));
                shapeVelocities.push_back(std::make_pair(vx, vy));
                ShapesContainer.push_back(rectangle);
            }
            else if (type == "Triangle") {
                float x1, y1, x2, y2, x3, y3, vx, vy;
                int r, g, b;

                std::string shapeName;
                iss >> shapeName;
                shapeNames.push_back(shapeName);
                iss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> vx >> vy >> r >> g >> b;

                std::shared_ptr<sf::ConvexShape> triangle = std::make_shared<sf::ConvexShape>(3);
                triangle->setPoint(0, sf::Vector2f(x1, y1));
                triangle->setPoint(1, sf::Vector2f(x2, y2));
                triangle->setPoint(2, sf::Vector2f(x3, y3));
                triangle->setFillColor(sf::Color(r, g, b));
                shapeVelocities.push_back(std::make_pair(vx, vy));
                ShapesContainer.push_back(triangle);
            }
            else if (type == "Ellipse") {
                float x, y, vx, vy, r1, r2;
                int r, g, b;

                std::string shapeName;
                iss >> shapeName;
                shapeNames.push_back(shapeName);
                iss >> x >> y >> vx >> vy >> r >> g >> b >> r1 >> r2;

                std::shared_ptr<sf::CircleShape> ellipse = std::make_shared<sf::CircleShape>(r1);
                ellipse->setPosition(x, y);
                ellipse->setScale(1.f, r2 / r1);
                ellipse->setFillColor(sf::Color(r, g, b));
                shapeVelocities.push_back(std::make_pair(vx, vy));
                ShapesContainer.push_back(ellipse);
            }
        }



        // Close the file
        inputFile.close();

        // SFML window
        sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "SFML Game!", sf::Style::Default, settings);
        window.setFramerateLimit(120);

        // load font
        sf::Font font;

        // load font file
        std::cout << fontFileName << " " << fontSize << " " << fontR << " " << fontG << " " << fontB << "\n";
        if (!font.loadFromFile(fontFileName))
        {
            // if can't load the font file
            std::cerr << "Failed to load font\n";
            exit(-1);
        }

        // to display size on bottom left
        sf::Text text("My first sfml Window", font, 24);
        text.setFillColor(sf::Color(fontR, fontG, fontB));
        sf::FloatRect textRect = text.getLocalBounds();
        text.setPosition(winWidth - textRect.width, 0);

        std::vector<sf::Text> shapeNamesText;
        for (size_t i = 0; i < ShapesContainer.size(); i++)
        {
            sf::Text text;
            text.setFont(font);
            text.setString(shapeNames[i]);
            text.setCharacterSize(fontSize);
            text.setFillColor(sf::Color(fontR, fontG, fontB));
            text.setOutlineColor(sf::Color::Yellow);

            // Set the origin of the text to its center
            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

            // Position the text in the center of the shape (Commented out since its being called at every frame when during draw)
            //sf::FloatRect shapeBounds = ShapesContainer[i]->getGlobalBounds();
            //text.setPosition(shapeBounds.left + shapeBounds.width / 2.0f, shapeBounds.top + shapeBounds.height / 2.0f);

            shapeNamesText.push_back(text);
        }

        // main loop - continuous for each frame while window is open
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                // this event is triggered when the close button is clicked
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }

            // Move shapes based on their velocities
            for (size_t i = 0; i < ShapesContainer.size(); i++)
            {
                float vx = ShapesContainer[i]->getPosition().x + shapeVelocities[i].first;
                float vy = ShapesContainer[i]->getPosition().y + shapeVelocities[i].second;

                // Check for collisions with window edges
                if (vx < 0.f || vx + ShapesContainer[i]->getGlobalBounds().width > winWidth)
                {
                    shapeVelocities[i].first *= -1;
                    vx = std::max(0.0f, std::min(vx, static_cast<float>(winWidth) - ShapesContainer[i]->getGlobalBounds().width));
                }

                if (vy < 0 || vy + ShapesContainer[i]->getGlobalBounds().height > winHeight)
                {
                    shapeVelocities[i].second *= -1;
                    vy = std::max(0.0f, std::min(vy, static_cast<float>(winHeight) - ShapesContainer[i]->getGlobalBounds().height));
                }

                for (size_t j = 0; j < ShapesContainer.size(); j++)
                {
                    if (i != j)
                    {
                        if (ShapesContainer[i]->getGlobalBounds().intersects(ShapesContainer[j]->getGlobalBounds()))
                        {
                            std::cout << "COLLISION Between " << shapeNames[i] << " and " << shapeNames[j] << "\n";
                            // Reverse the velocities of the shapes upon collision
                            std::swap(shapeVelocities[i], shapeVelocities[j]);
                            vx = ShapesContainer[i]->getPosition().x + shapeVelocities[i].first;
                            vy = ShapesContainer[i]->getPosition().y + shapeVelocities[i].second;
                        }
                    }
                }

                ShapesContainer[i]->setPosition(vx, vy);
                sf::FloatRect textRect = ShapesContainer[i]->getGlobalBounds();
                shapeNamesText[i].setPosition(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            }


            /* Calling clear before drawing anything is mandatory,
               otherwise the contents from previous frames will be present behind anything you draw */
            window.clear();
            for (auto& shape : ShapesContainer)
            {
                window.draw(*shape);
            }
            for (auto& shapeName : shapeNamesText)
            {
                window.draw(shapeName);
            }
            window.draw(text);
            window.display();
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}