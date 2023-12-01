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

    const std::vector<std::string>& getBookedSeats() const {
        return bookedSeats;
    }

    void bookSeat(const std::string& seat) {
        bookedSeats.push_back(seat);
    }

    void removeSeat(const std::string& seat) {

        auto it = std::find(bookedSeats.begin(), bookedSeats.end(), seat);

        if (it != bookedSeats.end()) {
            bookedSeats.erase(it);
        }
        else {
            std::cout << "\nInvalid seat.\n";
        }
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

    std::vector<std::pair<std::string, int>> getFreeSeats(const std::string& date, const std::string& flightNo) const {

        std::vector<std::pair<std::string, int>> freeSeats;

        for (const auto& priceRange : flightInfo.rowPrices) {

            for (int rowNum = priceRange.first.first; rowNum <= priceRange.first.second; ++rowNum) {

                for (char seatChar = 'A'; seatChar < 'A' + flightInfo.seatsInRow; ++seatChar) {

                    std::string seat = std::to_string(rowNum) + seatChar;

                    if (std::find(bookedSeats.begin(), bookedSeats.end(), seat) == bookedSeats.end()) {
                        freeSeats.push_back({ seat, priceRange.second });
                    }
                }
            }
        }

        return freeSeats;
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

    const std::string& getFlightNo() const{
        return flightNo;
    }

    const std::string& getSeat() const {
        return seat;
    }

    const int& getSeatPrice() const {
        return price;
    }

    const std::string& getName() const {
        return name;
    }

    const std::string& getDate() const {
        return date;
    }

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

    std::string returnTicket(const int& ticketId) {

        auto ticketsI = tickets.find(ticketId);

        if (ticketsI == tickets.end()) {

            std::cout << "Invalid ticket ID.\n";
            return "";
        }

        const std::string& flightNo = ticketsI->second.getFlightNo();
        const std::string& seat = ticketsI->second.getSeat();

        auto flightI = flights.find(flightNo);

        if (flightI != flights.end()) {

            flightI->second.removeSeat(seat);
        }
        else {

            std::cout << "Flight not found. If this error appears, something is very wrong.\n";
            return "";
        }

        std::string price = std::to_string(ticketsI->second.getSeatPrice());
        std::string name = ticketsI->second.getName();
        std::string result = "Confirmed $" + price + " refund for " + name;

        tickets.erase(ticketsI);
        return result;
    }

    void checkAvailableSeats(const std::string& date, const std::string& flightNo) {

        auto flightI = flights.find(flightNo);

        if (flightI != flights.end() && flightI->second.getDate() == date) {

            auto freeSeats = flightI->second.getFreeSeats(date, flightNo);

            std::cout << "\nAvailable seats:\n\n";

            int counter = 0;
            for (const auto& seatInfo : freeSeats) {

                std::cout << seatInfo.first << "-$" << seatInfo.second << ' ';
                counter++;

                if (counter % 5 == 0) {
                    std::cout << '\n';
                }
            }
        }
        else {
            std::cout << "Flight not found :(\n";
        }
    }

    void viewTicketById(const int& ticketId) {

        auto ticketI = tickets.find(ticketId);

        if (ticketI == tickets.end()) {

            std::cout << "Invalid ID.\n";
            return;
        }

        const Ticket& ticket = ticketI->second;

        std::cout << "Flight " << ticket.getFlightNo() << ", " << ticket.getDate() << ", seat " << ticket.getSeat() 
                  << ", price $" << ticket.getSeatPrice() << ", " << ticket.getName() << '\n';
    }

    void viewTicketsByUsername(const std::string& username) {

        bool found = false;
        int count = 1;

        for (const auto& ticketP : tickets) {

            const Ticket& ticket = ticketP.second;

            if (ticket.getName() == username) {

                std::cout << count << ". Flight " << ticket.getFlightNo() << ", " << ticket.getDate() << ", seat " 
                          << ticket.getSeat() << ", price $" << ticket.getSeatPrice() << '\n';

                found = true;
                count++;
            }
        }

        if (!found) {
            std::cout << "No tickets found for " << username << ".\n";
        }
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
    std::string commandType;

    Interface.setFlightData(configReader.getFlightData());

    while (true) {

        std::cin.ignore();
        std::cout << "\nEnter your command(check, book, return, view): " << '\n';
        std::cin >> commandType;

        if (commandType == "check") {

            std::string date, flightNo;
            std::cout << "Enter flight date and number: \n";
            std::cin >> date >> flightNo;

            Interface.checkAvailableSeats(date, flightNo);

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

            std::cout << "Enter ticket ID: \n";

            int ticketId;
            std::cin >> ticketId;

            std::string result = Interface.returnTicket(ticketId);

            if (result == "") {
                std::cout << "\nCouldn't return ticket.\n";
            }
            else {
                std::cout << result << '\n';
            }
        }
        else if (commandType == "view") {
            
            std::cout << "Enter ticket ID or a username: \n";
            std::string input;
            std::cin >> input;

            std::istringstream iss(input);
            int ticketId;

            if (iss >> ticketId && iss.eof()) {
                Interface.viewTicketById(ticketId);
            }
            else {
                Interface.viewTicketsByUsername(input);
            }
        }
        else if (commandType == "exit") {
            return 0;
        }
    }
    
    return 0;
}
