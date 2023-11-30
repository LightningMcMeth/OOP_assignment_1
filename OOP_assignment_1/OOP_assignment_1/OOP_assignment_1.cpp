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
        char dash, moneh; //money but spelled in a funny way. This is for the $ symbol

        while (stream >> rangeStart >> dash >> rangeEnd >> price >> moneh) {

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

    const std::vector<std::string>& getBookedSeats() const{
        return bookedSeats;
    }

    void bookSeat(const std::string& seat) {
        bookedSeats.push_back(seat);
    }

    const std::string& getDate() const{
        return flightInfo.date;
    }

    const int& getSeatsInRow() const {
        return flightInfo.seatsInRow;
    }

    const std::vector<std::pair<std::pair<int, int>, int>>& getRowPrices() const {
        return flightInfo.rowPrices;
    }

private:
    FlightData flightInfo;
    std::vector<std::string> bookedSeats;
};

class Ticket {
public:

    Ticket() {}

    Ticket(const std::string& D, const std::string& FN, const std::string& S, const std::string& N, const int& P)
        : date(D), flightNo(FN), seat(S), name(N), price(P) {}

    ~Ticket() {}

private:
    std::string date;
    std::string flightNo;
    std::string seat;
    std::string name;
    int price = 0;
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

    std::string bookSeat(const std::string& Date, const std::string& FlightNo, const std::string& seat, const std::string& Username) {

        auto flightI = flights.find(FlightNo);
        const Airplane& airplane = flightI->second;

        if (flightI == flights.end()) {

            std::cout << "Flight number not found :(\n";
            return "";
        }

        if (!isSeatValid(seat, airplane)) {
            std::cout << "Invalid seat.\n";
            return "";
        }

        if (flightI->second.getDate() != Date) {

            std::cout << "Seat not available :(\n";
            return "";
        }
        else if (!IsSeatAvailable(seat, FlightNo)) {

            std::cout << "Invalid date.\n";
            return "";
        }
    
        int seatPrice = calculateSeatPrice(seat, airplane);
        int ticketId = generateTicketId();

        if (seatPrice == 0) {

            std::cout << "Error finding seat price.\n";
            return "";
        }

        Ticket userTicket(Date, FlightNo, seat, Username, seatPrice);
        tickets[ticketId] = userTicket;

        flightI->second.bookSeat(seat);

        return std::to_string(ticketId);
    }

private:
    std::map<std::string, Airplane> flights;
    std::map<int,Ticket> tickets;
    int currentId = 0;

    int generateTicketId() {
        return ++currentId;
    }

    bool IsSeatAvailable(const std::string& seat, const std::string& FlightNo) {

        auto flight = flights.find(FlightNo);

        if (flight != flights.end()) {

            const auto& bookedSeats = flight->second.getBookedSeats();
            return std::find(bookedSeats.begin(), bookedSeats.end(), seat) == bookedSeats.end();
        }

        return false;
    }

    bool isSeatValid(const std::string& seat, const Airplane& airplane) {

        if (seat.length() < 2) {
            return false;
        }

        int rowNum = std::stoi(seat.substr(0, seat.length() - 1));
        char seatChar = seat.back();

        if (!isRowValid(rowNum, airplane.getRowPrices())) {
            return false;
        }

        int charLimit = 'A' + airplane.getSeatsInRow() - 1;
        return seatChar >= 'A' && seatChar <= charLimit;
    }

    bool isRowValid(const int& rowNum, const std::vector<std::pair<std::pair<int, int>, int>>& rowPrices) {

        for (const auto& priceRange : rowPrices) {

            if (rowNum >= priceRange.first.first && rowNum <= priceRange.first.second) {
                return true;
            }
        }
        return false;
    }

    int calculateSeatPrice(const std::string& seat, const Airplane& airplane) {

        int rowNum = std::stoi(seat.substr(0, seat.length() - 1));

        for (const auto& priceRange : airplane.getRowPrices()) {

            if (rowNum >= priceRange.first.first && rowNum <= priceRange.first.second) {
                return priceRange.second;
            }
        }
        return 0;
    }
};

int main()
{
    ConfigReader configReader("test.txt");
    UI Interface;

    Interface.setFlightData(configReader.getFlightData());

    std::string commandType;

    while (true) {

        std::cin.ignore();
        std::cout << "\nEnter your command(check, book, return, view): " << '\n';
        std::cin >> commandType;

        if (commandType == "check") {

            std::cout << "gamer\n";
        }
        else if (commandType == "book") {

            std::string date, flightNo, seat, username;
            std::cout << "Enter flight date, number, seat and your name: \n";
            std::cin >> date >> flightNo >> seat >> username;

            std::string result = Interface.bookSeat(date, flightNo, seat, username);

            if (result == "") {
                std::cout << "\nCouldn't book ticket. Cry about it.\n";
            }
            else {
                std::cout << "Booking confirmed with ID " << result << '\n';
            }
            
        }
        else if (commandType == "return") {

        }
        else if (commandType == "view") {
            //since depending on the user input, I can defferenciate ID from Username by
            //checking if the last character of the input string can be converted to an int
            //if you can convert it, you're checking by ID and vise versa
        }
    }
    
    return 0;
}
