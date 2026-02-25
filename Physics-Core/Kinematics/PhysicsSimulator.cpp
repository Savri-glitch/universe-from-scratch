#include <iostream>
using namespace std;

int main() {
    double initial_position;
    double velocity;
    double time;
    double position;

    cout << "Enter initial position (m): ";
    cin >> initial_position;

    cout << "Enter velocity (m/s): ";
    cin >> velocity;

    cout << "Enter time (s): ";
    cin >> time;

    position = initial_position + velocity * time;

    cout << "Final position = " << position << " meters" << endl;

    return 0;
}
