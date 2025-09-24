#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <regex>

using namespace std;

/// CLASSES
class AccountManager // Clasa AccountManager este o clasa abstracta care contine metode pentru generarea unei chei random, criptarea parolei si verificarea puterii parolei
{
protected:
    string generateRandomKey(int length) // Metoda pentru generarea unei chei random
    {                                    // Generam o cheie random de lungimea parametrului length (lungimea parolei)
        string key = ""; // Cheia generata
        random_device rd; // Generator de numere random
        mt19937 gen(rd()); // Generator de numere random
        uniform_int_distribution<> distr(0, 25); // Distributie uniforma de numere intregi intre 0 si 25

        for (int i = 0; i < length; i++) 
        {
            key += 'A' + distr(gen); // Adaugam la cheie un caracter random intre 'A' si 'Z'
        }
        return key; // Returnam cheia generata
    }

    string vigenereEncrypt(string text, string key) // Metoda pentru criptarea parolei
    {
        string encryptedText = ""; // Textul criptat

        for (int i = 0; i < text.size(); i++)
        {
            char base = islower(text[i]) ? 'a' : 'A'; // Caracterul de baza pentru a determina daca textul este scris cu litere mici sau mari

            encryptedText += (text[i] - base + (key[i % key.size()] - 'A')) % 26 + base; // Criptam textul
        }

        return encryptedText; // Returnam textul criptat
    }

    string checkPasswordStrength(const string &password) // Metoda pentru verificarea puterii parolei
    {
        if (password.length() < 8) // Daca lungimea parolei este mai mica de 8 caractere
        {
            return "weak"; // Parola este slaba
        }

        bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false; // Variabile pentru a verifica daca parola contine litere mici, mari, cifre sau caractere speciale
        string specialChars = "!@#$%^&*()-+"; // Caractere speciale

        for (char ch : password)
        {
            if (islower(ch))
                hasLower = true;
            else if (isupper(ch))
                hasUpper = true;
            else if (isdigit(ch))
                hasDigit = true;
            else if (specialChars.find(ch) != string::npos)
                hasSpecial = true;
        }

        if (hasLower && hasUpper && hasDigit && hasSpecial) // Daca parola contine litere mici, mari, cifre si caractere speciale
        {
            return "strong"; // Parola este puternica
        }
        else if ((hasLower || hasUpper) && (hasDigit || hasSpecial)) // Daca parola contine litere mici sau mari si cifre sau caractere speciale
        {
            return "ok"; // Parola este ok
        }
        else
        {
            return "weak"; // Parola este slaba
        }
    }

    int login(string fileName, string &inputEmail) // Metoda pentru logarea unui utilizator/operator
    {
        string inputPass;
        cout << "Enter email: ";
        getline(cin, inputEmail);
        cout << "Enter password: ";
        getline(cin, inputPass);

        ifstream operatorsFile(fileName);
        if (!operatorsFile.is_open())
        {
            cerr << "Unable to open operators file!" << endl;
            return -1;
        }

        string line, storedEmail, storedEncryptedPass, storedKey;
        bool emailFound = false;

        getline(operatorsFile, line); // Citim linia header

        while (getline(operatorsFile, line)) // Citim fiecare linie din fisier
        {                                    // Verificam daca email-ul introdus de utilizator se gaseste in fisier
            size_t pos1 = line.find(',');
            size_t pos2 = line.rfind(',');

            storedEmail = line.substr(0, pos1);
            storedEncryptedPass = line.substr(pos1 + 1, pos2 - pos1 - 1);
            storedKey = line.substr(pos2 + 1);

            if (storedEmail == inputEmail)
            {
                emailFound = true; // Daca email-ul se gaseste in fisier, setam variabila emailFound pe true
                break; // Iesim din bucla
            }
        }

        operatorsFile.close();

        if (!emailFound) // Daca email-ul introdus de utilizator nu se gaseste in fisier
        {
            throw "Incorrect email!"; // Aruncam un mesaj de eroare
        }

        string encryptedInputPass = vigenereEncrypt(inputPass, storedKey); // Criptam parola introdusa de utilizator

        if (encryptedInputPass == storedEncryptedPass) // Daca parola criptata introdusa de utilizator este egala cu parola criptata din fisier
        {
            cout << endl
                 << "Login successful!" << endl; // Afisam un mesaj de succes
            return 1; // Returnam 1, logarea a fost reusita
        }
        else
        {
            throw "Incorrect password!"; // Altfel, aruncam un mesaj de eroare
        }
    }

    int registerUser(string fileName, string &email) // Metoda pentru inregistrarea unui utilizator
    {
        string pass, confirmPass, passStrength;
        cout << "Enter email: ";
        getline(cin, email);
        cout << "Enter password: ";
        getline(cin, pass);
        cout << "Confirm password: ";
        getline(cin, confirmPass);

        if (email.empty() || pass.empty() || confirmPass.empty())
        {
            throw "Please fill in all fields!";
        }
        else if (email.find('@') == string::npos || email.find('.') == string::npos) // Verificam daca email-ul introdus de utilizator contine @ si .
        {
            throw "Invalid email address! Please try again.";
        }
        else if (!regex_match(email, regex(R"(^[\w.-]+@[\w.-]+\.[a-zA-Z]{2,}$)"))) // Verificam daca email-ul introdus de utilizator respecta pattern-ul regex
        {
            throw "Invalid email address! Please try again.";
        }
        else if (pass != confirmPass) // Verificam daca cele doua parole introduse de utilizator sunt egale
        {
            throw "Passwords do not match! Retry registration.";
        }
        else
        {
            passStrength = checkPasswordStrength(pass); // Verificam puterea parolei introduse de utilizator

            if (passStrength == "weak") // Daca parola este slaba
            {
                throw "Password is too weak! Please try again and use a stronger password."; // Aruncam un mesaj de eroare
            }
        }

        ofstream usersFile(fileName, ios::app);
        if (usersFile.is_open())
        {
            string key = generateRandomKey(pass.length());
            string encryptedPass = vigenereEncrypt(pass, key);

            usersFile << email << "," << encryptedPass << "," << key << endl; // Scriem datele utilizatorului in fisier
            usersFile.close();

            cout << endl
                 << "User registered successfully! Password strength: " << passStrength << endl; // Afisam un mesaj de succes
            return 1;
        }
        else
        {
            throw "Unable to open users file!"; // Altfel, aruncam un mesaj de eroare
        }
    }
};

class TrainRoute // Clasa TrainRoute contine metode pentru adaugarea si stergerea unei rute, verificarea validitatii unei date si a unui oras
{
public:
    string departure;
    string destination;
    string date;

    TrainRoute() {}

    TrainRoute(string departure, string destination, string date)
    {
        this->departure = departure;
        this->destination = destination;
        this->date = date;
    }

    bool isValidDate(const string &date) // Metoda pentru verificarea validitatii unei date
    {
        regex datePattern("^\\d{2}/\\d{2}/\\d{4}$"); // Pattern pentru data in formatul DD/MM/YYYY
        if (!regex_match(date, datePattern)) // Verificam daca data introdusa de utilizator respecta pattern-ul
        {
            return false;
        }

        int day = stoi(date.substr(0, 2)); // Extragem ziua din data introdusa de utilizator
        int month = stoi(date.substr(3, 2)); // Extragem luna din data introdusa de utilizator
        int year = stoi(date.substr(6, 4)); // Extragem anul din data introdusa de utilizator

        if (month < 1 || month > 12 || day < 1 || day > 31) // Verificam daca luna si ziua sunt in intervalul corect
        {
            return false;
        }

        int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // Numarul de zile din fiecare luna

        if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) // Daca anul este bisect
        {
            daysInMonth[1] = 29; // Setam numarul de zile din luna februarie la 29
        }

        if (day > daysInMonth[month - 1]) // Verificam daca ziua introdusa de utilizator este valida
        {
            return false;
        }

        time_t t = time(0); // Obtinem timpul curent
        tm *now = localtime(&t); // Obtinem data curenta

        int currentYear = now->tm_year + 1900;
        int currentMonth = now->tm_mon + 1;
        int currentDay = now->tm_mday;

        if (year < currentYear ||
           (year == currentYear && month < currentMonth) ||
           (year == currentYear && month == currentMonth && day < currentDay)) // Verificam daca data introdusa de utilizator este in trecut
        {
            return false;
        }

        return true;
    }

    bool isValidCity(const string &city) // Metoda pentru verificarea validitatii unui oras
    {
        regex cityPattern("^[a-zA-Z]+(?:[\\s-][a-zA-Z]+)*$"); // Pattern pentru oras, care poate contine litere mici si mari, spatii si cratime, fara alte caractere speciale
        return regex_match(city, cityPattern);                // Verificam daca orasul introdus de utilizator respecta pattern-ul
    }

    void addRoute(string departure, string destination, string date) // Metoda pentru adaugarea unei rute
    {
        ofstream routesFile("routes.csv", ios::app);
        if (routesFile.is_open())
        {
            routesFile << departure << "," << destination << "," << date << endl; // Scriem datele rutei in fisier
            routesFile.close();
        }
        else
        {
            cerr << "Unable to open routes file!" << endl;
        }
        routesFile.close();
    }

    void deleteRoute(string departure, string destination, string date) // Metoda pentru stergerea unei rute
    {
        ifstream routesFile("routes.csv");
        if (!routesFile.is_open())
        {   
            cerr << "Unable to open routes file!" << endl;
            return;
        }

        string line, storedDeparture, storedDestination, storedDate;
        bool routeFound = false;

        getline(routesFile, line); // Citim linia header

        while (getline(routesFile, line)) // Citim fiecare linie din fisier
        {
            size_t pos1 = line.find(',');
            size_t pos2 = line.rfind(',');

            storedDeparture = line.substr(0, pos1);
            storedDestination = line.substr(pos1 + 1, pos2 - pos1 - 1);
            storedDate = line.substr(pos2 + 1);

            if (storedDeparture == departure && storedDestination == destination && storedDate == date) // Verificam daca ruta se gaseste in fisier
            {
                routeFound = true;
                break;
            }
        }

        routesFile.close();

        if (!routeFound)
        {
            throw "Route not found!";
        }

        ifstream inputFile("routes.csv");
        ofstream tempFile("temp.csv");

        if (!inputFile.is_open() || !tempFile.is_open())
        {
            cerr << "Unable to open files for deletion process!" << endl;
            return;
        }

        tempFile << "departure,destination,date" << endl; // Scriem header-ul in fisierul temporar

        getline(inputFile, line); // Citim header-ul din fisierul original

        while (getline(inputFile, line)) // Citim fiecare linie din fisierul original
        {
            if (line != storedDeparture + "," + storedDestination + "," + storedDate) // Daca linia citita nu este cea pe care dorim sa o stergem
            {
                tempFile << line << endl; // Scriem linia in fisierul temporar
            }
        }

        inputFile.close();
        tempFile.close();

        remove("routes.csv"); // Stergem fisierul original
        rename("temp.csv", "routes.csv"); // Redenumim fisierul temporar in fisierul original

        cout << endl
             << "Route deleted successfully: " << departure << " to " << destination << " on " << date << endl; // Afisam un mesaj de succes
    }
};

class Operator : public AccountManager, public TrainRoute // Clasa Operator este o clasa care mosteneste metodele din clasele AccountManager si TrainRoute
{                                                         // Contine metode pentru adaugarea si stergerea unei rute, precum si pentru logarea unui operator
private:
    string email;
    string pass;
    string file = "operators.csv";

public:
    Operator() {}

    Operator(string email, string pass)
    {
        this->email = email;
        this->pass = pass;

        ofstream operatorsFile("operators.csv", ios::app);
        if (operatorsFile.is_open())
        {
            string key = generateRandomKey(pass.length());
            string encryptedPass = vigenereEncrypt(pass, key);

            operatorsFile << email << "," << encryptedPass << "," << key << endl;
            operatorsFile.close();
        }
        else
        {
            cerr << "Unable to open operators file!" << endl;
        }
    }

    int login() // Metoda pentru logarea unui operator
    {
        cout << endl
             << "-----OPERATOR LOGIN-----" << endl;

        return (AccountManager::login(file, email) == 1);
    }

    void addRoute() // Metoda pentru adaugarea unei rute
    {
        cout << endl
             << "------NEW ROUTE------" << endl;

        cout << "Departure: ";
        string departure;
        getline(cin, departure);

        cout << "Destination: ";
        string destination;
        getline(cin, destination);

        cout << "Date (DD/MM/YYYY): ";
        string date;
        getline(cin, date);

        if (!TrainRoute::isValidDate(date)) // Verificam daca data introdusa de operator este valida
        {
            throw "Invalid date format/date is in the past!";
        }

        if (!isValidCity(departure) || !isValidCity(destination)) // Verificam daca orasele introduse de operator sunt valide
        {
            throw "Invalid city name(s)! Please try again.";
        }

        TrainRoute::addRoute(departure, destination, date); // Adaugam ruta in fisier

        cout << endl
             << "Route added successfully: " << departure << " to " << destination << " on " << date << endl; // Afisam un mesaj de succes
    }

    void deleteRoute() // Metoda pentru stergerea unei rute
    {
        cout << endl
             << "-----DELETE ROUTE-----" << endl;

        cout << "Departure: ";
        string departure;
        getline(cin, departure);

        cout << "Destination: ";
        string destination;
        getline(cin, destination);

        cout << "Date (DD/MM/YYYY): ";
        string date;
        getline(cin, date);

        if (!isValidDate(date)) // Verificam daca data introdusa de operator este valida
        {
            throw "Invalid date format/date is in the past!";
        }

        if (!isValidCity(departure) || !isValidCity(destination)) // Verificam daca orasele introduse de operator sunt valide
        {
            throw "Invalid city name(s)! Please try again.";
        }

        TrainRoute::deleteRoute(departure, destination, date); // Stergem ruta din fisier
    }
};

class User : public AccountManager, public TrainRoute // Clasa User este o clasa care mosteneste metodele din clasele AccountManager si TrainRoute
{
private:
    string email;
    string pass;
    string file = "users.csv";

public:
    User() {}

    User(string email, string pass)
    {
        this->email = email;
        this->pass = pass;
    }

    int registerUser() // Metoda pentru inregistrarea unui utilizator
    {
        string email;

        cout << endl
             << "-----USER REGISTRATION-----" << endl;

        int succesfulRegistration = (AccountManager::registerUser(file, email) == 1); // Apelam metoda pentru inregistrarea unui utilizator
        if (succesfulRegistration)
        {
            this->email = email; // Setam email-ul utilizatorului
        }

        return succesfulRegistration; // Returnam 1, daca inregistrarea a fost reusita
    }

    int login() // Metoda pentru logarea unui utilizator
    {
        string email;

        cout << endl
             << "------USER LOGIN------" << endl;

        int succesfulLoggin = (AccountManager::login(file, email) == 1); // Apelam metoda pentru logarea unui utilizator
        if (succesfulLoggin)
        {
            this->email = email; // Setam email-ul utilizatorului
        }

        return succesfulLoggin; // Returnam 1, daca logarea a fost reusita
    }

    int findRoute(string departure, string destination, string date) // Metoda pentru cautarea unei rute
    {
        ifstream routesFile("routes.csv");
        if (!routesFile.is_open())
        {
            cerr << "Unable to open routes file!" << endl;
            return -1;
        }

        string line, storedDeparture, storedDestination, storedDate;
        bool routeFound = false;

        getline(routesFile, line); // Citim linia header

        while (getline(routesFile, line)) // Citim fiecare linie din fisier
        {
            size_t pos1 = line.find(',');
            size_t pos2 = line.rfind(',');

            storedDeparture = line.substr(0, pos1);
            storedDestination = line.substr(pos1 + 1, pos2 - pos1 - 1);
            storedDate = line.substr(pos2 + 1);

            if (storedDeparture == departure && storedDestination == destination && storedDate == date) // Verificam daca ruta se gaseste in fisier
            {
                routeFound = true; // Daca ruta se gaseste in fisier, setam variabila routeFound pe true
                break;
            }
        }

        routesFile.close();

        if (routeFound)
        {
            cout << endl
                 << "Route found: " << departure << " to " << destination << " on " << date << endl; // Afisam un mesaj de succes
            return 1;
        }
        else
        {
            throw "Route not found!"; // Altfel, aruncam un mesaj de eroare
        }
    }

    void bookTicket() // Metoda pentru rezervarea unui bilet
    {
        cout << endl
             << "-----BOOK TICKET-----" << endl;

        cout << "Departure: ";
        string departure;
        getline(cin, departure);

        cout << "Destination: ";
        string destination;
        getline(cin, destination);

        cout << "Date (DD/MM/YYYY): ";
        string date;
        getline(cin, date);

        if (!TrainRoute::isValidDate(date)) // Verificam daca data introdusa de utilizator este valida
        {
            throw "Invalid date format/date is in the past!";
        }

        if (!isValidCity(departure) || !isValidCity(destination)) // Verificam daca orasele introduse de utilizator sunt valide
        {
            throw "Invalid city name(s)! Please try again.";
        }

        if (findRoute(departure, destination, date) == 1) // Verificam daca ruta este disponibila
        {
            cout << "Route is available!" << endl << endl
                 << "What class do you want?" << endl
                 << "1. First Class" << endl
                 << "2. Second Class" << endl
                 << "3. Business Class" << endl
                 << "4. Economy Class" << endl
                 << endl
                 << "Choose an option: ";
            int choice;
            cin >> choice;
            cin.ignore();

            cout << endl;

            while (choice < 1 || choice > 4) // Verificam daca optiunea aleasa de utilizator este valida
            {
                cout << "Invalid option. Please try again." << endl
                     << "Choose an option: ";
                cin >> choice;
                cin.ignore();
            }
            string classOption;
            switch (choice) // Setam clasa in functie de optiunea aleasa de utilizator
            {
            case 1:
                classOption = "First Class";
                break;
            case 2:
                classOption = "Second Class";
                break;
            case 3:
                classOption = "Business Class";
                break;
            case 4:
                classOption = "Economy Class";
                break;
            }

            cout << "Choose an hour (0-23): "; // Introducem ora
            int hour;
            cin >> hour;
            cin.ignore();

            while (hour < 0 || hour > 23) // Verificam daca ora introdusa de utilizator este valida
            {
                cout << "Invalid hour. Please try again." << endl
                     << "Choose an hour: ";
                cin >> hour;
                cin.ignore();
            }

            ofstream ticketsFile("tickets.csv", ios::app);
            if (ticketsFile.is_open())
            {
                ticketsFile << email << "," << departure << "," << destination << "," << date << "," << classOption << "," << hour << endl; // Scriem datele biletului in fisier
                ticketsFile.close();

                cout << endl
                     << "Ticket booked successfully!" << endl
                     << "Departure: " << departure << endl
                     << "Destination: " << destination << endl
                     << "Date: " << date << endl
                     << "Class: " << classOption << endl
                     << "Hour: " << hour << endl;
            }
            else
            {
                throw "Unable to open tickets file!"; // Altfel, aruncam un mesaj de eroare
            }
        }
    }
};

/// MENU
int mainMenu() // Meniul principal
{
    cout << endl
         << "------MAIN MENU------" << endl
         << "1. Operator" << endl
         << "2. User" << endl
         << "3. Exit" << endl
        
         << endl
         << "Choose an option: ";
    int choice;
    cin >> choice;
    cin.ignore();
    return choice;
}

void operatorMenu(Operator &op) // Meniul pentru operator
{
    while (true)
    {
        cout << endl
             << "-----OPERATOR MENU-----" << endl
             << "1. Add Route" << endl
             << "2. Delete Route" << endl
             << "3. Logout" << endl
             << endl
             << "Choose an option: ";
        int choice;
        cin >> choice;
        cin.ignore();

        try
        {
            switch (choice)
            {
            case 1:
                op.addRoute();
                break;
            case 2:
                op.deleteRoute();
                break;
            case 3:
                return;
            default:
                throw "Invalid option. Please try again.";
                break;
            }
        }
        catch (const char *msg)
        {
            cerr << endl
                 << msg << endl;
        }
    }
}

void userMenu(User &user) // Meniul pentru utilizator
{
    bool loggedIn = false;

    while (true)
    {
        cout << endl
             << "-----USER MENU-----" << endl
             << "1. Register" << endl
             << "2. Login" << endl
             << "3. Search Route" << endl
             << "4. Book Ticket" << endl
             << "5. Logout" << endl
             << endl
             << "Choose an option: ";
        int choice;
        cin >> choice;
        cin.ignore();

        try
        {
            string departure, destination, date;
            switch (choice)
            {
            case 1:
                if (user.registerUser() == 1)
                {
                    loggedIn = true;
                }
                break;
            case 2:
                if (user.login() == 1)
                {
                    loggedIn = true;
                }
                break;
            case 3:
                if (!loggedIn)
                {
                    throw "Please login first!";
                }
                
                cout << endl
                        << "-----Search Route-----" << endl;
                
                cout << "Departure: ";
                getline(cin, departure);

                cout << "Destination: ";
                getline(cin, destination);

                cout << "Date (DD/MM/YYYY): ";
                getline(cin, date);

                if (!user.isValidDate(date))
                {
                    throw "Invalid date format/date is in the past!";
                }

                if (!user.isValidCity(departure) || !user.isValidCity(destination))
                {
                    throw "Invalid city name(s)! Please try again.";
                }

                if (user.findRoute(departure, destination, date) == 1)
                {
                    cout << "Route is available!" << endl;
                }
                else
                {
                    throw "Route not found!";
                }
                break;
            case 4:
                if (!loggedIn)
                {
                    throw "Please login first!";
                }
                user.bookTicket();
                break;
            case 5:
                return;
            default:
                throw "Invalid option. Please try again.";
                break;
            }
        }
        catch (const char *msg)
        {
            cerr << endl
                 << msg << endl;
        }
    }
}

/// MAIN
int main()
{
    ofstream operatorsFile("operators.csv", ios::trunc); // Cream fisierele operators.csv, users.csv, routes.csv si tickets.csv
    if (!operatorsFile.is_open())                        // Adaugam header-ul in fiecare fisier
    {
        cerr << "Unable to open operators file!" << endl;
    }
    operatorsFile << "email,password,encryption_key" << endl;
    operatorsFile.close();

    ofstream usersFile("users.csv", ios::trunc);
    if (!usersFile.is_open())
    {
        cerr << "Unable to open users file!" << endl;
    }
    usersFile << "email,password,encryption_key" << endl;
    usersFile.close();

    ofstream routesFile("routes.csv", ios::trunc);
    if (!routesFile.is_open())
    {
        cerr << "Unable to open routes file!" << endl;
    }
    routesFile << "departure,destination,date" << endl;
    routesFile.close();

    ofstream ticketsFile("tickets.csv", ios::trunc);
    if (!ticketsFile.is_open())
    {
        cerr << "Unable to open tickets file!" << endl;
    }
    ticketsFile << "user,departure,destination,date,class,hour" << endl;
    ticketsFile.close();

    Operator tempOperator;
    Operator operator1("operator1@mail.com", "password1"); // Operatorii sunt prestabiliti si adaugati in fisierul operators.csv
    Operator operator2("operator2@mail.com", "password2");

    User tempUser; // Instantiem un obiect de tip User pentru a putea apela metodele din clasa User

    while (true)
    {
        int choice = mainMenu();
        switch (choice)
        {
        case 1:
            try
            {
                if (tempOperator.login() == 1)
                {
                    operatorMenu(tempOperator);
                }
            }
            catch (const char *msg)
            {
                cerr << endl
                     << msg << endl;
            }
            break;
        case 2:
            try
            {
                userMenu(tempUser);
            }
            catch (const char *msg)
            {
                cerr << endl
                     << msg << endl;
            }
            break;
        case 3:
            cout << "Exiting program..." << endl;
            return 0;
        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    }
}