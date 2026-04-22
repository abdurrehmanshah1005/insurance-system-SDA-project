#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

struct Customer {
    int    customerId = 0;
    std::string name;
    std::string contact;
    std::string address;
    std::string createdDate;   // YYYY-MM-DD

    // ── Serialization ──
    std::string toCSV() const {
        return std::to_string(customerId) + "|" + name + "|" + contact + "|" + address + "|" + createdDate;
    }

    static Customer fromCSV(const std::string& line) {
        Customer c;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); c.customerId  = std::stoi(token);
        std::getline(ss, c.name,    '|');
        std::getline(ss, c.contact, '|');
        std::getline(ss, c.address, '|');
        std::getline(ss, c.createdDate, '|');
        return c;
    }

    void display() const {
        std::cout << "  ID: " << customerId
                  << " | Name: " << name
                  << " | Contact: " << contact
                  << " | Address: " << address
                  << " | Registered: " << createdDate << "\n";
    }

    int getId() const { return customerId; }
};

#endif // CUSTOMER_H
