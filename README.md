# assignmentC

# Hospital Appointment System - XIAMEN MEDICAL CENTER

## Live Demo

You can access the online Hospital Management System here:

[Open Hospital Appointment System](https://hospitalappointmentsystem.replit.app/)

## 1. Features

-   **Admin:** View patient records, register patients, update patient
    information and delete patient records (subject to access level).
-   **Patient:** Sign up, log in, view personal information, book
    appointments, view appointments and cancel appointments.
-   **Doctor:** Log in, view assigned appointments, and confirm or
    reject pending appointment requests.
-   Input validation, appointment availability checking, appointment
    limits and text-file persistence.

## 2. Requirements

-   Windows
-   MinGW-w64 `g++` compiler with C++17 support
-   Visual Studio Code (optional)

No third-party C++ libraries are required.

## 3. Project folder structure

Keep the files together in one project folder:

``` text
HospitalManagementSystem/
├── main.cpp
├── admin.txt
├── doctors.txt
├── patients.txt
├── appointments.txt
└── README.md
```

**Important:** The program uses relative filenames (`admin.txt`,
`doctors.txt`, `patients.txt`, `appointments.txt`). Run it with the
project folder as the current working directory.

-   `admin.txt` and `doctors.txt` must exist and contain valid login
    records.
-   `patients.txt` stores patient accounts.
-   `appointments.txt` stores appointment records.
-   `patients.txt` and `appointments.txt` can initially be absent; they
    are created when the program successfully saves records.

## 4. Text-file formats

Each record must be on a separate line. Text values must be enclosed in
double quotation marks.

### admin.txt

``` text
"username" "password" "adminID" "adminName" "department" accessLevel
```

Example format only:

``` text
"adminUsername" "adminPassword" "A001" "System Admin" "Administration" 3
```

### doctors.txt

``` text
"doctorID" "doctorName" "specialty" roomNumber "username" "password"
```

Example format only:

``` text
"D001" "Dr. Example" "Cardiology" 101 "doctorUsername" "doctorPassword"
```

Use the actual accounts configured for your project. The example
credentials above are placeholders, not working accounts.

### patients.txt

``` text
"patientID" "patientName" "ailment" age "username" "password"
```

This file can initially be empty. Patient records are saved after
successful registration.

### appointments.txt

``` text
"appointmentID" "patientID" "doctorID" "date" "time" "status"
```

This file can initially be empty. Appointment records are saved after
successful booking and status updates.

## 5. Method 1 -- Compile and run using the VS Code terminal (recommended)

1.  Open the project folder in Visual Studio Code.
2.  Select **Terminal → New Terminal**.
3.  Check that `g++` is available:

``` powershell
g++ --version
```

4.  Compile the program:

``` powershell
g++ main.cpp -o main.exe -std=c++17
```

5.  Run it:

``` powershell
.\main.exe
```

The main menu should appear:

``` text
========================================
       XIAMEN MEDICAL CENTER
       HOSPITAL MANAGEMENT SYSTEM
========================================

1. Admin
2. Patient
3. Doctor
4. Exit

Select User Type:
```

Make sure the terminal is currently in the project folder before
compiling and running.

## 6. Method 2 -- Compile and run using Windows Command Prompt

Use this method if the VS Code right-click **Run** option does not work.

1.  Open the project folder in File Explorer.
2.  Click the address bar, type `cmd`, and press Enter. Command Prompt
    opens in that folder.
3.  Compile:

``` bat
g++ main.cpp -o main.exe -std=c++17
```

4.  Run:

``` bat
main.exe
```

This method starts the program from the project folder, allowing it to
find the text files.

## 7. Method 3 -- Run an already compiled executable

If `main.exe` has already been compiled:

1.  Open the project folder in File Explorer.
2.  Double-click `main.exe`.

If the console closes immediately or the program cannot find its data
files, use Method 1 or Method 2 instead. Running through a terminal
keeps the console open and makes errors easier to see.
