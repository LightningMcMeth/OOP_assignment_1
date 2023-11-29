#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

struct FlightData {
    std::string date;
    std::string flightNo;
    int seatsInRow = 0;
    std::vector<std::pair<std::pair<int, int>, int>> rowPrices;
};

class ConfigReader {
public:
    ConfigReader(const std::string& filename) {
        processFile(filename);
    }

    const std::vector<FlightData>& getFlightData() const {
        return flightData;
    }

private:
    std::vector<FlightData> flightData;

    void processFile(const std::string& filename) {

        std::ifstream file(filename);
        std::string line;
        int numRecords;

        if (!file.is_open()) {
            std::cerr << "Error opening file :(" << '\n';
            return;
        }

        if (getline(file, line)) {
            std::istringstream(line) >> numRecords;
        }

        while (getline(file, line)) {

            FlightData data = parseLine(line);
            flightData.push_back(data);
        }
    }

    FlightData parseLine(const std::string& line) {

        std::istringstream stream(line);
        FlightData data;

        stream >> data.date >> data.flightNo >> data.seatsInRow;

        int rangeStart, rangeEnd, price;
        char dash;

        while (stream >> rangeStart >> dash >> rangeEnd >> price) {
            data.rowPrices.push_back(std::make_pair(std::make_pair(rangeStart, rangeEnd), price));
        }

        return data;
    }
};

class Airplane {
public:

    Airplane() {}
    
    ~Airplane() {}

private:
    FlightData flightInfo;
};

class Ticket {
public:

private:

};

class UI {
public:

    UI() {}

    ~UI() {}

    void setFlightData(const std::vector<Airplane>& flights) {
        this->flights = flights;
    }

    const std::vector<Airplane>& getFlightData() const {
        return flights;
    }

private:
    std::vector<Airplane> flights;

};

int main()
{
    ConfigReader configReader("C:\\Users\\markh\\OneDrive\\Documents\\Gamer repositories\\OOP\\OOP_assignment_1\\OOP_assignment_1\\test.txt");
    UI Interface;


    //Interface.setFlightData(configReader.getFlightData());
    //std::cout << Interface.getFlightData()[0].flightNo << '\n';

    return 0;
}
