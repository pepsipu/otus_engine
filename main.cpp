#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <fstream>

#include "http.hpp"
#include "json.hpp"
#include "notify.h"

#define OTUS_GATEWAY std::string {"http://api.otus.pepsipu.com/"}

using json = nlohmann::json;

int main() {
    std::string image_name;
    std::string username;
    std::cout << "Otus Engine Copyright © 2019 Sammy Hajhamid." << std::endl;
    std::cout << "Enter Otus Image Name: ";
    std::cin >> image_name;
    std::cout << "Enter Competitor Identification: ";
    std::cin >> username;
    json registrationData = {
            {"image_name", image_name},
            {"username",   username},
    };
    std::string data;
    try {
        http::Request request{OTUS_GATEWAY + "compete/"};
        http::Response response = request.send("POST", registrationData.dump(), {
                "Content-Type: application/json"
        });
        data = {response.body.begin(), response.body.end()};
    }
    catch (const std::exception &e) {
        std::cerr << "Error Contacting Otus Server: " << e.what() << std::endl;
        return 1;
    }
    json response;
    try {
        response = json::parse(data);
    } catch (const std::exception &e) {
        if (data == "") {
            std::cerr << "The server did not send anything back. This is likely because it did not know how to handle your input. Contact an Otus developer.";
        } else {
            std::cerr << "The server sent back unreadable text.\n" << data << std::endl;
        }
        return 1;
    }
    if (!response["success"]) {
        if (response["err"] == "image_void") {
            std::cerr << "Image does not exist.";
        } else if (response["err"] == "username_in_use") {
            std::cerr << "That competitor name is taken.";
        }
        return 1;
    }
    json correct = json::object();
    int ic = 0;
    while (true) {
        json new_correct = json::object();
        for (int unsigned long i = 0; i < response["vulnerabilities"].size(); ++i) {
            std::string command = response["vulnerabilities"][i]["command"];
            std::string success_message = response["vulnerabilities"][i]["success_message"];
            if (system(command.c_str()) == response["vulnerabilities"][i]["status_code"]) {
                if (!correct.contains(std::to_string(i))) {
                    score((char *) success_message.c_str());
                }
                new_correct[std::to_string(i)] = response["vulnerabilities"][i];
            }
            system("clear");
        }
        correct = new_correct;
        std::ofstream scoring_report;
        scoring_report.open("scoring_report.json");
        scoring_report << correct.dump();
        scoring_report.close();
        json vulns = json::array();
        for (auto i: correct.items()) {
            vulns.push_back(i.value());
        }
        json heartbeat_data = {
                {"correct", vulns},
                {"session", response["session"]}
        };
        if (ic == 2) {
            ic = 0;
            http::Request request{OTUS_GATEWAY + "heartbeat/"};
            http::Response r = request.send("POST", heartbeat_data.dump(), {
                    "Content-Type: application/json"
            });
            std::string new_data = {r.body.begin(), r.body.end()};
            std::cout << new_data;
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));
        ++ic;
    }

}