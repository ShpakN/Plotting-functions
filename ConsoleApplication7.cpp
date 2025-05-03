#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

class Point {
public:
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

class Range {
public:
    double min, max;
    Range(double min, double max) : min(min), max(max) {}
};

class Function {
public:
    virtual double evaluate(double x) = 0;
    virtual std::string getFormula() = 0;
};

class PolynomialFunction : public Function {
private:
    std::vector<double> coefficients;
public:
    PolynomialFunction(std::vector<double> coeffs) : coefficients(coeffs) {}

    double evaluate(double x) override {
        double result = 0;
        for (int i = 0; i < coefficients.size(); ++i) {
            result += coefficients[i] * std::pow(x, i);
        }
        return result;
    }

    std::string getFormula() override {
        return "Polynomial Function";
    }
};

class TrigonometricFunction : public Function {
private:
    std::string type;
    double amplitude, frequency, phaseShift;
public:
    TrigonometricFunction(std::string type, double amplitude, double frequency, double phaseShift)
        : type(type), amplitude(amplitude), frequency(frequency), phaseShift(phaseShift) {
    }

    double evaluate(double x) override {
        if (type == "sin") {
            return amplitude * std::sin(frequency * x + phaseShift);
        }
        else if (type == "cos") {
            return amplitude * std::cos(frequency * x + phaseShift);
        }
        return 0.0; // Unknown type
    }

    std::string getFormula() override {
        return "Trigonometric Function";
    }
};

class ExponentialFunction : public Function {
private:
    double base, coefficient;
public:
    ExponentialFunction(double coefficient, double base) : coefficient(coefficient), base(base) {}

    double evaluate(double x) override {
        return coefficient * std::pow(base, x);
    }

    std::string getFormula() override {
        return "Exponential Function";
    }
};

class Graph {
private:
    std::vector<Point> points;
    Function* function;
public:
    Graph(Function* func) : function(func) {}

    void generatePoints(Range xRange, int numPoints) {
        points.clear();
        double step = (xRange.max - xRange.min) / numPoints;
        for (int i = 0; i <= numPoints; ++i) {
            double x = xRange.min + i * step;
            double y = function->evaluate(x);
            points.emplace_back(x, y);
        }
    }

    const std::vector<Point>& getPoints() const {
        return points;
    }
};

class CoordinateSystem {
private:
    Range xRange;
    Range yRange;

public:
    CoordinateSystem(Range x, Range y) : xRange(x), yRange(y) {}

    void setRanges(Range newXRange, Range newYRange) {
        xRange = newXRange;
        yRange = newYRange;
    }

    Range getXRange() const { return xRange; }
    Range getYRange() const { return yRange; }
};

class PlotArea {
private:
    CoordinateSystem coordinateSystem;
    std::vector<Graph> graphs;
public:
    PlotArea(CoordinateSystem cs) : coordinateSystem(cs) {}

    void addGraph(Graph graph) {
        graphs.push_back(graph);
    }

    void clear() {
        graphs.clear();
    }

    const std::vector<Graph>& getGraphs() const {
        return graphs;
    }
};

class GraphPlotter {
private:
    PlotArea* plotArea;

    void drawAxes(sf::RenderWindow& window) {
        // Draw X axis
        sf::Vertex xAxis[] = {
            sf::Vertex(sf::Vector2f(0, 300), sf::Color::Black), // Start of X axis
            sf::Vertex(sf::Vector2f(800, 300), sf::Color::Black) // End of X axis
        };
        window.draw(xAxis, 2, sf::Lines);

        // Draw Y axis
        sf::Vertex yAxis[] = {
            sf::Vertex(sf::Vector2f(400, 0), sf::Color::Black), // Start of Y axis
            sf::Vertex(sf::Vector2f(400,            600), sf::Color::Black) // End of Y axis
        };
        window.draw(yAxis, 2, sf::Lines);

        // Label the axes
        sf::Font font;
        font.loadFromFile("arial.ttf"); // Ensure the font is available

        sf::Text xLabel("X", font, 20);
        xLabel.setPosition(780, 310); // Position for X axis label
        xLabel.setFillColor(sf::Color::Black); // Set text color to black
        window.draw(xLabel);

        sf::Text yLabel("Y", font, 20);
        yLabel.setPosition(420, 10); // Position for Y axis label
        yLabel.setFillColor(sf::Color::Black); // Set text color to black
        window.draw(yLabel);
    }

    void drawGrid(sf::RenderWindow& window) {
        const int step = 40; // Increase step to 40 pixels
        sf::Font font;

        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
            return; // Exit if font is not loaded
        }

        for (int i = 0; i <= 800; i += step) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(i, 0), sf::Color(200, 200, 200)),
                sf::Vertex(sf::Vector2f(i, 600), sf::Color(200, 200, 200))
            };
            window.draw(line, 2, sf::Lines);

            // Labels for X axis
            if (i != 400) { // Avoid overlap on Y axis
                sf::Text xValue(std::to_string((i - 400) / 20), font, 15);
                xValue.setPosition(i, 310); // Position for label
                xValue.setFillColor(sf::Color::Black);
                window.draw(xValue);
            }
        }

        for (int i = 0; i <= 600; i += step) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(0, i), sf::Color(200, 200, 200)),
                sf::Vertex(sf::Vector2f(800, i), sf::Color(200, 200, 200))
            };
            window.draw(line, 2, sf::Lines);

            // Labels for Y axis
            if (i != 300) { // Avoid overlap on X axis
                sf::Text yValue(std::to_string((300 - i) / 20), font, 15);
                yValue.setPosition(410, i); // Position for label
                yValue.setFillColor(sf::Color::Black);
                window.draw(yValue);
            }
        }
    }

public:
    GraphPlotter(PlotArea* area) : plotArea(area) {}

    void plot(sf::RenderWindow& window) {
        // First draw the grid
        drawGrid(window);

        // Then draw the axes
        drawAxes(window);

        // Then draw the graphs
        for (const auto& graph : plotArea->getGraphs()) {
            const auto& points = graph.getPoints();
            for (size_t i = 1; i < points.size(); ++i) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(points[i - 1].x * 20 + 400, -points[i - 1].y * 20 + 300), sf::Color::Black),
                    sf::Vertex(sf::Vector2f(points[i].x * 20 + 400, -points[i].y * 20 + 300), sf::Color::Black)
                };
                window.draw(line, 2, sf::Lines);
            }
        }
    }

    void clear() {
        plotArea->clear();
    }
};

class UserInterface {
public:
    void showMenu() {
        std::cout << "1. Построить многочлен\n";
        std::cout << "2. Построить тригонометрическую функцию\n";
        std::cout << "3. Построить показательную функцию\n";
        std::cout << "4. Изменить диапазон\n";
        std::cout << "5. Очистить графики\n"; // Новый пункт меню
        std::cout << "6. Выход\n";
    }

    void getNewRange(double& xMin, double& xMax, double& yMin, double& yMax) {
        std::cout << "Введите новый диапазон по оси X (min max): ";
        std::cin >> xMin >> xMax;
        std::cout << "Введите новый диапазон по оси Y (min max): ";
        std::cin >> yMin >> yMax;
    }

    void getPolynomialParameters(double& a, double& b, double& c) {
        std::cout << "Введите коэффициенты многочлена (a, b, c): ";
        std::cin >> a >> b >> c;
    }

    void getTrigonometricParameters(double& amplitude, double& frequency, double& phase) {
        std::cout << "Введите параметры тригонометрической функции (амплитуда, частота, фаза): ";
        std::cin >> amplitude >> frequency >> phase;
    }

    void getExponentialParameters(double& coefficient, double& base) {
        std::cout << "Введите параметры показательной функции (коэффициент, основание): ";
        std::cin >> coefficient >> base;
    }
};

int main() {
    setlocale(LC_ALL, "Rus");
    // Инициализация SFML и создание окна
    sf::RenderWindow window(sf::VideoMode(800, 600), "Graph Plotter");

    // Создание диапазонов
    Range xRange(-10, 10);
    Range yRange(-10, 10);
    CoordinateSystem coordinateSystem(xRange, yRange);
    PlotArea plotArea(coordinateSystem);
    GraphPlotter graphPlotter(&plotArea);

    // Создание и добавление функций
    PolynomialFunction polyFunc({ 1, 0, -1 }); // x^2 - 1
    Graph polyGraph(&polyFunc);
    polyGraph.generatePoints(xRange, 100);
    plotArea.addGraph(polyGraph);

    TrigonometricFunction sinFunc("sin", 1.0, 1.0, 0.0); // sin(x)
    Graph sinGraph(&sinFunc);
    sinGraph.generatePoints(xRange, 100);
    plotArea.addGraph(sinGraph);

    UserInterface ui;

    // Основной цикл
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Очистка окна
        window.clear(sf::Color::White);

        // Отрисовка графиков
        graphPlotter.plot(window);

        // Отображение содержимого окна
        window.display();

        // Показать меню и получить ввод пользователя
        ui.showMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: // Polynomial function
        {
            double a, b, c;
            ui.getPolynomialParameters(a, b, c);
            polyFunc = PolynomialFunction({ a, b, c });
            polyGraph.generatePoints(coordinateSystem.getXRange(), 100);
            plotArea.clear();
            plotArea.addGraph(polyGraph);
            break;
        }
        case 2: // Trigonometric function
        {
            double amplitude, frequency, phase;
            ui.getTrigonometricParameters(amplitude, frequency, phase);
            sinFunc = TrigonometricFunction("sin", amplitude, frequency, phase);
            sinGraph.generatePoints(coordinateSystem.getXRange(), 100);
            plotArea.clear();
            plotArea.addGraph(sinGraph);
            break;
        }
        case 3: // Exponential function
        {
            double coefficient, base;
            ui.getExponentialParameters(coefficient, base);
            ExponentialFunction expFunc(coefficient, base);
            Graph expGraph(&expFunc);
            expGraph.generatePoints(coordinateSystem.getXRange(), 100);
            plotArea.clear();
            plotArea.addGraph(expGraph);
            break;
        }
        case 4: // Change range
        {
            double xMin, xMax, yMin, yMax;
            ui.getNewRange(xMin, xMax, yMin, yMax);
            coordinateSystem.setRanges(Range(xMin, xMax), Range(yMin, yMax));
            polyGraph.generatePoints(coordinateSystem.getXRange(), 100);
            sinGraph.generatePoints(coordinateSystem.getXRange(), 100);

            plotArea.clear();
            plotArea.addGraph(polyGraph);
            plotArea.addGraph(sinGraph);
            break;
        }
        case 5: // Clear graphs
            graphPlotter.clear(); // Очищаем графики
            plotArea.clear(); // Также очищаем данные в plotArea
            break;
        case 6: // Exit
            window.close();
            break;
        default:
            std::cout << "Неверный выбор. Пожалуйста, попробуйте снова.\n";
            break;
        }
    }

    return 0;
}

