#include <iostream>
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

int main() {
    string name, ailment;
    int age;
    static int patientNumber =1;

    cout << "========================================\n";
    cout << "       XIAMEN MEDICAL CENTER\n";
    cout << "          PATIENT REGISTRATION\n";
    cout << "========================================\n";

    do{
        cout << "\nEnter Patient Name: ";
        getline(cin, name);

        if(name.empty()){
            cout << "Patient name cannot be empty. Please try again.\n";
        }
    } while (ailment.empty());

    while(true) {
        cout << "Enter age: ";
        if (cin >> age && age > 0 && age < 100) {
            break;
        }
        cout << "Invalid age! Please enter 1 - 100\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string patientID = "P";

    if (patientNumber < 1000) {
        if (patientNumber < 100)
            patientID += "0";

        if (patientNumber < 10)
            patientID += "0";
    }
    patientID += to_string(patientNumber);

    Patient newPatient(patientID, name, ailment, age);
    patientNumber ++;
    cout << "\nPatient registered successfully!\n";
    newPatient.displayInfo();
    return 0;
};