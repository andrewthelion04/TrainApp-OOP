# Train Booking CLI — C++ (OOP + CSV persistence)

A small, menu-driven console app that simulates a **train ticket system** with two roles:

- **Operator**: add/delete routes  
- **User**: register, log in, search routes, book tickets

Data is stored in simple **CSV** files alongside the executable.

---

## ✨ Features

- **User Account Management**
  - Email & password registration (basic regex validation)
  - Password **Vigenère** encryption with a per-password random key (demo-only)
  - Login with encrypted verification

- **Routes & Tickets**
  - Add/Delete **routes** (Operator)
  - Validate **date** (`DD/MM/YYYY`, must be in the future; leap years handled)
  - Validate **city names** (letters/spaces/hyphens)
  - Search routes & **book tickets** (User)

- **CSV Persistence** (auto-created with headers on app start)
  - `operators.csv` — `email,password,encryption_key`
  - `users.csv` — `email,password,encryption_key`
  - `routes.csv` — `departure,destination,date`
  - `tickets.csv` — `user,departure,destination,date,class,hour`

⚠️ On startup, the program **truncates** all CSVs and seeds two operators:  
`operator1@mail.com / password1` and `operator2@mail.com / password2`.

---

## 🧠 OOP Design

- **Classes**
  - `AccountManager`: password key generation, Vigenère encryption, login/registration helpers, password strength check.  
  - `TrainRoute`: route data & validators (date/city), add/delete helpers.  
  - `Operator`: multiple inheritance from `AccountManager` + `TrainRoute`; role-specific menus.  
  - `User`: multiple inheritance from `AccountManager` + `TrainRoute`; user flows (register/login/search/book).  

- **Concepts Used**
  - Encapsulation of auth & validation logic.  
  - Multiple inheritance for code reuse.  
  - Abstraction of common operations.  
  - Exception handling with simple `throw const char*`.  

---

## 🛠️ Build & Run

Requires a C++17 compiler.

```bash
# Linux / macOS
g++ -std=cpp17 -O2 -Wall main.cpp -o train_cli
./train_cli

# Windows (MSVC)
cl /std:c++17 /EHsc main.cpp && main.exe
