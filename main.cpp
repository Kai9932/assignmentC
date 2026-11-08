#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;


const string FILE_NAME = "patients.txt";
const string APPOINTMENT_FILE = "appointments.txt";

class Person {
protected:
    string id;
    string name;

public:
    Person() {
        id = "";
        name = "";
    }

    Person(string id, string name) {
        this->id = id;
        this->name = name;
    }

    string getID() const {
        return id;
    }

    string getName() const {
        return name;
    }

    void displayInfo() const {
        cout << "PatientID      : " << id << endl;
        cout << "Patient Name   : " << name << endl;
    }

    bool setName(string n) {
        if (n.empty())
            return false;

        name = n;
        return true;
    }
};


class Patient : public Person {
private:
    string ailment;
    int age;
    string username;
    string password;

public:
    Patient() : Person() {
        ailment = "";
        age = 0;
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
    }

    bool setAilment(string a) {
        if (a.empty())
            return false;

        ailment = a;
        return true;
    }

    bool setAge(int a) {
        if (a <= 0 || a > 100)
            return false;

        age = a;
        return true;
    }

    bool setUsername(string u) {
        if (u.empty())
            return false;

        username = u;
        return true;
    }

    bool setPassword(string p) {
        if (p.empty())
            return false;

        password = p;
        return true;
    }

    string getAilment() const {
        return ailment;
    }

    int getAge() const {
        return age;
    }

    string getUsername() const {
        return username;
    }

    string getPassword() const {
        return password;
    }

    bool verifyLogin(string u, string p) const {
        return username == u && password == p;
    }

    void displayInfo() const {
        cout << "\n========= PATIENT INFORMATION =========\n";
        Person::displayInfo();
        cout << "Patient Ailment: " << ailment << endl;
        cout << "Patient Age    : " << age << endl;
        cout << "=======================================\n";
    }
};


class Doctor : public Person {
private:
    string department;
    string username;
    string password;

public:
    Doctor() : Person() {
        department = "";
        username = "";
        password = "";
    }

    Doctor(string id, string name, string department,
           string username, string password)
        : Person(id, name) {

        this->department = department;
        this->username = username;
        this->password = password;
    }

    string getDepartment() const {
        return department;
    }

    string getUsername() const {
        return username;
    }

    bool verifyLogin(string u, string p) const {
        return username == u && password == p;
    }

    void displayInfo() const {
        cout << "\n========= DOCTOR INFORMATION =========\n";
        cout << "Doctor ID    : " << id << endl;
        cout << "Doctor Name  : " << name << endl;
        cout << "Department   : " << department << endl;
        cout << "======================================\n";
    }
};


class Appointment {
private:
    string appointmentID;
    string patientID;
    string doctorID;
    string date;
    string time;
    string status;

public:
    Appointment() {
        appointmentID = "";
        patientID = "";
        doctorID = "";
        date = "";
        time = "";
        status = "Pending";
    }

    Appointment(string appointmentID, string patientID,
                string doctorID, string date,
                string time, string status = "Pending") {

        this->appointmentID = appointmentID;
        this->patientID = patientID;
        this->doctorID = doctorID;
        this->date = date;
        this->time = time;
        this->status = status;
    }

    string getAppointmentID() const {
        return appointmentID;
    }

    string getPatientID() const {
        return patientID;
    }

    string getDoctorID() const {
        return doctorID;
    }

    string getDate() const {
        return date;
    }

    string getTime() const {
        return time;
    }

    string getStatus() const {
        return status;
    }

    void setStatus(string s) {
        status = s;
    }

    void displayInfo(string doctorName) const {
        cout << "\n========= APPOINTMENT INFORMATION =========\n";
        cout << "Appointment ID : " << appointmentID << endl;
        cout << "Patient ID     : " << patientID << endl;
        cout << "Doctor Name    : " << doctorName << endl;
        cout << "Date           : " << date << endl;
        cout << "Time           : " << time << endl;
        cout << "Status         : " << status << endl;
        cout << "============================================\n";
    }
};


void displayDoctors() {

    ifstream file("doctors.txt");

    if (!file.is_open()) {
        cout << "Error: Cannot open doctors.txt!\n";
        return;
    }

    string doctorID, doctorName, department;

    cout << "\n============== AVAILABLE DOCTORS ==============\n";

    while (file >> quoted(doctorID)
                >> quoted(doctorName)
                >> quoted(department)) {

        cout << "\nDoctor ID   : " << doctorID << endl;
        cout << "Doctor Name : " << doctorName << endl;
        cout << "Department  : " << department << endl;
    }

    cout << "================================================\n";

    file.close();
}

bool doctorExists(string searchID) {

    ifstream file("doctors.txt");

    if (!file.is_open())
        return false;

    string doctorID, doctorName, department;

    while (file >> quoted(doctorID)
                >> quoted(doctorName)
                >> quoted(department)) {

        if (doctorID == searchID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

string getDoctorName(string searchID) {

    ifstream file("doctors.txt");

    if (!file.is_open())
        return "Unknown Doctor";

    string doctorID, doctorName, department;

    while (file >> quoted(doctorID)
                >> quoted(doctorName)
                >> quoted(department)) {

        if (doctorID == searchID) {
            file.close();
            return doctorName;
        }
    }

    file.close();
    return "Unknown Doctor";
}


class AppointmentRecords {
private:
    Appointment appointments[100];
    int appointmentCount;

    void loadAppointments() {
        ifstream file(APPOINTMENT_FILE);

        if (!file.is_open()) {
            return;
        }

        string line;
        string appointmentID, patientID, doctorID;
        string date, time, status;

        while (appointmentCount < 100 && getline(file, line)) {

            istringstream record(line);

            if (record >> quoted(appointmentID)
                       >> quoted(patientID)
                       >> quoted(doctorID)
                       >> quoted(date)
                       >> quoted(time)
                       >> quoted(status)) {

                Appointment appointment(
                    appointmentID,
                    patientID,
                    doctorID,
                    date,
                    time,
                    status
                );

                appointments[appointmentCount] = appointment;
                appointmentCount++;
            }
        }

        file.close();
    }

    bool saveAppointments() {
        ofstream file(APPOINTMENT_FILE);

        if (!file.is_open()) {
            cout << "Error: Cannot save appointment records!\n";
            return false;
        }

        for (int i = 0; i < appointmentCount; i++) {
            file << quoted(appointments[i].getAppointmentID()) << " "
                 << quoted(appointments[i].getPatientID()) << " "
                 << quoted(appointments[i].getDoctorID()) << " "
                 << quoted(appointments[i].getDate()) << " "
                 << quoted(appointments[i].getTime()) << " "
                 << quoted(appointments[i].getStatus()) << endl;
        }

        file.close();

        return !file.fail();
    }

    int findIndex(string appointmentID) const {
        for (int i = 0; i < appointmentCount; i++) {
            if (appointments[i].getAppointmentID() == appointmentID) {
                return i;
            }
        }

        return -1;
    }

public:
    AppointmentRecords() {
        appointmentCount = 0;
        loadAppointments();
    }

    string generateAppointmentID() const {
        int highestNumber = 0;

        for (int i = 0; i < appointmentCount; i++) {
            string id = appointments[i].getAppointmentID();

            if (id.length() > 2 && id.substr(0, 2) == "AP") {
                try {
                    int number = stoi(id.substr(2));

                    if (number > highestNumber) {
                        highestNumber = number;
                    }
                }
                catch (...) {
                }
            }
        }

        int number = highestNumber + 1;
        string id = "AP";

        if (number < 100)
            id += "0";

        if (number < 10)
            id += "0";

        id += to_string(number);

        return id;
    }

    bool addAppointment(Appointment appointment) {
        if (appointmentCount >= 100) {
            cout << "Appointment storage is full!\n";
            return false;
        }

        if (findIndex(appointment.getAppointmentID()) != -1) {
            cout << "Appointment ID already exists!\n";
            return false;
        }

        appointments[appointmentCount] = appointment;
        appointmentCount++;

        if (!saveAppointments()) {
            appointmentCount--;
            return false;
        }

        return true;
    }

    bool isSlotAvailable(string doctorID, string date, string time) const {

        for (int i = 0; i < appointmentCount; i++) {

            if (appointments[i].getDoctorID() == doctorID &&
                appointments[i].getDate() == date &&
                appointments[i].getTime() == time &&
                appointments[i].getStatus() != "Cancelled") {

                return false;
            }
        }

        return true;
    }


    void displayPatientAppointments(string patientID) const {

        bool found = false;

        for (int i = 0; i < appointmentCount; i++) {

            if (appointments[i].getPatientID() == patientID) {

                appointments[i].displayInfo(
                    getDoctorName(appointments[i].getDoctorID())
                );

                found = true;
            }
        }

        if (!found) {
            cout << "\nNo appointments found!\n";
        }
    }

    
    void displayDoctorAppointments(string doctorID, string doctorName) const {

        bool found = false;

        cout << "\n========= MY APPOINTMENTS =========\n";

        for (int i = 0; i < appointmentCount; i++) {

            if (appointments[i].getDoctorID() == doctorID) {

                appointments[i].displayInfo(doctorName);
                found = true;
            }
        }

        if (!found) {
            cout << "\nNo appointments assigned to you.\n";
        }
    }


    bool updateAppointmentStatus(string appointmentID,
                                string doctorID,
                                string newStatus) {

        int index = findIndex(appointmentID);

        if (index == -1)
            return false;

        if (appointments[index].getDoctorID() != doctorID)
            return false;

        if (appointments[index].getStatus() != "Pending")
            return false;

        if (newStatus != "Confirmed" && newStatus != "Rejected")
            return false;

        string oldStatus = appointments[index].getStatus();

        appointments[index].setStatus(newStatus);

        if (!saveAppointments()) {
            appointments[index].setStatus(oldStatus);
            return false;
        }

        return true;
    }



    bool cancelAppointment(string appointmentID, string patientID) {
        int index = findIndex(appointmentID);

        if (index == -1)
            return false;

        if (appointments[index].getPatientID() != patientID)
            return false;

        if (appointments[index].getStatus() == "Cancelled")
            return false;

        appointments[index].setStatus("Cancelled");

        if (!saveAppointments()) {
            appointments[index].setStatus("Pending");
            return false;
        }

        return true;
    }

};


void bookAppointment(Patient &patient, AppointmentRecords &records) {

    string doctorID, date, time;
    string confirmation;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "          BOOK APPOINTMENT\n";
    cout << "========================================\n";

    displayDoctors();

    cout << "\nEnter Doctor ID: ";
    getline(cin, doctorID);

    if (!doctorExists(doctorID)) {
        cout << "\nInvalid Doctor ID!\n";
        return;
    }

    cout << "Enter Appointment Date (YYYY-MM-DD): ";
    getline(cin, date);

    if (date.empty()) {
        cout << "\nAppointment date cannot be empty!\n";
        return;
    }

    cout << "Enter Appointment Time (HH:MM): ";
    getline(cin, time);

    if (time.empty()) {
        cout << "\nAppointment time cannot be empty!\n";
        return;
    }

    if (!records.isSlotAvailable(doctorID, date, time)) {
        cout << "\nSorry! This doctor is already booked at that time.\n";
        return;
    }

    cout << "\n========== BOOKING SUMMARY ==========\n";
    cout << "Patient Name : " << patient.getName() << endl;
    cout << "Patient ID   : " << patient.getID() << endl;
    cout << "Doctor Name  : " << getDoctorName(doctorID) << endl;
    cout << "Date         : " << date << endl;
    cout << "Time         : " << time << endl;
    cout << "=====================================\n";

    cout << "\nConfirm appointment? (Y/N): ";
    getline(cin, confirmation);

    if (confirmation != "Y" && confirmation != "y") {
        cout << "\nBooking cancelled!\n";
        return;
    }

    string appointmentID = records.generateAppointmentID();

    Appointment newAppointment(
        appointmentID,
        patient.getID(),
        doctorID,
        date,
        time,
        "Pending"
    );

    if (records.addAppointment(newAppointment)) {

        cout << "\n========================================\n";
        cout << "       APPOINTMENT BOOKED!\n";
        cout << "========================================\n";

        cout << "Appointment ID : " << appointmentID << endl;
        cout << "Doctor Name    : " << getDoctorName(doctorID) << endl;
        cout << "Date           : " << date << endl;
        cout << "Time           : " << time << endl;
        cout << "Status         : Pending\n";

    }
    else {
        cout << "\nFailed to book appointment!\n";
    }
}


class ClinicRecords {

private:
    Patient patients[10];
    int patientCount;


    void loadPatients() {
        ifstream file(FILE_NAME);

        if (!file.is_open()) {
            return;
        }

        string line;
        string id, name, ailment, username, password;
        int age;

        while (patientCount < 10 && getline(file, line)) {

            istringstream record(line);

            username = "";
            password = "";

            if (record >> quoted(id)
                    >> quoted(name)
                    >> quoted(ailment)
                    >> age) {

                record >> quoted(username) >> quoted(password);

                Patient p(id, name, ailment, age, username, password);

                patients[patientCount] = p;
                patientCount++;
            }
        }

        file.close();
    }



    bool savePatients() {
        ofstream file(FILE_NAME);

        if (!file.is_open()) {
            cout << "Error: Cannot save patient records!\n";
            return false;
        }

        for (int i = 0; i < patientCount; i++) {
            file << quoted(patients[i].getID()) << " "
                << quoted(patients[i].getName()) << " "
                << quoted(patients[i].getAilment()) << " "
                << patients[i].getAge() << " "
                << quoted(patients[i].getUsername()) << " "
                << quoted(patients[i].getPassword()) << endl;
        }

        file.close();

        return !file.fail();
    }


public:
    ClinicRecords() {
        patientCount = 0;
        loadPatients();
    }

    string generatePatientID() {
        int number = patientCount + 1;
        string id = "P";

        if (number < 100)
            id += "0";

        if (number < 10)
            id += "0";

        id += to_string(number);
        return id;
    }

    int findIndex(string id) {
        for (int i = 0; i < patientCount; i++) {
            if (patients[i].getID() == id) {
                return i;
            }
        }

        return -1;
    }
    bool verifyPatient(string id, string name) {
    int index = findIndex(id);

    if (index == -1)
        return false;

    return patients[index].getName() == name;
    }

    bool idExists(string id) {
        return findIndex(id) != -1;
    }


    bool usernameExists(string username) {
        for (int i = 0; i < patientCount; i++) {
            if (!username.empty() &&
                patients[i].getUsername() == username) {
                return true;
            }
        }

        return false;
    }


    bool patientLogin(string username, string password, Patient &loggedInPatient) {

        for (int i = 0; i < patientCount; i++) {

            if (patients[i].verifyLogin(username, password)) {
                loggedInPatient = patients[i];
                return true;
            }
        }

        return false;
    }



    bool addPatient(Patient p) {
        if (patientCount >= 10)
            return false;

        if (idExists(p.getID()))
            return false;

        ofstream file(FILE_NAME, ios::app);

        if (!file.is_open()) {
            cout << "Error: Cannot open patients.txt!\n";
            return false;
        }

        file << quoted(p.getID()) << " "
            << quoted(p.getName()) << " "
            << quoted(p.getAilment()) << " "
            << p.getAge() << " "
            << quoted(p.getUsername()) << " "
            << quoted(p.getPassword()) << endl;

        if (!file.good()) {
            file.close();
            cout << "Error saving patient information!\n";
            return false;
        }

        file.close();

        patients[patientCount] = p;
        patientCount++;

        return true;
    }

    bool updatePatient(string id, string name, string ailment, int age, int choice) {
        int index = findIndex(id);

        if (index == -1)
            return false;

        if (patients[index].getName() != name)
            return false;

        Patient oldPatient = patients[index];

        if (choice == 1) {
            if (ailment.empty())
                return false;

            patients[index].setAilment(ailment);
        }
        else if (choice == 2) {
            if (age <= 0 || age > 100)
                return false;

            patients[index].setAge(age);
        }
        else {
            return false;
        }

        if (!savePatients()) {
            patients[index] = oldPatient;
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

        for (int i = 0; i < patientCount; i++) {
            backup[i] = patients[i];
        }

        for (int i = index; i < patientCount - 1; i++) {
            patients[i] = patients[i + 1];
        }

        patientCount--;

        if (!savePatients()) {
            patientCount = oldCount;

            for (int i = 0; i < oldCount; i++) {
                patients[i] = backup[i];
            }

            return false;
        }

        return true;
    }

    void displayAll() {
        if (patientCount == 0) {
            cout << "\nNo patient records found!\n";
            return;
        }

        for (int i = 0; i < patientCount; i++) {
            patients[i].displayInfo();
        }

        cout << "\nTotal Patients: "
             << patientCount << endl;
    }

    void displayPatient(string id) {
        int index = findIndex(id);

        if (index == -1) {
            cout << "\nPatient not found!\n";
            return;
        }

        patients[index].displayInfo();
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

    int getAccessLevel() const {
        return accessLevel;
    }

    void displayInfo() const {
        cout << "\n========= ADMIN INFORMATION =========\n";
        cout << "Admin ID       : " << id << endl;
        cout << "Admin Name     : " << name << endl;
        cout << "Department     : " << department << endl;
        cout << "Access Level   : " << accessLevel << endl;
        cout << "=====================================\n";
    }
};

bool adminLogin(Admin &loggedInAdmin) {

    string username, password;
    string savedUsername, savedPassword;
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

    ifstream file("admin.txt");

    if (!file.is_open()) {
        cout << "Error: Cannot open admin.txt!\n";
        return false;
    }

    while (file >> quoted(savedUsername)
                >> quoted(savedPassword)
                >> quoted(adminID)
                >> quoted(adminName)
                >> quoted(department)
                >> accessLevel) {

        if (username == savedUsername &&
            password == savedPassword) {

            loggedInAdmin = Admin(
                adminID,
                adminName,
                department,
                accessLevel
            );

            file.close();

            cout << "\nLogin successful!\n";
            return true;
        }
    }

    file.close();

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

        if (level >= 3) {
            cout << "4. Delete Patient Records\n";
        }

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

                    string name, ailment;
                    int age;

                    cout << "\n---------- PATIENT REGISTRATION ----------\n";

                    do {
                        cout << "Enter Patient Name: ";
                        getline(cin, name);

                        if (name.empty())
                            cout << "Name cannot be empty!\n";

                    } while (name.empty());

                    do {
                        cout << "Enter Patient Ailment: ";
                        getline(cin, ailment);

                        if (ailment.empty())
                            cout << "Ailment cannot be empty!\n";

                    } while (ailment.empty());

                    while (true) {
                        cout << "Enter Patient Age: ";

                        if (cin >> age && age > 0 && age <= 100)
                            break;

                        cout << "Invalid age! Enter 1-100.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }

                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    string patientID = clinic.generatePatientID();
                    Patient newPatient(patientID, name, ailment, age);

                    if (clinic.addPatient(newPatient)) {
                        cout << "\nPatient registered successfully!\n";
                        cout << "Generated Patient ID: " << patientID << endl;
                        newPatient.displayInfo();
                    }
                    else {
                        cout << "\nRegistration failed!\n";
                    }

                }
                else {
                    cout << "Access denied!\n";
                }
                break;

            case 3:
                if (level >= 2) {

                    string patientID, patientName, ailment;
                    int age = 0;
                    int updateChoice;

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
                    }
                    else if (updateChoice == 2) {

                        while (true) {
                            cout << "\nEnter New Patient Age: ";

                            if (cin >> age && age > 0 && age <= 100)
                                break;

                            cout << "Invalid age! Enter 1-100.\n";
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }

                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else if (updateChoice == 0) {
                        cout << "\nUpdate cancelled!\n";
                        break;
                    }
                    else {
                        cout << "\nInvalid choice!\n";
                        break;
                    }

                    if (clinic.updatePatient(patientID, patientName, ailment, age, updateChoice)) {
                        cout << "\nPatient information updated successfully!\n";
                        clinic.displayPatient(patientID);
                    }
                    else {
                        cout << "\nFailed to update patient information!\n";
                    }

                }
                else {
                    cout << "Access denied!\n";
                }
                break;

            case 4:
                if (level >= 3) {

                    string patientID;

                    cout << "\n---------- DELETE PATIENT RECORD ----------\n";
                    cout << "Enter Patient ID: ";
                    getline(cin, patientID);

                    if (clinic.deletePatient(patientID)) {
                        cout << "\nPatient record deleted successfully!\n";
                    }
                    else {
                        cout << "\nPatient ID not found or deletion failed!\n";
                    }

                }
                else {
                    cout << "Access denied!\n";
                }
                break;

            default:
                cout << "\nInvalid choice or unavailable function!\n";
                break;
        }
    }
}

//login (patient)
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


void loggedInPatientDashboard(Patient &patient, ClinicRecords &clinic, AppointmentRecords &appointmentRecords) {

    int choice;

    while (true) {

        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "          PATIENT DASHBOARD\n";
        cout << "========================================\n";

        cout << "\nWelcome, " << patient.getName() << "!\n";
        cout << "Patient ID: " << patient.getID() << endl;

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
                bookAppointment(patient, appointmentRecords);
                break;

            case 3:
                appointmentRecords.displayPatientAppointments(patient.getID());
                break;

            case 4: {
                string appointmentID;

                cout << "\nEnter Appointment ID to cancel: ";
                getline(cin, appointmentID);

                if (appointmentRecords.cancelAppointment(
                        appointmentID, patient.getID())) {

                    cout << "\nAppointment cancelled successfully!\n";
                }
                else {
                    cout << "\nAppointment not found or cannot be cancelled!\n";
                }

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

//signup (patient)
void patientSignUp(ClinicRecords &clinic) {

    string name, ailment;
    string username, password;
    int age;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "          PATIENT SIGN UP\n";
    cout << "========================================\n";

    do {
        cout << "\nEnter Full Name: ";
        getline(cin, name);

        if (name.empty())
            cout << "Name cannot be empty!\n";

    } while (name.empty());

    while (true) {
        cout << "Enter Age: ";

        if (cin >> age && age > 0 && age <= 100)
            break;

        cout << "Invalid age! Enter 1-100.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    do {
        cout << "Enter Ailment: ";
        getline(cin, ailment);

        if (ailment.empty())
            cout << "Ailment cannot be empty!\n";

    } while (ailment.empty());

    while (true) {
        cout << "Create Username: ";
        getline(cin, username);

        if (username.empty()) {
            cout << "Username cannot be empty!\n";
        }
        else if (clinic.usernameExists(username)) {
            cout << "Username already exists! Try another username.\n";
        }
        else {
            break;
        }
    }

    do {
        cout << "Create Password: ";
        getline(cin, password);

        if (password.empty())
            cout << "Password cannot be empty!\n";

    } while (password.empty());

    string patientID = clinic.generatePatientID();

    Patient newPatient(
        patientID,
        name,
        ailment,
        age,
        username,
        password
    );

    if (clinic.addPatient(newPatient)) {

        cout << "\n========================================\n";
        cout << "       REGISTRATION SUCCESSFUL!\n";
        cout << "========================================\n";

        cout << "Patient ID : " << patientID << endl;
        cout << "Name       : " << name << endl;
        cout << "Username   : " << username << endl;

        cout << "\nYou can now log in to your patient account.\n";

    }
    else {
        cout << "\nRegistration failed! Please try again.\n";
    }
}




void patientDashboard(ClinicRecords &clinic, AppointmentRecords &appointmentRecords) {

    int choice;

    while (true) {

        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "           PATIENT PORTAL\n";
        cout << "========================================\n";

        cout << "\n1. Patient Sign Up\n";
        cout << "2. Patient Login\n";
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

                if (patientLogin(clinic, loggedInPatient)) {
                    loggedInPatientDashboard(loggedInPatient, clinic, appointmentRecords);
                }

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
    string doctorID, doctorName, department;
    string savedUsername, savedPassword;

    cout << "\n========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "            DOCTOR LOGIN\n";
    cout << "========================================\n";

    cout << "\nUsername: ";
    getline(cin, username);

    cout << "Password: ";
    getline(cin, password);

    ifstream file("doctors.txt");

    if (!file.is_open()) {
        cout << "Error: Cannot open doctors.txt!\n";
        return false;
    }

    while (file >> quoted(doctorID)
                >> quoted(doctorName)
                >> quoted(department)
                >> quoted(savedUsername)
                >> quoted(savedPassword)) {

        if (username == savedUsername &&
            password == savedPassword) {

            loggedInDoctor = Doctor(
                doctorID,
                doctorName,
                department,
                savedUsername,
                savedPassword
            );

            file.close();

            cout << "\nLogin successful!\n";
            return true;
        }
    }

    file.close();

    cout << "\nInvalid username or password!\n";
    return false;
}


void loggedInDoctorDashboard(Doctor &doctor,
                             AppointmentRecords &appointmentRecords) {

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
                appointmentRecords.displayDoctorAppointments(
                    doctor.getID(),
                    doctor.getName()
                );
                break;

            case 2: {

                string appointmentID;
                int action;

                cout << "\n========= MANAGE APPOINTMENT =========\n";

                appointmentRecords.displayDoctorAppointments(
                    doctor.getID(),
                    doctor.getName()
                );

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

                if (action == 1) {
                    newStatus = "Confirmed";
                }
                else if (action == 2) {
                    newStatus = "Rejected";
                }
                else {
                    cout << "\nInvalid choice!\n";
                    break;
                }

                if (appointmentRecords.updateAppointmentStatus(
                        appointmentID,
                        doctor.getID(),
                        newStatus)) {

                    cout << "\nAppointment successfully "
                         << newStatus << "!\n";
                }
                else {
                    cout << "\nUnable to update appointment.\n";
                    cout << "Please check the Appointment ID, "
                         << "assignment and current status.\n";
                }

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



void doctorDashboard(AppointmentRecords &appointmentRecords) {

    int choice;

    while (true) {

        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "           DOCTOR PORTAL\n";
        cout << "========================================\n";

        cout << "\n1. Doctor Login\n";
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

                if (doctorLogin(loggedInDoctor)) {

                    loggedInDoctorDashboard(
                        loggedInDoctor,
                        appointmentRecords
                    );
                }

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
    AppointmentRecords appointmentRecords;
    int choice;

    do {

        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "       HOSPITAL MANAGEMENT SYSTEM\n";
        cout << "========================================\n";

        cout << "\n1. Admin Dashboard\n";
        cout << "2. Patient Dashboard\n";
        cout << "3. Doctor Dashboard\n";
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

                if (adminLogin(loggedInAdmin)) {
                    adminDashboard(loggedInAdmin, clinic);
                }

                break;
            }

            case 2:
                patientDashboard(clinic, appointmentRecords);
                break;

            case 3:
                doctorDashboard(appointmentRecords);
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