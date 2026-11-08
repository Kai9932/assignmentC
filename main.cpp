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
        string getName() const{
            return name;
        }
        void displayInfo() const {
            cout << "PatientID      : " << id << endl;
            cout << "Patient Name   : " << name << endl;
        }
};

class Patient : public Person {
    private:
        string ailment;
        int age;
    public:
        Patient() : Person() {
            ailment = "";
            age = 0;
        }
        Patient(string id, string name, string ailment, int age) : Person(id, name) {
            this->ailment = ailment;
            this->age = age;
        }
        bool setAilment(string a) {
            if(a.empty())
                return false;
            ailment = a;
                return true;
            
        }
        bool setAge(int a){
            if(a <= 0 || a > 100)
                return false;
            age = a;
                return true;
        }
        string getAilment() const {
            return ailment;
        }
        int getAge() const {
            return age;
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
};

void adminDashboard() {
        ClinicRecords clinic;
    int choice;
    do {
        cout << "\n========================================\n";
        cout << "       XIAMEN MEDICAL CENTER\n";
        cout << "       PATIENT MANAGEMENT SYSTEM\n";
        cout << "========================================\n";
        cout << "\n1. Register New Patient\n";
        cout << "2. Display Patient Information\n";
        cout << "3. Exit\n";
        cout << "\nEnter your choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
            case 1: {
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
                } else {
                    cout << "\nRegistration failed!\n";
                }
                break;
            }

            case 2:
                clinic.displayAll();
                break;

            case 3:
                cout << "\nThank you for using Xiamen Medical Center!\n";
                break;
            default:
                cout << "\nInvalid choice!\n";
        }
    } while (choice != 3);
}

void patientDashboard() {
}

void doctorDashboard() {
}
int main() {
    return 0;
}
