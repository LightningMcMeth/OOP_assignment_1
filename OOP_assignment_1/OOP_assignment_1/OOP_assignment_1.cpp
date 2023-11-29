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

    Airplane(const FlightData& flightInfo) : flightInfo(flightInfo) {}
    
    ~Airplane() {}

    const std::vector<std::string> getBookedSeats() const{
        return bookedSeats;
    }

private:
    FlightData flightInfo;
    std::vector<std::string> bookedSeats;
};

class Ticket {
public:

    Ticket() {}

    Ticket(std::string D, std::string FN, std::string P, std::string N) {
        Date = D;
        FlightNo = FN;
        Place = P;
        Name = N;
    }

    ~Ticket() {}

private:
    std::string Date;
    std::string FlightNo;
    std::string Place;
    std::string Name;
};

class UI {
public:

    UI() {}

    ~UI() {}

    void setFlightData(const std::vector<FlightData>& flightData) {
        
        flights.clear();

        for (const auto& data : flightData) {
            flights[data.flightNo] = Airplane(data);
        }
    }

    const std::map<std::string, Airplane>& getFlights() const {
        return flights;
    }

    //that is not how this method should be written. Still might prove to be useful later on.
    bool checkSeatAvailability(const std::string& seat, const std::string& FlightNo) {

        auto flight = flights.find(FlightNo);

        if (flight != flights.end()) {

            const auto& bookedSeats = flight->second.getBookedSeats();
            return std::find(bookedSeats.begin(), bookedSeats.end(), seat) == bookedSeats.end();
        }

        return false;
    }

private:
    std::map<std::string, Airplane> flights;
};

int main()
{
    ConfigReader configReader("test.txt");
    UI Interface;

    Interface.setFlightData(configReader.getFlightData());

    std::string commandType;

    while (true) {

        std::cout << "Enter your command(check, book, return, view): " << '\n';

        if (commandType == "check") {
        }
    }
    

    return 0;
}
