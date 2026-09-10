#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <vector>
using namespace std;

const string DATA_FILE = "records.txt";
const int W = 47;  // inner box width (total line = W+2 = 49)

string line(char c)   { return "+" + string(W, c) + "+"; }
string top()          { return line('='); }
string mid()          { return line('-'); }
string row(const string& s) { return "| " + s + string(W - 2 - s.size(), ' ') + " |"; }
string pad(const string& s, int w) { return s + string(max(0, w - (int)s.size()), ' '); }

class UserInfo {
private:
    string cardNumber;
    string computerNumber;
    string startTime;
public:
    UserInfo() {}
    UserInfo(string card, string computer, string time)
        : cardNumber(card), computerNumber(computer), startTime(time) {}
    string getCard() const { return cardNumber; }
    string getComputer() const { return computerNumber; }
    string getTime() const { return startTime; }
    double calculateFee() const {
        tm tm = {};
        sscanf_s(startTime.c_str(), "%d-%d-%d %d:%d:%d",
            &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
        tm.tm_year -= 1900; tm.tm_mon -= 1;
        time_t start = mktime(&tm);
        double hours = difftime(time(nullptr), start) / 3600.0;
        return max(hours, 0.1) * 2.0;
    }
};

string getTime() {
    time_t t = time(nullptr);
    tm tm; localtime_s(&tm, &t);
    char buf[50]; strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

void saveUser(const UserInfo& u) {
    ofstream f(DATA_FILE, ios::app);
    f << u.getCard() << " " << u.getComputer() << " " << u.getTime() << endl;
}

vector<UserInfo> loadUsers() {
    vector<UserInfo> users;
    ifstream f(DATA_FILE);
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        string card, computer, datePart, timePart;
        istringstream iss(line);
        iss >> card >> computer >> datePart >> timePart;
        users.emplace_back(card, computer, datePart + " " + timePart);
    }
    return users;
}

void saveUsers(const vector<UserInfo>& users) {
    ofstream f(DATA_FILE);
    for (const auto& u : users)
        f << u.getCard() << " " << u.getComputer() << " " << u.getTime() << endl;
}

void checkIn() {
    system("cls");
    string card, computer;
    cout << top() << "\n";
    cout << row("Check In") << "\n";
    cout << mid() << "\n\n";

    cout << "  Enter card number     : ";
    cin >> card;
    cout << "  Enter computer number : ";
    cin >> computer;

    saveUser(UserInfo(card, computer, getTime()));

    cout << "\n" << top() << "\n";
    cout << row("Check In Successful!") << "\n";
    cout << mid() << "\n";
    cout << row("Computer    : " + computer) << "\n";
    cout << row("Start Time  : " + getTime()) << "\n";
    cout << top() << "\n";
}

void checkOut() {
    system("cls");
    string card;
    cout << top() << "\n";
    cout << row("Check Out") << "\n";
    cout << mid() << "\n\n";

    cout << "  Enter card number to check out: ";
    cin >> card;

    auto users = loadUsers();
    UserInfo targetUser;
    bool found = false;

    for (const auto& u : users) {
        if (u.getCard() == card) {
            targetUser = u;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "\n" << top() << "\n";
        cout << row("Card Not Found!") << "\n";
        cout << top() << "\n";
        return;
    }

    double fee = targetUser.calculateFee();
    string feeStr = to_string(fee);
    feeStr = feeStr.substr(0, feeStr.find('.') + 3) + " Yuan";

    cout << "\n" << top() << "\n";
    cout << row("Receipt") << "\n";
    cout << mid() << "\n";
    cout << row("Card Number : " + targetUser.getCard()) << "\n";
    cout << row("Computer    : " + targetUser.getComputer()) << "\n";
    cout << row("Start Time  : " + targetUser.getTime()) << "\n";
    cout << row("End Time    : " + getTime()) << "\n";
    cout << mid() << "\n";
    cout << row("Total Fee   : " + feeStr) << "\n";
    cout << top() << "\n";

    vector<UserInfo> remaining;
    for (const auto& u : users)
        if (u.getCard() != card) remaining.push_back(u);
    saveUsers(remaining);

    cout << "\n" << top() << "\n";
    cout << row("Check Out Successful!") << "\n";
    cout << top() << "\n";
}

void showUsers() {
    system("cls");
    auto users = loadUsers();

    cout << top() << "\n";
    cout << row("Online Users") << "\n";
    cout << top() << "\n\n";

    if (users.empty()) {
        cout << top() << "\n";
        cout << row("No Online Users") << "\n";
        cout << top() << "\n";
        return;
    }

    // table total width = W+2; columns: C1+C2+C3 = W-8
    const int C1 = 12, C2 = 8, C3 = W - 8 - C1 - C2;
    string sep = mid();

    cout << sep << "\n";
    cout << "| " << pad("Card Number", C1) << " | "
         << pad("Computer", C2) << " | "
         << pad("Start Time", C3) << " |\n";
    cout << sep << "\n";

    for (const auto& u : users) {
        cout << "| " << pad(u.getCard(), C1) << " | "
             << pad(u.getComputer(), C2) << " | "
             << pad(u.getTime(), C3) << " |\n";
    }

    cout << sep << "\n";
    cout << row("Total Users: " + to_string(users.size()) + " users") << "\n";
    cout << sep << "\n";
}

void showMenu() {
    cout << top() << "\n";
    cout << row("Internet Cafe Billing System") << "\n";
    cout << mid() << "\n";
    cout << row("1. Check In") << "\n";
    cout << row("2. Check Out") << "\n";
    cout << row("3. View Users") << "\n";
    cout << row("0. Exit") << "\n";
    cout << mid() << "\n";
    cout << "  Please choose: ";
}

int main() {
    int choice;
    while (true) {
        system("cls");
        showMenu();
        cin >> choice;

        switch (choice) {
        case 1: checkIn(); break;
        case 2: checkOut(); break;
        case 3: showUsers(); break;
        case 0: {
            system("cls");
            auto users = loadUsers();
            if (!users.empty()) {
                double total = 0;
                cout << top() << "\n";
                cout << row("Exit Settlement") << "\n";
                cout << mid() << "\n";
                for (const auto& u : users) {
                    double fee = u.calculateFee();
                    total += fee;
                    cout << row("Card: " + u.getCard() + "  Fee: "
                        + to_string(fee).substr(0, to_string(fee).find('.') + 3) + " Yuan") << "\n";
                }
                cout << mid() << "\n";
                string ts = to_string(total);
                cout << row("Total Revenue: " + ts.substr(0, ts.find('.') + 3) + " Yuan") << "\n";
                cout << top() << "\n";
                ofstream(DATA_FILE);
            }
            cout << "\n  Goodbye!\n";
            return 0;
        }
        default: cout << "  Invalid choice!\n";
        }
        cout << "\n  Press any key to continue...";
        system("pause");
    }
}