#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

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


int main()
{
    ConfigReader configReader("");
    const auto& flights = configReader.getFlightData();

    for (const auto& flight : flights) {
        std::cout << "Flight: " << flight.flightNo << '\n';
    }

    return 0;
}
