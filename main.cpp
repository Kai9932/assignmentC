#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cctype>

using namespace std;

const string FILE_NAME = "patients.txt";
const string APPOINTMENT_FILE = "appointments.txt";
const string DOCTOR_FILE = "doctors.txt";
const string ADMIN_FILE = "admin.txt";

bool isValidName(const string &n) {
    if (n.empty())
        return false;

    bool hasLetter = false;

    for (char c : n) {
        if (isalpha(static_cast<unsigned char>(c))) {
            hasLetter = true;
        }
        else if (c != ' ') {
            return false;
        }
    }

    return hasLetter;
}

bool isValidAge(const string &input) {
    if (input.empty())
        return false;

    for (char c : input) {
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    }

    if (input.size() > 3)
        return false;

    int age = stoi(input);

    if (age < 1 || age > 100)
        return false;

    return true;
}

bool isValidUsername(const string &username) {
    if (username.empty())
        return false;

    bool hasLetter = false;
    bool hasNumber = false;

    for (char c : username) {
        if (isalpha(static_cast<unsigned char>(c)))
            hasLetter = true;
        else if (isdigit(static_cast<unsigned char>(c)))
            hasNumber = true;
        else
            return false;
    }

    return hasLetter && hasNumber;
}

class Appointment {
private:
    string appointmentID;
    string date;
    string time;
    string doctorID;
    string status;

public:
    Appointment() {
        appointmentID = "";
        date = "";
        time = "";
        doctorID = "";
        status = "Pending";
    }

    Appointment(string appointmentID, string date, string time,
                 string doctorID, string status = "Pending") {
        this->appointmentID = appointmentID;
        this->date = date;
        this->time = time;
        this->doctorID = doctorID;
        this->status = status;
    }

    string getAppointmentID() const { return appointmentID; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    string getDoctorID() const { return doctorID; }
    string getStatus() const { return status; }

    bool setDetails(string date, string time, string doctorID) {

        // Validate date format: YYYY-MM-DD
        if (date.length() != 10 ||
            date[4] != '-' ||
            date[7] != '-')
            return false;

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7)
                continue;

            if (!isdigit(static_cast<unsigned char>(date[i])))
                return false;
        }

        // Validate time format: HH:MM
        if (time.length() != 5 || time[2] != ':')
            return false;

        for (int i = 0; i < 5; i++) {
            if (i == 2)
                continue;

            if (!isdigit(static_cast<unsigned char>(time[i])))
                return false;
        }

        // Validate hour and minute ranges
        int hour = stoi(time.substr(0, 2));
        int minute = stoi(time.substr(3, 2));

        if (hour < 0 || hour > 23 ||
            minute < 0 || minute > 59)
            return false;

        if (doctorID.empty())
            return false;

        this->date = date;
        this->time = time;
        this->doctorID = doctorID;

        return true;
    }

    void setStatus(string status) {
        this->status = status;
    }

    void confirm() {
        status = "Confirmed";
    }

    void cancel() {
        status = "Cancelled";
    }

    void display(string doctorName = "Unknown Doctor") const {
        cout << "\n========= APPOINTMENT INFORMATION =========\n";
        cout << "Appointment ID : " << appointmentID << '\n';
        cout << "Doctor Name    : " << doctorName << '\n';
        cout << "Date           : " << date << '\n';
        cout << "Time           : " << time << '\n';
        cout << "Status         : " << status << '\n';
        cout << "============================================\n";
    }
};

class Person {
private:
    string id;
    string name;

protected:
    bool isValidName(string n) const {
        if (n.empty())
            return false;

        for (char c : n) {
            if (!isalpha(static_cast<unsigned char>(c)) && c != ' ')
                return false;
        }

        return true;
    }


public:
    Person() {
        id = "";
        name = "";
    }

    Person(string id, string name) {
        this->id = id;
        this->name = name;
    }

    bool setID(string id) {
        if (id.length() != 4)
            return false;

        if (!isupper(static_cast<unsigned char>(id[0])))
            return false;

        for (int i = 1; i < 4; i++) {
            if (!isdigit(static_cast<unsigned char>(id[i])))
                return false;
        }

        this->id = id;
        return true;
    }
    string getID() const {
        return id;
    }

    bool setName(string name) {
        if (!isValidName(name))
            return false;
        this->name = name;
        return true;
    }

    string getName() const {
        return name;
    }

    virtual void displayInfo() const = 0;

    virtual ~Person() {}
};

class Patient : public Person {
private:
    string ailment;
    int age;
    Appointment appointments[3];
    int appointmentCount;
    string username;
    string password;

public:
    Patient() : Person() {
        ailment = "";
        age = 0;
        appointmentCount = 0;
        username = "";
        password = "";
    }

    Patient(string id, string name, string ailment, int age,
            string username = "", string password = "")
        : Person(id, name) {
        this->ailment = ailment;
        this->age = age;
        this->username = username;
        this->password = password;
        appointmentCount = 0;
    }

    bool setAilment(string ailment) {
        if (ailment.empty())
            return false;
        this->ailment = ailment;
        return true;
    }

    bool setAge(int age) {
        if (age <= 0 || age > 100)
            return false;
        this->age = age;
        return true;
    }

    string getAilment() const { return ailment; }
    int getAge() const { return age; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    int getAppointmentCount() const { return appointmentCount; }

    bool setUsername(string username) {
        if (username.empty())
            return false;
        this->username = username;
        return true;
    }

    bool setPassword(string password) {
        if (password.empty())
            return false;
        this->password = password;
        return true;
    }

    bool verifyLogin(string username, string password) const {
        return this->username == username && this->password == password;
    }

    bool requestAppointment(Appointment appointment) {
        if (appointmentCount >= 3)
            return false;
        appointments[appointmentCount] = appointment;
        appointmentCount++;
        return true;
    }

    Appointment getAppointment(int index) const {
        if (index < 0 || index >= appointmentCount)
            return Appointment();
        return appointments[index];
    }

    bool updateAppointmentStatus(string appointmentID, string newStatus) {
        for (int i = 0; i < appointmentCount; i++) {
            if (appointments[i].getAppointmentID() == appointmentID) {
                appointments[i].setStatus(newStatus);
                return true;
            }
        }
        return false;
    }

    bool removeAppointment(string appointmentID) {
        for (int i = 0; i < appointmentCount; i++) {
            if (appointments[i].getAppointmentID() == appointmentID) {
                for (int j = i; j < appointmentCount - 1; j++)
                    appointments[j] = appointments[j + 1];
                appointmentCount--;
                return true;
            }
        }
        return false;
    }

    void displayAppointments() const {
        if (appointmentCount == 0) {
            cout << "\nNo appointments found!\n";
            return;
        }

        for (int i = 0; i < appointmentCount; i++)
            appointments[i].display();
    }

    void displayInfo() const override {
        cout << "\n========= PATIENT INFORMATION =========\n";
        cout << "Patient ID     : " << getID() << '\n';
        cout << "Patient Name   : " << getName() << '\n';
        cout << "Patient Ailment: " << ailment << '\n';
        cout << "Patient Age    : " << age << '\n';
        cout << "=======================================\n";
    }
};

class Doctor : public Person {
private:
    string specialty;
    int roomNumber;
    string username;
    string password;

public:
    Doctor() : Person() {
        specialty = "";
        roomNumber = 0;
        username = "";
        password = "";
    }

    Doctor(string id, string name, string specialty, int roomNumber,
           string username = "", string password = "")
        : Person(id, name) {
        this->specialty = specialty;
        this->roomNumber = roomNumber;
        this->username = username;
        this->password = password;
    }

    string getSpecialty() const { return specialty; }
    int getRoomNumber() const { return roomNumber; }
    string getUsername() const { return username; }

    bool verifyLogin(string username, string password) const {
        return this->username == username && this->password == password;
    }

    void displayInfo() const override {
        cout << "\n========= DOCTOR INFORMATION =========\n";
        cout << "Doctor ID    : " << getID() << '\n';
        cout << "Doctor Name  : " << getName() << '\n';
        cout << "Specialty    : " << specialty << '\n';
        cout << "Room Number  : " << roomNumber << '\n';
        cout << "======================================\n";
    }
};

class Admin : public Person {
private:
    string department;
    int accessLevel;

public:
    Admin() : Person() {
        department = "";
        accessLevel = 0;
    }

    Admin(string id, string name, string department, int accessLevel)
        : Person(id, name) {
        this->department = department;
        this->accessLevel = accessLevel;
    }

    int getAccessLevel() const { return accessLevel; }

    void displayInfo() const override {
        cout << "\n========= ADMIN INFORMATION =========\n";
        cout << "Admin ID       : " << getID() << '\n';
        cout << "Admin Name     : " << getName() << '\n';
        cout << "Department     : " << department << '\n';
        cout << "Access Level   : " << accessLevel << '\n';
        cout << "=====================================\n";
    }
};

string getDoctorName(string searchID);

class ClinicRecords {
private:
    Patient patients[10];
    int patientCount;

    void loadPatients() {
        ifstream file(FILE_NAME);
        if (!file.is_open())
            return;

        string line, id, name, ailment, username, password;
        int age;

        while (patientCount < 10 && getline(file, line)) {
            istringstream record(line);
            username = "";
            password = "";

            if (record >> quoted(id) >> quoted(name) >> quoted(ailment) >> age && isValidName(name)) {
                record >> quoted(username) >> quoted(password);
                patients[patientCount] =
                    Patient(id, name, ailment, age, username, password);
                patientCount++;
            }
        }
    }

    bool savePatients() const {
        ofstream file(FILE_NAME);
        if (!file.is_open())
            return false;

        for (int i = 0; i < patientCount; i++) {
            file << quoted(patients[i].getID()) << ' '
                 << quoted(patients[i].getName()) << ' '
                 << quoted(patients[i].getAilment()) << ' '
                 << patients[i].getAge() << ' '
                 << quoted(patients[i].getUsername()) << ' '
                 << quoted(patients[i].getPassword()) << '\n';
        }
        return file.good();
    }

    void loadAppointments() {
        ifstream file(APPOINTMENT_FILE);
        if (!file.is_open())
            return;

        string line, appointmentID, patientID, doctorID, date, time, status;

        while (getline(file, line)) {
            istringstream record(line);

            if (record >> quoted(appointmentID) >> quoted(patientID)
                       >> quoted(doctorID) >> quoted(date)
                       >> quoted(time) >> quoted(status)) {
                int index = findIndex(patientID);
                if (index != -1) {
                    Appointment a(appointmentID, date, time, doctorID, status);
                    patients[index].requestAppointment(a);
                }
            }
        }
    }

    bool saveAppointments() const {
        ofstream file(APPOINTMENT_FILE);
        if (!file.is_open())
            return false;

        for (int i = 0; i < patientCount; i++) {
            for (int j = 0; j < patients[i].getAppointmentCount(); j++) {
                Appointment a = patients[i].getAppointment(j);
                file << quoted(a.getAppointmentID()) << ' '
                     << quoted(patients[i].getID()) << ' '
                     << quoted(a.getDoctorID()) << ' '
                     << quoted(a.getDate()) << ' '
                     << quoted(a.getTime()) << ' '
                     << quoted(a.getStatus()) << '\n';
            }
        }
        return file.good();
    }

public:
    ClinicRecords() {
        patientCount = 0;
        loadPatients();
        loadAppointments();
    }

    int findIndex(string id) const {
        for (int i = 0; i < patientCount; i++)
            if (patients[i].getID() == id)
                return i;
        return -1;
    }

    bool idExists(string id) const {
        return findIndex(id) != -1;
    }

    bool verifyPatient(string id, string name) const {
        int index = findIndex(id);
        return index != -1 && patients[index].getName() == name;
    }

    bool usernameExists(string username) const {
        for (int i = 0; i < patientCount; i++)
            if (!username.empty() && patients[i].getUsername() == username)
                return true;
        return false;
    }

    bool patientLogin(string username, string password,
                      Patient &loggedInPatient) const {
        for (int i = 0; i < patientCount; i++) {
            if (patients[i].verifyLogin(username, password)) {
                loggedInPatient = patients[i];
                return true;
            }
        }
        return false;
    }

    string generatePatientID() const {
        int highest = 0;
        for (int i = 0; i < patientCount; i++) {
            string current = patients[i].getID();
            if (current.size() > 1 && current[0] == 'P') {
                try {
                    int number = stoi(current.substr(1));
                    if (number > highest)
                        highest = number;
                } catch (...) {}
            }
        }

        int number = highest + 1;
        ostringstream result;
        result << 'P' << setw(3) << setfill('0') << number;
        return result.str();
    }

    string generateAppointmentID() const {
        int highest = 0;
        for (int i = 0; i < patientCount; i++) {
            for (int j = 0; j < patients[i].getAppointmentCount(); j++) {
                string current = patients[i].getAppointment(j).getAppointmentID();
                if (current.size() > 2 && current.substr(0, 2) == "AP") {
                    try {
                        int number = stoi(current.substr(2));
                        if (number > highest)
                            highest = number;
                    } catch (...) {}
                }
            }
        }

        int number = highest + 1;
        ostringstream result;
        result << "AP" << setw(3) << setfill('0') << number;
        return result.str();
    }

    bool addPatient(Patient patient) {

        string id = patient.getID();

        if (id.length() != 4 || id[0] != 'P')
            return false;

        for (int i = 1; i < 4; i++) {
            if (!isdigit(static_cast<unsigned char>(id[i])))
                return false;
        }

        if (patientCount >= 10 ||
            idExists(id) ||
            !isValidName(patient.getName()))
            return false;

        patients[patientCount] = patient;
        patientCount++;

        if (!savePatients()) {
            patientCount--;
            return false;
        }

        return true;
    }

    bool updatePatient(string id, string name, string ailment,
                       int age, int choice) {
        int index = findIndex(id);
        if (index == -1 || patients[index].getName() != name)
            return false;

        Patient backup = patients[index];

        if (choice == 1) {
            if (!patients[index].setAilment(ailment))
                return false;
        } else if (choice == 2) {
            if (!patients[index].setAge(age))
                return false;
        } else {
            return false;
        }

        if (!savePatients()) {
            patients[index] = backup;
            return false;
        }
        return true;
    }

    bool deletePatient(string id) {
        int index = findIndex(id);
        if (index == -1)
            return false;

        Patient backup[10];
        int oldCount = patientCount;
        for (int i = 0; i < oldCount; i++)
            backup[i] = patients[i];

        for (int i = index; i < patientCount - 1; i++)
            patients[i] = patients[i + 1];
        patientCount--;

        if (!savePatients() || !saveAppointments()) {
            patientCount = oldCount;
            for (int i = 0; i < oldCount; i++)
                patients[i] = backup[i];
            return false;
        }
        return true;
    }

    bool isDoctorAvailable(string doctorID, string date, string time) const {
        for (int i = 0; i < patientCount; i++) {
            for (int j = 0; j < patients[i].getAppointmentCount(); j++) {
                Appointment a = patients[i].getAppointment(j);
                if (a.getDoctorID() == doctorID &&
                    a.getDate() == date &&
                    a.getTime() == time &&
                    a.getStatus() != "Cancelled" &&
                    a.getStatus() != "Rejected")
                    return false;
            }
        }
        return true;
    }

    bool scheduleAppointment(string patientID, Appointment appointment) {
        int index = findIndex(patientID);
        if (index == -1 || patients[index].getAppointmentCount() >= 3)
            return false;

        if (!isDoctorAvailable(appointment.getDoctorID(),
                               appointment.getDate(), appointment.getTime()))
            return false;

        if (!patients[index].requestAppointment(appointment))
            return false;

        if (!saveAppointments()) {
            patients[index].removeAppointment(appointment.getAppointmentID());
            return false;
        }
        return true;
    }

    bool cancelAppointment(string appointmentID, string patientID) {
        int index = findIndex(patientID);
        if (index == -1)
            return false;

        for (int i = 0; i < patients[index].getAppointmentCount(); i++) {
            Appointment a = patients[index].getAppointment(i);
            if (a.getAppointmentID() == appointmentID &&
                a.getStatus() != "Cancelled") {
                patients[index].updateAppointmentStatus(appointmentID, "Cancelled");
                if (!saveAppointments()) {
                    patients[index].updateAppointmentStatus(appointmentID, a.getStatus());
                    return false;
                }
                return true;
            }
        }
        return false;
    }

    bool updateAppointmentStatus(string appointmentID, string doctorID,
                                 string newStatus) {
        for (int i = 0; i < patientCount; i++) {
            for (int j = 0; j < patients[i].getAppointmentCount(); j++) {
                Appointment a = patients[i].getAppointment(j);
                if (a.getAppointmentID() == appointmentID &&
                    a.getDoctorID() == doctorID &&
                    a.getStatus() == "Pending" &&
                    (newStatus == "Confirmed" || newStatus == "Rejected")) {
                    patients[i].updateAppointmentStatus(appointmentID, newStatus);
                    if (!saveAppointments()) {
                        patients[i].updateAppointmentStatus(appointmentID, "Pending");
                        return false;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    void displayPatientAppointments(string patientID) const {
        int index = findIndex(patientID);
        if (index == -1 || patients[index].getAppointmentCount() == 0) {
            cout << "\nNo appointments found!\n";
            return;
        }

        for (int i = 0; i < patients[index].getAppointmentCount(); i++) {
            Appointment a = patients[index].getAppointment(i);
            a.display(getDoctorName(a.getDoctorID()));
        }
    }

    void displayDoctorAppointments(string doctorID, string doctorName) const {
        bool found = false;
        cout << "\n========= MY APPOINTMENTS =========\n";

        for (int i = 0; i < patientCount; i++) {
            for (int j = 0; j < patients[i].getAppointmentCount(); j++) {
                Appointment a = patients[i].getAppointment(j);
                if (a.getDoctorID() == doctorID) {
                    cout << "Patient Name   : " << patients[i].getName() << '\n';
                    cout << "Patient ID     : " << patients[i].getID() << '\n';
                    a.display(doctorName);
                    found = true;
                }
            }
        }

        if (!found)
            cout << "\nNo appointments assigned to you.\n";
    }

    void displayAll() const {
        if (patientCount == 0) {
            cout << "\nNo patient records found!\n";
            return;
        }

        for (int i = 0; i < patientCount; i++)
            patients[i].displayInfo();

        cout << "\nTotal Patients: " << patientCount << '\n';
    }

    void displayPatient(string id) const {
        int index = findIndex(id);
        if (index == -1) {
            cout << "\nPatient not found!\n";
            return;
        }
        patients[index].displayInfo();
    }
};

bool readDoctorRecord(ifstream &file, string &id, string &name,
                      string &specialty, int &room,
                      string &username, string &password) {
    return static_cast<bool>(file >> quoted(id) >> quoted(name)
                                 >> quoted(specialty) >> room
                                 >> quoted(username) >> quoted(password));
}

bool doctorExists(string searchID) {
    ifstream file(DOCTOR_FILE);
    if (!file.is_open())
        return false;

    string id, name, specialty, username, password;
    int room;

    while (readDoctorRecord(file, id, name, specialty, room, username, password)) {
        if (id == searchID)
            return true;
    }
    return false;
}

string getDoctorName(string searchID) {
    ifstream file(DOCTOR_FILE);
    if (!file.is_open())
        return "Unknown Doctor";

    string id, name, specialty, username, password;
    int room;

    while (readDoctorRecord(file, id, name, specialty, room, username, password)) {
        if (id == searchID)
            return name;
    }
    return "Unknown Doctor";
}

bool displayDoctors(string date, string time, ClinicRecords &clinic) {
    ifstream file(DOCTOR_FILE);
    if (!file.is_open()) {
        cout << "\nError: Cannot open doctors.txt!\n";
        return false;
    }

    string id, name, specialty, username, password;
    int room;
    bool found = false;

    cout << "\n============== AVAILABLE DOCTORS ==============\n";

    while (readDoctorRecord(file, id, name, specialty, room, username, password)) {
        if (clinic.isDoctorAvailable(id, date, time)) {
            cout << "\nDoctor ID   : " << id << '\n';
            cout << "Doctor Name : " << name << '\n';
            cout << "Specialty   : " << specialty << '\n';
            cout << "Room Number : " << room << '\n';
            found = true;
        }
    }

    if (!found)
        cout << "\nNo doctors are available at this date and time.\n";

    cout << "================================================\n";
    return found;
}

void bookAppointment(Patient &patient, ClinicRecords &clinic) {
    string doctorID, date, time, confirmation;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "          BOOK APPOINTMENT\n";
    cout << "========================================\n";

    cout << "\nEnter Appointment Date (YYYY-MM-DD): ";
    getline(cin, date);
    if (date.empty()) {
        cout << "\nAppointment date cannot be empty!\n";
        return;
    }

    cout << "Enter Appointment Time (HH:MM): ";
    getline(cin, time);
    
    if (!displayDoctors(date, time, clinic)) {
        cout << "\nPlease choose another date or time.\n";
        return;
    }

    cout << "\nEnter Doctor ID: ";
    getline(cin, doctorID);

    if (!doctorExists(doctorID)) {
        cout << "\nInvalid Doctor ID!\n";
        return;
    }

    Appointment validation;

    if (!validation.setDetails(date, time, doctorID)) {
        cout << "\nDate must look like 2026-11-20 and time like 10:00.\n";
        return;
    }

    if (!clinic.isDoctorAvailable(doctorID, date, time)) {
        cout << "\nSorry! This doctor is already booked at that time.\n";
        return;
    }

    cout << "\n========== BOOKING SUMMARY ==========\n";
    cout << "Patient Name : " << patient.getName() << '\n';
    cout << "Patient ID   : " << patient.getID() << '\n';
    cout << "Doctor Name  : " << getDoctorName(doctorID) << '\n';
    cout << "Date         : " << date << '\n';
    cout << "Time         : " << time << '\n';
    cout << "=====================================\n";

    cout << "\nConfirm appointment? (Y/N): ";
    getline(cin, confirmation);

    if (confirmation != "Y" && confirmation != "y") {
        cout << "\nBooking cancelled!\n";
        return;
    }

    string appointmentID = clinic.generateAppointmentID();
    Appointment appointment(appointmentID, date, time, doctorID, "Pending");

    if (clinic.scheduleAppointment(patient.getID(), appointment)) {
        cout << "\n========================================\n";
        cout << "       APPOINTMENT BOOKED!\n";
        cout << "========================================\n";
        cout << "Appointment ID : " << appointmentID << '\n';
        cout << "Doctor Name    : " << getDoctorName(doctorID) << '\n';
        cout << "Date           : " << date << '\n';
        cout << "Time           : " << time << '\n';
        cout << "Status         : Pending\n";
    } else {
        cout << "\nFailed to book appointment. Check your appointment limit or availability.\n";
    }
}

bool adminLogin(Admin &loggedInAdmin) {
    string username, password, savedUsername, savedPassword;
    string adminID, adminName, department;
    int accessLevel;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "             ADMIN LOGIN\n";
    cout << "========================================\n";

    cout << "\nUsername: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);

    ifstream file(ADMIN_FILE);
    if (!file.is_open()) {
        cout << "Error: Cannot open admin.txt!\n";
        return false;
    }

    while (file >> quoted(savedUsername) >> quoted(savedPassword)
                >> quoted(adminID) >> quoted(adminName)
                >> quoted(department) >> accessLevel) {
        if (username == savedUsername && password == savedPassword) {
            loggedInAdmin = Admin(adminID, adminName, department, accessLevel);
            cout << "\nLogin successful!\n";
            return true;
        }
    }

    cout << "\nInvalid username or password!\n";
    return false;
}

void adminDashboard(Admin &admin, ClinicRecords &clinic) {
    int choice;
    int level = admin.getAccessLevel();

    while (true) {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "            ADMIN PORTAL\n";
        cout << "========================================\n";
        admin.displayInfo();

        cout << "\n1. Display Patient Information\n";
        if (level >= 2) {
            cout << "2. Register New Patient\n";
            cout << "3. Update Patient Information\n";
        }
        if (level >= 3)
            cout << "4. Delete Patient Records\n";
        cout << "0. Logout\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) {
            cout << "\nAdmin logged out successfully!\n";
            break;
        }

        switch (choice) {
            case 1:
                clinic.displayAll();
                break;


        case 2:
            if (level >= 2) {
                string name, ailment, username, password;
                int age;

                cout << "\n---------- PATIENT REGISTRATION ----------\n";

                do {
                    cout << "Enter Patient Name: ";
                    getline(cin, name);

                    if (!isValidName(name))
                        cout << "Invalid name! Please enter letters and spaces only.\n";

                } while (!isValidName(name));

                do {
                    cout << "Enter Patient Ailment: ";
                    getline(cin, ailment);

                    if (ailment.empty())
                        cout << "Ailment cannot be empty!\n";

                } while (ailment.empty());
                
                string ageInput;
                while (true) {
                    cout << "Enter Patient Age: ";
                    getline(cin, ageInput);

                    if (isValidAge(ageInput)) {
                        age = stoi(ageInput);
                        break;
                    }
                    cout << "Invalid age! Please enter a number between 1 and 100.\n";
                };

                while (true) {
                    cout << "Create Patient Username: ";
                    getline(cin, username);

                    if (!isValidUsername(username)) {
                        cout << "Invalid format! Please use letters and numbers!\n";
                    }
                    else if (clinic.usernameExists(username)) {
                        cout << "Username already exists! Try another username.\n";
                    }
                    else {
                        break;
                    }
                }

                do {
                    cout << "Create Patient Password: ";
                    getline(cin, password);

                    if (password.empty())
                        cout << "Password cannot be empty!\n";

                } while (password.empty());

                string patientID = clinic.generatePatientID();

                Patient newPatient;

                while (!newPatient.setID(patientID)) {
                    patientID = clinic.generatePatientID();
                }

                while (!newPatient.setName(name)) {
                    cout << "Invalid name! Please enter letters and spaces only.\n";
                    cout << "Enter Patient Name: ";
                    getline(cin, name);
                }

                while (!newPatient.setAilment(ailment)) {
                    cout << "Ailment cannot be empty!\n";
                    cout << "Enter Patient Ailment: ";
                    getline(cin, ailment);
                }

                while (!newPatient.setAge(age)) {
                    cout << "Invalid age! Please enter a number between 1 and 100.\n";
                    cout << "Enter Patient Age: ";
                    cin >> age;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

                newPatient.setUsername(username);
                newPatient.setPassword(password);
                if (clinic.addPatient(newPatient)) {
                    cout << "\n========================================\n";
                    cout << "       PATIENT REGISTERED!\n";
                    cout << "========================================\n";
                    cout << "Patient ID : " << patientID << '\n';
                    cout << "Name       : " << name << '\n';
                    cout << "Username   : " << username << '\n';
                    cout << "Password   : Account credentials created\n";
                    cout << "\nPatient can now log in using these credentials.\n";
                }
                else {
                    cout << "\nRegistration failed!\n";
                }

            } else {
                cout << "Access denied!\n";
            }
            break;


            case 3:
                if (level >= 2) {
                    string patientID, patientName, ailment;
                    int age = 0, updateChoice;

                    cout << "\n---------- UPDATE PATIENT INFORMATION ----------\n";
                    cout << "Enter Patient ID: ";
                    getline(cin, patientID);
                    cout << "Enter Patient Name: ";
                    getline(cin, patientName);

                    if (!clinic.verifyPatient(patientID, patientName)) {
                        cout << "\nInvalid Patient ID or Patient Name!\n";
                        break;
                    }

                    cout << "\nPatient verified successfully!\n";
                    clinic.displayPatient(patientID);
                    cout << "\n========== SELECT FIELD TO UPDATE ==========\n";
                    cout << "1. Update Patient Ailment\n";
                    cout << "2. Update Patient Age\n";
                    cout << "0. Cancel\n";
                    cout << "Enter your choice: ";

                    if (!(cin >> updateChoice)) {
                        cout << "Invalid input!\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (updateChoice == 1) {
                        do {
                            cout << "\nEnter New Patient Ailment: ";
                            getline(cin, ailment);
                            if (ailment.empty())
                                cout << "Ailment cannot be empty!\n";
                        } while (ailment.empty());

                    } else if (updateChoice == 2) {
                        string ageInput;

                        while (true) {
                            cout << "\nEnter New Patient Age: ";
                            getline(cin, ageInput);

                            if (isValidAge(ageInput)) {
                                age = stoi(ageInput);
                                break;
                            }

                            cout << "Invalid age! Please enter a number between 1 and 100.\n";
                        }

                    } else if (updateChoice == 0) {
                        cout << "\nUpdate cancelled!\n";
                        break;
                    } else {
                        cout << "\nInvalid choice!\n";
                        break;
                    }

                    if (clinic.updatePatient(patientID, patientName, ailment, age, updateChoice)) {
                        cout << "\nPatient information updated successfully!\n";
                        clinic.displayPatient(patientID);
                    } else {
                        cout << "\nFailed to update patient information!\n";
                    }
                } else {
                    cout << "Access denied!\n";
                }
                break;

            case 4:
                if (level >= 3) {
                    string patientID;
                    cout << "\n---------- DELETE PATIENT RECORD ----------\n";
                    cout << "Enter Patient ID: ";
                    getline(cin, patientID);

                    if (clinic.deletePatient(patientID))
                        cout << "\nPatient record deleted successfully!\n";
                    else
                        cout << "\nPatient ID not found or deletion failed!\n";
                } else {
                    cout << "Access denied!\n";
                }
                break;

            default:
                cout << "\nInvalid choice or unavailable function!\n";
                break;
        }
    }
}

bool patientLogin(ClinicRecords &clinic, Patient &loggedInPatient) {
    string username, password;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "           PATIENT LOGIN\n";
    cout << "========================================\n";
    cout << "\nUsername: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);

    if (clinic.patientLogin(username, password, loggedInPatient)) {
        cout << "\nLogin successful!\n";
        cout << "Welcome, " << loggedInPatient.getName() << "!\n";
        return true;
    }

    cout << "\nInvalid username or password!\n";
    return false;
}

void loggedInPatientDashboard(Patient &patient, ClinicRecords &clinic) {
    int choice;

    while (true) {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "          PATIENT DASHBOARD\n";
        cout << "========================================\n";
        cout << "\nWelcome, " << patient.getName() << "!\n";
        cout << "Patient ID: " << patient.getID() << '\n';
        cout << "\n1. View My Information\n";
        cout << "2. Book Appointment\n";
        cout << "3. View My Appointments\n";
        cout << "4. Cancel Appointment\n";
        cout << "0. Logout\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                patient.displayInfo();
                break;
            case 2:
                bookAppointment(patient, clinic);
                break;
            case 3:
                clinic.displayPatientAppointments(patient.getID());
                break;
            case 4: {
                string appointmentID;
                cout << "\nEnter Appointment ID to cancel: ";
                getline(cin, appointmentID);

                if (clinic.cancelAppointment(appointmentID, patient.getID()))
                    cout << "\nAppointment cancelled successfully!\n";
                else
                    cout << "\nAppointment not found or cannot be cancelled!\n";
                break;
            }
            case 0:
                cout << "\nYou have successfully logged out!\n";
                return;
            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
    }
}

void patientSignUp(ClinicRecords &clinic) {
    string name, ailment, username, password;
    int age;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "          PATIENT SIGN UP\n";
    cout << "========================================\n";

    do {
        cout << "\nEnter Full Name: ";
        getline(cin, name);

        if (!isValidName(name))
            cout << "Invalid name! Please enter letters and spaces only.\n";

    } while (!isValidName(name));
    string ageInput;

    while (true) {
        cout << "Enter Age: ";
        getline(cin, ageInput);

        if (isValidAge(ageInput)) {
            age = stoi(ageInput);
            break;
        }

        cout << "Invalid age! Please enter a number between 1 and 100.\n";
    }

    do {
        cout << "Enter Ailment: ";
        getline(cin, ailment);
        if (ailment.empty())
            cout << "Ailment cannot be empty!\n";
    } while (ailment.empty());

    while (true) {
        cout << "Create Username: ";
        getline(cin, username);

        if (!isValidUsername(username))
            cout << "Invalid format! Please use letters and numbers\n";
        else if (clinic.usernameExists(username))
            cout << "Username already exists! Try another username.\n";
        else
            break;
    }

    do {
        cout << "Create Password: ";
        getline(cin, password);
        if (password.empty())
            cout << "Password cannot be empty!\n";
    } while (password.empty());

    string patientID = clinic.generatePatientID();
    Patient newPatient(patientID, name, ailment, age, username, password);

    if (clinic.addPatient(newPatient)) {
        cout << "\n========================================\n";
        cout << "       REGISTRATION SUCCESSFUL!\n";
        cout << "========================================\n";
        cout << "Patient ID : " << patientID << '\n';
        cout << "Name       : " << name << '\n';
        cout << "Username   : " << username << '\n';
        cout << "\nYou can now log in to your patient account.\n";
    } else {
        cout << "\nRegistration failed! Please try again.\n";
    }
}

void patientDashboard(ClinicRecords &clinic) {
    int choice;

    while (true) {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "           PATIENT PORTAL\n";
        cout << "========================================\n";
        cout << "\n1. Sign Up\n";
        cout << "2. Login\n";
        cout << "0. Back to Main Menu\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                patientSignUp(clinic);
                break;
            case 2: {
                Patient loggedInPatient;
                if (patientLogin(clinic, loggedInPatient))
                    loggedInPatientDashboard(loggedInPatient, clinic);
                break;
            }
            case 0:
                cout << "\nReturning to main menu...\n";
                return;
            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
    }
}

bool doctorLogin(Doctor &loggedInDoctor) {
    string username, password;
    string doctorID, doctorName, specialty, savedUsername, savedPassword;
    int room;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "            DOCTOR LOGIN\n";
    cout << "========================================\n";
    cout << "\nUsername: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);

    ifstream file(DOCTOR_FILE);
    if (!file.is_open()) {
        cout << "Error: Cannot open doctors.txt!\n";
        return false;
    }

    while (readDoctorRecord(file, doctorID, doctorName, specialty,
                            room, savedUsername, savedPassword)) {
        if (username == savedUsername && password == savedPassword) {
            loggedInDoctor = Doctor(doctorID, doctorName, specialty, room,
                                    savedUsername, savedPassword);
            cout << "\nLogin successful!\n";
            return true;
        }
    }

    cout << "\nInvalid username or password!\n";
    return false;
}

void loggedInDoctorDashboard(Doctor &doctor, ClinicRecords &clinic) {
    int choice;

    while (true) {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "          DOCTOR DASHBOARD\n";
        cout << "========================================\n";
        doctor.displayInfo();

        cout << "\n1. View My Appointments\n";
        cout << "2. Manage Appointment Requests\n";
        cout << "0. Logout\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                clinic.displayDoctorAppointments(doctor.getID(), doctor.getName());
                break;

            case 2: {
                string appointmentID;
                int action;

                cout << "\n========= MANAGE APPOINTMENT =========\n";
                clinic.displayDoctorAppointments(doctor.getID(), doctor.getName());

                cout << "\nEnter Appointment ID: ";
                getline(cin, appointmentID);
                cout << "\n1. Confirm Appointment\n";
                cout << "2. Reject Appointment\n";
                cout << "0. Back\n";
                cout << "Enter your choice: ";

                if (!(cin >> action)) {
                    cout << "\nInvalid input!\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (action == 0) {
                    cout << "\nReturning to Doctor Dashboard...\n";
                    break;
                }

                string newStatus;
                if (action == 1)
                    newStatus = "Confirmed";
                else if (action == 2)
                    newStatus = "Rejected";
                else {
                    cout << "\nInvalid choice!\n";
                    break;
                }

                if (clinic.updateAppointmentStatus(appointmentID,
                                                   doctor.getID(), newStatus))
                    cout << "\nAppointment successfully " << newStatus << "!\n";
                else
                    cout << "\nUnable to update appointment. Check the ID, assignment and status.\n";
                break;
            }

            case 0:
                cout << "\nDoctor logged out successfully!\n";
                return;

            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
    }
}

void doctorDashboard(ClinicRecords &clinic) {
    int choice;

    while (true) {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "           DOCTOR PORTAL\n";
        cout << "========================================\n";
        cout << "\n1. Login\n";
        cout << "0. Back to Main Menu\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                Doctor loggedInDoctor;
                if (doctorLogin(loggedInDoctor))
                    loggedInDoctorDashboard(loggedInDoctor, clinic);
                break;
            }
            case 0:
                cout << "\nReturning to Main Menu...\n";
                return;
            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
    }
}

int main() {
    ClinicRecords clinic;
    int choice;

    do {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "       HOSPITAL MANAGEMENT SYSTEM\n";
        cout << "========================================\n";
        cout << "\n1. Admin\n";
        cout << "2. Patient\n";
        cout << "3. Doctor\n";
        cout << "4. Exit\n";
        cout << "\nSelect User Type: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                Admin loggedInAdmin;
                if (adminLogin(loggedInAdmin))
                    adminDashboard(loggedInAdmin, clinic);
                break;
            }
            case 2:
                patientDashboard(clinic);
                break;
            case 3:
                doctorDashboard(clinic);
                break;
            case 4:
                cout << "\nThank you for using Xiamen Medical Center!\n";
                break;
            default:
                cout << "\nInvalid choice!\n";
        }
    } while (choice != 4);

    return 0;
}
