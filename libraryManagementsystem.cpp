#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib> 
#include <cctype>// Required for tolower function

#define _CRT_SECURE_NO_WARNINGS
using namespace std;

// Constants for max users and books
const int MAX_USERS = 100;

// User struct
struct User {
    string username;
    string password;
    string role; // "admin", "registered", or "visitor"
    User* next;  // Pointer to the next user (for linked list)

};

// Book struct (Linked List)
struct Book {
    string title;
    string author;
    string genre;
    string ISBN;
    bool isAvailable;
    Book* next; // Pointer to the next book in the linked list
};
struct Event {
    string name;
    string date;
    string time;
    string description;
    Event* left;
    Event* right;

    Event(string eventName, string eventDate, string eventTime, string eventDescription) {
        name = eventName;
        date = eventDate;
        time = eventTime;
        description = eventDescription;
        left = right = nullptr;
    }
};
struct Transaction {
    int transactionID;
    string username;      // match with User.username
    string bookTitle;     // match with Book.title
    string action;        // "Borrow" or "Return"
    string date;
    Transaction* next;
};
struct Review {
    string username;
    string bookTitle;
    string reviewText;
    Review* next;
};

Review* reviewHead = nullptr;
User* loggedInUser = nullptr;
Book* searchCatalog(string searchTerm);
Transaction* transactionHead = nullptr;
User* userHead = nullptr;
Book* bookHead = nullptr;
User* currentUser = nullptr;
void insertNewBook(User* user);
void manageUsers(User* currentUser);
void displayUsers();
User* searchUser(const string& username);
void deleteUser(const string& username, User* currentUser);
void editEvent(Event* root, string name);
Event* insertEvent(Event* root, string name, string date, string time, string description);
void inorderTraversal(Event* root);
void deleteBook(string ISBN);
void updateBook(string ISBN);
void displayReviews(User* user);
void displayCatalog();
void showDashboard(User* user);
void viewBorrowedBooks(User* user);
void eventmanagement(User* user);
void visitor();
void mainMenu();
Event* eventRoot = nullptr;


// Flag to track if an admin exists
bool adminExists = false;

int transactionCounter = 1; // Auto-increment I
// Function to insert a new book into the catalog (sorted by title)

#include <map>

void generateReport(User* user) {
    if (transactionHead == nullptr) {
        cout << "No transactions to generate report.\n";
        cout << "\nPress Enter to return to the Dashboard...";
        cin.ignore(1000, '\n');  // Instead of using numeric_limits
        cin.get();
        system("cls");  // Clear screen (Windows)
        showDashboard(user);
        return;
    }

    int totalTransactions = 0;
    int totalBorrowed = 0;
    int totalReturned = 0;

    map<string, int> bookBorrowCount;
    map<string, int> userTransactionCount;

    Transaction* temp = transactionHead;
    while (temp != nullptr) {
        totalTransactions++;

        if (temp->action == "Borrow") {
            totalBorrowed++;
            bookBorrowCount[temp->bookTitle]++;
        }
        else if (temp->action == "Return") {
            totalReturned++;
        }

        userTransactionCount[temp->username]++;
        temp = temp->next;
    }

    cout << "\n--- Report Summary ---\n";
    cout << "Total Transactions: " << totalTransactions << "\n";
    cout << "Total Borrowed: " << totalBorrowed << "\n";
    cout << "Total Returned: " << totalReturned << "\n";

    cout << "\nBook Borrow Counts:\n";
    for (Book* b = bookHead; b != nullptr; b = b->next) {
        cout << b->title << ": " << bookBorrowCount[b->title] << " times\n";
    }

    cout << "\nUser Transactions:\n";
    for (User* u = userHead; u != nullptr; u = u->next) {
        cout << u->username << ": " << userTransactionCount[u->username] << " transactions\n";
    }

    cout << "---------------------\n";

    // Prompt user to go back
    cout << "\nPress Enter to go back to Dashboard...";
    cin.ignore(1000, '\n');  // Flush input buffer without <limits>
    cin.get();

    system("cls");  // Clear screen (Windows)
    showDashboard(user);
}


void displayReviews(User* user) {
    if (reviewHead == nullptr) {
        cout << "No reviews yet.\n";
        cout << "Do you want to return to the dashboard? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            showDashboard(user);
            return;
        }
        else {
            // You can decide what to do if user says no
            cout << "Staying on the reviews page.\n";
            // Maybe call displayReviews again, or just return
            return;
        }
    }

    cout << "\n=== All Reviews ===\n";
    Review* temp = reviewHead;
    while (temp != nullptr) {
        cout << "Book Title: " << temp->bookTitle << "\n";
        cout << "Review by: " << temp->username << "\n";
        cout << "Review: " << temp->reviewText << "\n\n";
        temp = temp->next;
    }
    showDashboard(user);
}


void insertBook(string title, string author, string genre, string ISBN, bool isAvailable) {
    Book* newBook = new Book{ title, author, genre, ISBN, isAvailable, nullptr };

    if (bookHead == nullptr || bookHead->title > title) {
        newBook->next = bookHead;
        bookHead = newBook;

    }
    else {
        Book* temp = bookHead;
        while (temp->next != nullptr && temp->next->title < title) {
            temp = temp->next;
        }
        newBook->next = temp->next;
        temp->next = newBook;
    }
}
Book* searchCatalog(string searchTerm) {
    Book* temp = bookHead;
    while (temp != nullptr) {
        if (temp->title == searchTerm) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}
#include <limits>  // for std::numeric_limits

void searchCataloggWithexit(User* user) {
    string searchTerm;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clear leftover newline
    while (true) {
        cout << "Enter the title of the book to search (or type 'exit' to quit): ";
        getline(cin, searchTerm);

        if (searchTerm == "exit") {
            cout << "Exiting search.\n";
            break; // Exit the search loop
        }

        Book* result = searchCatalog(searchTerm);
        if (result != nullptr) {
            cout << "Book found: " << result->title << ", Author: " << result->author << endl;
     
        }
        else {
            cout << "No book found with the title: " << searchTerm << ". Please try again.\n";
        }
    }
    system("cls");  // Clear screen (Windows only)
    showDashboard(user);
}



//manage users
void manageUsers(User* currentUser) {
    int choice;
    system("cls");
    string username;

    cout << "\n=== Manage Users ===\n";
    cout << "1. Display Users\n";
    cout << "2. Search User\n";
    cout << "3. Delete User\n";
    cout << "4. Go Back to Admin Dashboard\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {
    case 1:
        displayUsers();
        break;
    case 2:
    {
        cout << "Enter the username to search: ";
        cin >> username;
        User* user = searchUser(username);
        if (user != nullptr) {
            cout << "User found: " << user->username << ", Role: " << user->role << endl;
        }
        else {
            cout << "User not found.\n";
        }
        char choice;
        cout << "Would you like to go back to Manage Users? (Y/N): ";
        cin >> choice;
        if (tolower(choice) == 'y') {
            manageUsers(currentUser);  // Go back to manage users menu
        }
        break;
    }

    case 3:
        cout << "Enter the username to delete: ";
        cin >> username;
        deleteUser(username, currentUser);
        break;
    case 4:
        showDashboard(currentUser);
        break;
    default:
        cout << "Invalid choice. Please try again.\n";
        break;
    }
}

// Function to display all users
void displayUsers()
{
    system("cls");
    cout << "\n=== User List ===\n";
    User* temp = userHead;
    if (temp == nullptr) {
        cout << "No users found.\n";
        return;
    }
    while (temp != nullptr) {
        cout << "Username: " << temp->username << ", Role: " << temp->role << endl;
        temp = temp->next;
    }
    char choice;
    cout << "Would you like to go back to Manage Users? (Y/N): ";
    cin >> choice;
    if (tolower(choice) == 'y') {
        manageUsers(currentUser);  // Go back to manage users menu
    }

}

// Function to search a user by username
User* searchUser(const string& username) {
    User* temp = userHead;
    while (temp != nullptr) {
        if (temp->username == username) {
            return temp;  // User found
        }
        temp = temp->next;
    }
    return nullptr;  // User not found

}

// Function to delete a user by username
void deleteUser(const string& username, User* currentUser) {
    system("cls");
    User* temp = userHead;
    User* prev = nullptr;

    // Case 1: User is at the head
    if (temp != nullptr && temp->username == username) {
        userHead = temp->next;
        delete temp;
        cout << "User \"" << username << "\" has been deleted.\n";
        goto askReturn;
    }

    // Case 2: Search in the list
    while (temp != nullptr && temp->username != username) {
        prev = temp;
        temp = temp->next;
    }

    // Case 3: User not found
    if (temp == nullptr) {
        cout << "User \"" << username << "\" not found.\n";
        goto askReturn;
    }

    // Delete the user
    prev->next = temp->next;
    delete temp;
    cout << "User \"" << username << "\" has been deleted.\n";

askReturn:
    char choice;
    while (true) {
        cout << "\nWhat would you like to do next?\n";
        cout << "Y - Return to Manage Users\n";
        cout << "N - Delete Another User\n";
        cout << "Enter your choice (Y/N): ";
        cin >> choice;
        choice = tolower(choice);

        if (choice == 'y') {
            manageUsers(currentUser);
            break;
        }
        else if (choice == 'n') {
            cin.ignore(); // clear input buffer
            string anotherUsername;
            cout << "Enter the username of the user to delete: ";
            getline(cin, anotherUsername);
            deleteUser(anotherUsername, currentUser); // recursive call
            break;
        }
        else {
            cout << "Invalid input. Please enter Y or N.\n";
        }
    }
}

void eventmanagement(User* user) {
    system("cls");
    cout << "\n=== Event Management ===\n";
    int eventChoice;
    string choicee;
    string eventName;
    cout << "1. Create New Event\n";
    cout << "2. Edit Existing Event\n";
    cout << "3. Display All Events\n";
    cout << "4.exit\n";// Display all events
    cout << "Enter your choice: ";
    cin >> eventChoice;

    cin.ignore();  // To ignore leftover newline character from previous input
    if (eventChoice == 1) {
        string choicee;

        do {
            string name, date, time, description;
            cin.ignore(); // To clear the buffer before getline (only needed once at start)

            cout << "Enter event name: ";
            getline(cin, name);

            cout << "Enter event date (DD/MM/YYYY): ";
            getline(cin, date);

            cout << "Enter event time (HH:MM): ";
            getline(cin, time);

            cout << "Enter event description: ";
            getline(cin, description);

            eventRoot = insertEvent(eventRoot, name, date, time, description);
            cout << "Event created successfully!\n";

            cout << "\nDo you want to return to the dashboard? (yes or no): ";
            getline(cin, choicee);  // use getline to avoid leftover newline issues

            if (choicee == "yes" || choicee == "Yes") {
                system("cls"); // optional
                showDashboard(user);
                return; // Important: exit this function to avoid continuing the loop
            }

            // If user enters "no", the loop will continue and allow adding more events

        } while (true);
    }

    else if (eventChoice == 2) {
        string choicee;

        do {
            string eventName;
            cin.ignore(); // clear buffer before getline (only once at start)

            cout << "Enter event name to edit: ";
            getline(cin, eventName);

            editEvent(eventRoot, eventName);

            cout << "\nDo you want to return to the dashboard? (yes or no): ";
            getline(cin, choicee); // use getline to handle spaces and leftover newline

            if (choicee == "yes" || choicee == "Yes") {
                system("cls"); // optional
                showDashboard(user);
                return; // exit after going back to dashboard
            }

            // If "no", loop will continue for more edits

        } while (true);
    }

    else if (eventChoice == 3) {
        string choicee;

        do {
            cout << "\n=== All Events ===\n";
            inorderTraversal(eventRoot);  // Display events sorted

            cout << "\nDo you want to return to the dashboard? (yes or no): ";
            cin.ignore();  // Clear newline left in buffer before getline
            getline(cin, choicee);

            if (choicee == "yes" || choicee == "Yes") {
                system("cls");  // Optional: clear screen
                showDashboard(user);
                return;  // Exit function to stop loop
            }
            // If "no", loop repeats to show events again
        } while (true);
    }

    else if (eventChoice == 4) {
        showDashboard(user);
    }
    else {
        cout << "Invalid choice.\n";
    }
}
Event* insertEvent(Event* root, string name, string date, string time, string description) {
    if (root == nullptr) {
        return new Event(name, date, time, description);
    }

    if (name < root->name) {
        root->left = insertEvent(root->left, name, date, time, description);
    }
    else if (name > root->name) {
        root->right = insertEvent(root->right, name, date, time, description);
    }

    return root;
}

Event* searchEvent(Event* root, string name) {
    if (root == nullptr || root->name == name) {
        return root;
    }

    if (name < root->name) {
        return searchEvent(root->left, name);
    }
    else {
        return searchEvent(root->right, name);
    }
}

void editEvent(Event* root, string name) {
    Event* event = searchEvent(root, name);
    if (event != nullptr) {
        cout << "Editing event: " << event->name << endl;

        cout << "Enter new event date (DD/MM/YYYY): ";
        getline(cin, event->date);

        cout << "Enter new event time (HH:MM): ";
        getline(cin, event->time);

        cout << "Enter new event description: ";
        getline(cin, event->description);

        cout << "Event updated successfully!\n";
    }
    else {
        cout << "Event not found.\n";
    }
}
//In-order traversal is used to display the events in sorted order.

void inorderTraversal(Event* root) {
    system("cls");  // Clear screen BEFORE starting display

    if (root == nullptr) {
        cout << "No library events available at the moment.\n";
    }
    else {
        // Inorder traversal
        inorderTraversal(root->left);
        cout << "Event Name: " << root->name << "\n";
        cout << "Date: " << root->date << "\n";
        cout << "Time: " << root->time << "\n";
        cout << "Description: " << root->description << "\n";
        cout << "----------------------\n";
        inorderTraversal(root->right);
    }

    cout << "\nPress Enter to return...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    visitor();
}

void manageCatalogMenu(User* user) {
    int choice;
    system("cls");
    string ISBN;
    do {
        cout << "\n=== Manage Catalog ===\n";
        cout << "1. Update Book\n";
        cout << "2. Delete Book\n";
        cout << "3. View Catalog\n";
        cout << "4. Back to Admin Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter Book ID to update: ";
            cin >> ISBN;
            updateBook(ISBN);
            break;
        case 2:
            cout << "Enter Book ID to delete: ";
            cin >> ISBN;
            deleteBook(ISBN);
            break;
        case 3:
            displayCatalog();
            break;
        case 4:
            cout << "Returning to Admin Menu...\n";
            showDashboard(user);
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 4);
}
void deleteBook(string ISBN) {
    Book* temp = bookHead;
    Book* prev = nullptr;

    while (temp != nullptr && temp->ISBN != ISBN) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr) {
        cout << "Book not found.\n";
        return;
    }

    if (prev == nullptr) {
        bookHead = temp->next;
    }
    else {
        prev->next = temp->next;
    }

    delete temp;
    cout << "Book deleted successfully.\n";
}
void updateBook(string ISBN) {
    Book* temp = bookHead;
    while (temp != nullptr) {
        if (temp->ISBN == ISBN) {
            cin.ignore();
            cout << "Enter new title: "; getline(cin, temp->title);
            cout << "Enter new author: "; getline(cin, temp->author);
            cout << "Enter new genre: "; getline(cin, temp->genre);
            cout << "Enter new ISBN: "; getline(cin, temp->ISBN);
            system("cls");
            cout << "Book updated successfully!\n";
            return;
        }
        temp = temp->next;
    }system("cls");
    cout << "Book not found.\n";
}

void displayCatalog() {
    system("cls");  // Clear screen before displaying

    if (bookHead == nullptr) {
        cout << "Catalog is empty.\n";
    }
    else {
        Book* temp = bookHead;
        while (temp != nullptr) {
            cout << "\n--- Book ---\n";
            cout << "Title: " << temp->title << "\n";
            cout << "Author: " << temp->author << "\n";
            cout << "Genre: " << temp->genre << "\n";
            cout << "ISBN: " << temp->ISBN << "\n";
            temp = temp->next;
        }
    }

    cout << "\nPress Enter to continue...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear input buffer
    cin.get();  // Wait for user to press Enter

    // After this, call your dashboard or menu function
    // Example:
   
}

void borrowBook(User* user) {
    string bookTitle;
    cout << "\n=== Borrow Book ===\n";
    cout << "Enter the title of the book you want to borrow: ";
    cin.ignore(); // To clear the input buffer
    getline(cin, bookTitle);

    // Perform search
    Book* result = searchCatalog(bookTitle);

    if (result != nullptr) {
        if (result->isAvailable) {
            result->isAvailable = false;
            system("cls");
            cout << "You have successfully borrowed the book: " << result->title << endl;

            // Create a transaction
            Transaction* newTransaction = new Transaction;
            newTransaction->transactionID = transactionCounter++;
            newTransaction->username = user->username;
            newTransaction->bookTitle = result->title;
            newTransaction->action = "Borrow";

            // Simple dummy date
            time_t now = time(0);
            char timeBuffer[26];
            ctime_s(timeBuffer, sizeof(timeBuffer), &now);
            newTransaction->date = timeBuffer;
            newTransaction->date.pop_back(); // Optional: remove trailing newline// Remove trailing newline

            newTransaction->next = nullptr;

            // Append to transaction list
            if (transactionHead == nullptr) {
                transactionHead = newTransaction;
            }
            else {
                Transaction* temp = transactionHead;
                while (temp->next != nullptr) {
                    temp = temp->next;
                }
                temp->next = newTransaction;
            }

        }
        else {
            system("cls");
            cout << "Sorry, the book is currently unavailable.\n";
        }
    }
    else {
        system("cls");
        cout << "No book found with the title: " << bookTitle << endl;
    }

    showDashboard(user);
}

void viewBorrowedBooks(User* user) {
    system("cls");
    cout << "\n=== Borrowed Books ===\n";

    if (transactionHead == nullptr) {
        cout << "No transactions found.\n";
        cout << "Press Enter to return to the dashboard...";
        cin.ignore(); // to clear leftover newline
        cin.get();    // wait for user input
        showDashboard(user);
        return;
    }

    bool found = false;
    Transaction* temp = transactionHead;

    while (temp != nullptr) {
        if (temp->action == "Borrow") {
            found = true;
            cout << "Transaction ID: " << temp->transactionID << endl;
            cout << "Username      : " << temp->username << endl;
            cout << "Book Title    : " << temp->bookTitle << endl;
            cout << "Date Borrowed : " << temp->date << endl;
            cout << "--------------------------\n";
        }
        temp = temp->next;
    }

    if (!found) {
        cout << "No borrowed books found.\n";
    }

    cout << "Press Enter to return to the dashboard...";
    cin.ignore();
    cin.get();

    showDashboard(user);
}

void submitReview(User* user) {
    cin.ignore(); // Clear input buffer before getline
    string bookTitle, reviewText;

    cout << "\n=== Submit Review ===\n";
    cout << "Enter book title: ";
    getline(cin, bookTitle);

    cout << "Enter your review: ";
    getline(cin, reviewText);

    // Create a new review node
    Review* newReview = new Review{ user->username, bookTitle, reviewText, nullptr };

    // Append to the review list
    if (reviewHead == nullptr) {
        reviewHead = newReview;
    }
    else {
        Review* temp = reviewHead;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newReview;
    }
    system("cls");
    cout << "Review submitted successfully!\n";

    showDashboard(user); // Go back to dashboard
}

// Function to register a new user
void registerUser() {
    string username, password, role;

    cout << "\n=== User Registration ===\n";
    cout << "Enter username: ";
    cin >> username;

    // Check if the user already exists
    User* temp = userHead;
    while (temp != nullptr) {
        if (temp->username == username) {
            cout << "Username already exists! Please choose a different one.\n";
            return;
        }
        temp = temp->next;
    }

    // User does not exist, proceed with registration
    cout << "Enter password: ";
    cin >> password;

    // Ensure only one admin exists
    if (adminExists) {
        system("cls");
        cout << "Admin already exists! You can only register as a 'registered' or 'visitor'.\n";
        role = "registered"; // Default to registered if admin exists
    }
    else {
        cout << "Enter role (admin/registered): ";
        cin >> role;

        if (role == "admin") {
            adminExists = true; // Mark that an admin has been created
        }
    }

    // Create a new user node
    User* newUser = new User{ username, password, role, nullptr };

    // Add new user to the linked list
    if (userHead == nullptr) {
        userHead = newUser;
    }
    else {
        temp = userHead;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newUser;
    }
    system("cls");
    cout << "Registration successful!\n";

}
void searchCatalogg() {
    string searchTerm;
    char choice;

    do {
        cout << "Enter the title of the book to search: ";
        getline(cin, searchTerm);

        Book* result = searchCatalog(searchTerm);
        if (result != nullptr) {
            system("cls");
            cout << "\nBook found!\n";
            cout << "Title : " << result->title << endl;
            cout << "Author: " << result->author << endl;
            cout << "Genre : " << result->genre << endl;
            cout << "ISBN  : " << result->ISBN << endl;
            cout << "Availability: " << (result->isAvailable ? "Available" : "Not Available") << endl;
        }
        else {
            system("cls");
            cout << "No book found with the title: " << searchTerm << endl;
        }

        cout << "\nDo you want to search again? (y/n): ";
        cin >> choice;
        cin.ignore(); // To consume the leftover newline after cin

    } while (choice == 'y' || choice == 'Y');
    system("cls");

    visitor(); // Return to visitor menu
}
//function to handle User Login
User* loginSystem() {
    string inputUsername, inputPassword;

    cout << "\n=== Login ===\n";
    cout << "Enter username: ";
    cin >> inputUsername;

    User* temp = userHead;
    while (temp != nullptr) {
        if (temp->username == inputUsername) {
            cout << "Enter password: ";
            cin >> inputPassword;

            if (temp->password == inputPassword) {
                cout << "\nLogin successful! Welcome, " << inputUsername << ".\n";
                system("cls");
                return temp;
                // Return the logged-in user
            }
            else {
                cout << "\nIncorrect password.\n";
                system("cls");
                return nullptr;

            }
        }
        temp = temp->next;
    }

    cout << "\nUser not found.\n";
    system("cls");
    return nullptr;
    // User not found
}

// Function to show the dashboard based on the user's role

void showDashboard(User* user) {
    if (user == nullptr) {
        cout << "No user logged in.\n";
        return;
    }

    if (user->role == "admin") {
        system("cls");
        cout << "\n=== Admin Dashboard ===\n";

        int choice;

        cout << "1. Insert New Book\n";
        cout << "2. Manage Users\n";
        cout << "3. Manage Catalog\n";
        cout << "4. Create/Edit Events\n";
        cout << "5. Generate Reports\n";
        cout << "6. See Reviews\n";
        cout << "7. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            system("cls");
            insertNewBook(user);
            break;
        case 2:
            system("cls");
            cout << "You chose to Manage Users.\n";
            // Assume the admin is logged in
            manageUsers(user); // Show the admin dashboard
            break;
        case 3:
            system("cls");
            cout << "You chose to Manage Catalog.\n";
            manageCatalogMenu(user);
            break;
        case 4:
            system("cls");
            cout << "You chose to Create/Edit Events.\n";
            eventmanagement(user);
            break;
        case 5:
            system("cls");
            cout << "You chose to Generate Reports.\n";
            generateReport(user);
            break;
        case 6:
            system("cls");
            cout << "You chose to see review.\n";

            displayReviews(user);
            return;
            break;
        case 7:
            system("cls");
            cout << "You chose to Logout.\n";
            cout << "Exiting...\n";
            return;
            break;
        default:
            system("cls");
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
    else if (user->role == "registered") {
        cout << "\n=== Registered User Dashboard ===\n";
        int choicee;
        cout << "1. Search Catalog\n";
        cout << "2. Borrow Book\n";
        cout << "3. View Borrowed Books\n";
        cout << "4. Submit Review\n";
        cout << "5. Logout\n";
        cout << "Enter your choice: ";
        cin >> choicee;
        string searchTerm;
        switch (choicee) {
        case 1:
            system("cls");
            cout << "You chose to Search Catalog.\n";
            searchCataloggWithexit(user);

            // Logic for searching the catalog goes here
            break;
        case 2:
            system("cls");
            cout << "You chose to Borrow Book.\n";
            borrowBook(user);
            // Logic for borrowing a book goes here
            break;
        case 3:
            system("cls");
            cout << "You chose to View Borrowed Books.\n";
            viewBorrowedBooks(user);
            break;
        case 4:
            system("cls");
            cout << "You chose to Submit Review.\n";
            submitReview(user);
            break;
        case 5:
            system("cls");
            cout << "You chose to Logout.\n";
            // Logic for logging out goes here
            return;
            break;
        default:
            system("cls");
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }

}

void visitor() {
    int choiceee;
    string bookTitle;
    while (true) {

        cout << "\n=== Visitor Dashboard ===\n";
        cout << "1. Search Catalog\n";
        cout << "2. View Book Details\n";
        cout << "3. Explore Library Events\n";
        cout << "4. Sign Up / Login\n";
        cout << "5. Exit \n";
        cout << "Enter your choice: ";
        cin >> choiceee;
        cin.ignore(); // to avoid input issues after cin

        switch (choiceee) {
        case 1:
            system("cls");
            cout << "You chose to Search Catalog.\n";
            searchCatalogg();
            break;
        case 2:
            system("cls");
            cout << "You chose to View Book Details.\n";
            displayCatalog();  // Or viewBookDetails() if you renamed it
            break;
        case 3:
            system("cls");
            cout << "You chose to Explore Library Events.\n";
            inorderTraversal(eventRoot);
            break;
        case 4:
            system("cls");
            cout << "You chose to Sign Up / Login.\n";
            mainMenu();
            break;
        case 5:
            system("cls");
            cout << "   Exit...\n";
            exit(0);  // Exit
        default:
            system("cls");
            cout << "Invalid choice. Please try again.\n";
        }
    }
}
// Function to insert a new book (admin only)
void insertNewBook(User* user) {
    string title, author, genre, ISBN;
    bool isAvailable;
    char choice;

    cout << "\n=== Insert New Book (Admin) ===\n";

    do {
        cout << "Enter book title: ";
        cin.ignore(); // Clear input buffer before first getline
        getline(cin, title);
        cout << "Enter author: ";
        getline(cin, author);
        cout << "Enter genre: ";
        getline(cin, genre);
        cout << "Enter ISBN: ";
        getline(cin, ISBN);
        cout << "Enter availability (1 for available, 0 for unavailable): ";
        cin >> isAvailable;

        // Insert the new book into the catalog
        insertBook(title, author, genre, ISBN, isAvailable);
        cout << "Book has been successfully added to the catalog.\n";

        cout << "Do you want to add another book? (Y/N): ";
        cin >> choice;
        cin.ignore(); // Clear buffer again for next getline
        choice = tolower(choice); // Optional: to handle both 'Y' and 'y'
    } while (choice == 'y');

    cout << "Exiting book insertion.\n";
    showDashboard(user);

}
void mainMenu() {
    int choice;
    while (true) {
        cout << "\n=== Main Menu ===\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Visit as Guest\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";

        cin >> choice;

        switch (choice) {
        case 1:
            system("cls");
            registerUser();
            break;
        case 2:
            system("cls");
            loggedInUser = loginSystem();
            if (loggedInUser != nullptr) {
                showDashboard(loggedInUser); // Your existing user dashboard
            }
            break;
        case 3:
            system("cls");
            visitor();
            break;
        case 4:
            system("cls");
            cout << "Exiting...\n";
            exit(0); // or return if you call this from main
        default:
            system("cls");
            cout << "Invalid choice, please try again.\n";
        }
    }
}
int main() {
    int choice;

    // Insert some books into the catalog (Linked List)
    insertBook("The Great Gatsby", "F. Scott Fitzgerald", "Fiction", "9780743273565", true);
    insertBook("To Kill a Mockingbird", "Harper Lee", "Fiction", "9780061120084", true);
    insertBook("1984", "George Orwell", "Dystopian", "9780451524935", false);

    mainMenu();
    // Main Menu 

    return 0;
}