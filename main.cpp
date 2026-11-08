#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <iomanip>
using namespace std;

const string FILE_NAME = "patients.txt";

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


class ClinicRecords {

private:
    Patient patients[10];
    int patientCount;

    void loadPatients() {
        ifstream file(FILE_NAME);

        if (!file.is_open()) {
            return;
        }

        string id, name, ailment;
        int age;

        while (patientCount < 10 &&
               file >> quoted(id)
                    >> quoted(name)
                    >> quoted(ailment)
                    >> age) {

            Patient p(id, name, ailment, age);
            patients[patientCount] = p;
            patientCount++;
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
                 << patients[i].getAge() << endl;
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
             << p.getAge() << endl;

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

void patientDashboard() {
}

void doctorDashboard() {
}

int main() {

    ClinicRecords clinic;
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
                patientDashboard();
                break;

            case 3:
                doctorDashboard();
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