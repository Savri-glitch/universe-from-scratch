#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

// ==================== Uniform Motion ====================
void uniformMotion() {
    double x0, v, t, xf;
    char choice;
    int sim_count = 0;
    double last_final_position = 0;

    ofstream logfile("UniformMotionLog.txt", ios::app);
    if (!logfile) { cerr << "Error opening log file!\n"; return; }

    // Write header if file empty
    logfile.seekp(0, ios::end);
    if (logfile.tellp() == 0) {
        logfile << left << setw(12) << "Simulation"
                << setw(20) << "Initial Position(m)"
                << setw(15) << "Velocity(m/s)"
                << setw(10) << "Time(s)"
                << setw(20) << "Final Position(m)" << endl;
        logfile << string(77, '-') << endl;
    }

    do {
        cout << "Enter initial position (m): "; cin >> x0;
        cout << "Enter velocity (m/s): "; cin >> v;
        cout << "Enter time (s): "; cin >> t;

        xf = x0 + v*t;
        last_final_position = xf;
        sim_count++;

        cout << "At time t = " << t << " s, the object moved from x0 = "
             << x0 << " m to x = " << xf << " m" << endl;

        logfile << left << setw(12) << sim_count
                << setw(20) << x0
                << setw(15) << v
                << setw(10) << t
                << setw(20) << xf << endl;

        cout << "Calculate again? (y/n): "; cin >> choice;
        cout << endl;
    } while(choice == 'y' || choice == 'Y');

    // Summary after loop
    cout << "==================== Summary ====================" << endl;
    cout << "Total simulations run: " << sim_count << endl;
    if(sim_count > 0) cout << "Last final position: " << last_final_position << " m" << endl;

    logfile.close();
}

// ==================== Uniform Acceleration ====================
void uniformAcceleration() {
    double x0, v0, a, t, xf;
    char choice;
    int sim_count = 0;

    ofstream logfile("UniformAccelerationLog.txt", ios::app);
    if (!logfile) { cerr << "Error opening log file!\n"; return; }

    logfile.seekp(0, ios::end);
    if (logfile.tellp() == 0) {
        logfile << left << setw(12) << "Simulation"
                << setw(20) << "Initial Position(m)"
                << setw(20) << "Initial Velocity(m/s)"
                << setw(20) << "Acceleration(m/s^2)"
                << setw(10) << "Time(s)"
                << setw(20) << "Final Position(m)" << endl;
        logfile << string(102, '-') << endl;
    }

    do {
        cout << "Enter initial position (m): "; cin >> x0;
        cout << "Enter initial velocity (m/s): "; cin >> v0;
        cout << "Enter acceleration (m/s^2): "; cin >> a;
        cout << "Enter time (s): "; cin >> t;

        xf = x0 + v0*t + 0.5*a*t*t;
        sim_count++;

        cout << "At time t = " << t << " s, the object moved from x0 = "
             << x0 << " m with v0 = " << v0 << " m/s and acceleration a = "
             << a << " m/s^2 to x = " << xf << " m" << endl;

        logfile << left << setw(12) << sim_count
                << setw(20) << x0
                << setw(20) << v0
                << setw(20) << a
                << setw(10) << t
                << setw(20) << xf << endl;

        cout << "Calculate again? (y/n): "; cin >> choice;
        cout << endl;
    } while(choice == 'y' || choice == 'Y');

    logfile.close();
}

// ==================== Free Fall ====================
void freeFall() {
    double y0, t, yf;
    const double g = 9.8;
    char choice;
    int sim_count = 0;

    ofstream logfile("FreeFallLog.txt", ios::app);
    if (!logfile) { cerr << "Error opening log file!\n"; return; }

    logfile.seekp(0, ios::end);
    if (logfile.tellp() == 0) {
        logfile << left << setw(12) << "Simulation"
                << setw(20) << "Initial Height(m)"
                << setw(10) << "Time(s)"
                << setw(20) << "Final Height(m)" << endl;
        logfile << string(62, '-') << endl;
    }

    do {
        cout << "Enter initial height (m): "; cin >> y0;
        cout << "Enter time (s): "; cin >> t;

        yf = y0 - 0.5*g*t*t;
        if(yf < 0) yf = 0;
        sim_count++;

        cout << "At time t = " << t << " s, object fell from y0 = " 
             << y0 << " m to y = " << yf << " m" << endl;

        logfile << left << setw(12) << sim_count
                << setw(20) << y0
                << setw(10) << t
                << setw(20) << yf << endl;

        cout << "Calculate again? (y/n): "; cin >> choice;
        cout << endl;
    } while(choice == 'y' || choice == 'Y');

    logfile.close();
}

// ==================== Main Menu ====================
int main() {
    int option;
    do {
        cout << "======= Physics Simulator =======\n";
        cout << "1. Uniform Motion\n";
        cout << "2. Uniform Acceleration\n";
        cout << "3. Free Fall\n";
        cout << "4. Exit\n";
        cout << "Choose simulation: ";
        cin >> option;
        cout << endl;

        switch(option) {
            case 1: uniformMotion(); break;
            case 2: uniformAcceleration(); break;
            case 3: freeFall(); break;
            case 4: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
        cout << endl;
    } while(option != 4);

    cout << "Thank you for using the Physics Simulator!\n";
    return 0;
}
