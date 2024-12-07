#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include <iomanip>
#include <cstdlib>
#include <ctime> 
using namespace std;
//We used this function to clear the screen and return to main menu
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// then we will encrypt the text with password 
string encryptDecrypt(string text, char key = 'K') {
    for (char &c : text) {
        c ^= key;
    }
    return text;
}
// We used this function for time settings 
string getCurrentTimestamp() {
    time_t now = time(0);          // This gets the current time of our system
    tm *ltm = localtime(&now);     // This converts time in local time
// Here we will convert that universal time in PAKISTAN time
    ltm->tm_hour += 0;
    if (ltm->tm_hour >= 24) {
        ltm->tm_hour -= 24;  // It Adjusts the Overflow of 24 Hours
    }
// Here we coming to change the time in 12 hour format
    int hour = ltm->tm_hour;
    string am_pm = "AM";
    if (hour >= 12) {
        am_pm = "PM";
        if (hour > 12) {
            hour -= 12; 
        }
    } else if (hour == 0) {
        hour = 12; // for midnight correction 
    }

    // Here we convert that above time into string format so that can be separated easily
    string timestamp = to_string(1900 + ltm->tm_year) + "-" +
                       to_string(1 + ltm->tm_mon) + "-" +
                       to_string(ltm->tm_mday) + " " +
                       to_string(hour) + ":" +
                       to_string(1 + ltm->tm_min) + ":" +
                       to_string(1 + ltm->tm_sec) + " " +
                       am_pm;

    return timestamp;
}

class Note {
private:
    string title;
    string content;
    string category;
    string tags;
    string priority;
    string timestamp;  
    bool isEncrypted;
    string password;

public:
    Note(string t, string c, string cat, string tag, string prio, bool enc = false, string pass = "")
        : title(t), content(c), category(cat), tags(tag), priority(prio), isEncrypted(enc), password(pass) {
        timestamp = getCurrentTimestamp();
    }
    void displayNote(bool decrypt = false) const {
    if (!password.empty()) {  // Check if the note is password-protected
        string enteredPassword;
        cout << "\nThis note is password-protected. Please enter the password: ";
        cin >> enteredPassword;
        if (enteredPassword != password) {
            cout << "Incorrect Password. Cannot display the note." << endl;
            return;
        }
    }

    string displayContent = (decrypt && isEncrypted) ? encryptDecrypt(content) : content;
    cout << "\nTitle       : " << title << endl;
    cout << "Content     : " << displayContent << endl;
    cout << "Category    : " << category << endl;
    cout << "Tags        : " << tags << endl;
    cout << "Priority    : " << priority << endl;
    cout << "Timestamp   : " << timestamp << endl;
    cout << "\nPress Enter to return to the main menu...";
    cin.ignore();
    cin.get();
}


    void editContent(string newContent) {
        content = newContent;
        timestamp = getCurrentTimestamp();
    }

    void updatePriority(string newPriority) {
        priority = newPriority;
        timestamp = getCurrentTimestamp();
    }

    void toggleEncryption() {
        content = encryptDecrypt(content);
        isEncrypted = !isEncrypted;
    }

    void setPassword(string pass) {
        password = pass;
    }

    string getTitle() const { return title; }
    string getContent() const { return content; }
    string getCategory() const { return category; }
    string getTags() const { return tags; }
    string getPriority() const { return priority; }
    string getTimestamp() const { return timestamp; }
    bool getEncryptionStatus() const { return isEncrypted; }
    string getPassword() const { return password; }
};
// class for sticky notes section 
class StickyNote {
private:
    string title;
    string content;

public:
    StickyNote(string t, string c) : title(t), content(c) {}

    void displayStickyNote() const {
        cout << "Title   : " << title << "\nContent : " << content << "\n";
    }

    void editStickyNote(string newContent) {
        content = newContent;
    }

    string getTitle() const { return title; }
    string getContent() const { return content; }
};
// Here we declare another class for Notebook
class Notebook {
private:
    vector<Note> notes;
    vector<Note> recycleBin;
    vector<StickyNote> stickyNotes;
    stack<vector<Note>> undoStack;
    stack<vector<Note>> redoStack;

public:
    void addNote(Note note) {
        undoStack.push(notes);
        notes.push_back(note);
    }

   void displayAllNotes(bool decrypt = false) const {
    clearScreen();
    if (notes.empty()) {
        cout << "\nNo Notes Available!" << endl;
        return;
    }
    int i = 0;
    while (i < notes.size()) {
        clearScreen();
        cout << "\nYour Notes (Sorted by Priority):\n";
        cout << "=================================\n";
        cout << "[" << i + 1 << "] ";
        notes[i].displayNote(decrypt);
        
        if (i == notes.size() - 1) {
            cout << "\nThis is the last note." << endl;
            cin.ignore();
            cin.get();
            break;  // Stop the loop after the last note
        } else {
            char option;
            cout << "\nPress 'N' for Next Note or 'M' to return to the Main Menu: ";
            cin >> option;
            if (option == 'M' || option == 'm') {
                break;  // Return to the main menu
            } else if (option == 'N' || option == 'n') {
                i++;  // Move to the next note
            } else {
                cout << "\nInvalid option. Returning to the main menu." << endl;
                break;  // Invalid option, return to the main menu
            }
        }
    }
}


 void editNoteContent(int index) {
    if (index >= 0 && index < notes.size()) {
        Note &noteToEdit = notes[index];

        // Check if the note is encrypted
        if (noteToEdit.getEncryptionStatus()) {
            string enteredPassword;
            cout << "\nThis note is password-protected. Please enter the password: ";
            cin >> enteredPassword;

            // Clear input buffer to handle newlines
            cin.ignore();

            // Verify password
            if (enteredPassword != noteToEdit.getPassword()) {
                cout << "Incorrect Password. Returning to main menu." << endl;
                return;
            }
        }

        // Prompt for new content
        string newContent;
        cout << "Enter New Content (Press Enter twice on an empty line to finish):" << endl;
        string line;
        newContent.clear();

        // Collect the new content
        while (true) {
            getline(cin, line);
            if (line.empty()) break;
            newContent += line + '\n';
        }

        // Update the note with the new content
        noteToEdit.editContent(newContent);
        cout << "\nNote Updated Successfully!" << endl;
    } else {
        cout << "\nInvalid Note Index!" << endl;
    }
}



    void deleteNote(int index) {
        if (index >= 0 && index < notes.size()) {
            undoStack.push(notes);
            recycleBin.push_back(notes[index]);
            notes.erase(notes.begin() + index);
            cout << "\nNote Moved To Recycle Bin!" << endl;
        } else {
            cout << "\nInvalid Note Index!" << endl;
        }
    }

    void displayRecycleBin() const {
        clearScreen();
        if (recycleBin.empty()) {
            cout << "\nRecycle Bin is empty!" << endl;
            return;
        }
        cout << "\nRecycle Bin:\n";
        cout << "=================================\n";
        for (size_t i = 0; i < recycleBin.size(); ++i) {
            cout << "[" << i + 1 << "] " << recycleBin[i].getTitle() << endl;
        }
    }

    void restoreFromRecycleBin(int index) {
        if (index >= 0 && index < recycleBin.size()) {
            notes.push_back(recycleBin[index]);
            recycleBin.erase(recycleBin.begin() + index);
            cout << "\nNote Restored Successfully!" << endl;
        } else {
            cout << "\nInvalid recycle bin index!" << endl;
        }
    }

    void searchNotes(string keyword) const {
        clearScreen();
        bool found = false;
        cout << "\nSearch Results:\n";
        cout << "=================================\n";
        for (const auto &note : notes) {
            if (note.getTitle().find(keyword) != string::npos ||
                note.getContent().find(keyword) != string::npos ||
                note.getTags().find(keyword) != string::npos) {
                note.displayNote();
                cout << "---------------------------------" << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "\nNo Notes Found Matching the Keyword!" << endl;
        }
    }

    void undo() {
        if (!undoStack.empty()) {
            redoStack.push(notes);
            notes = undoStack.top();
            undoStack.pop();
            cout << "\nUndo Successful!" << endl;
        } else {
            cout << "\nNo Actions To Undo!" << endl;
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            undoStack.push(notes);
            notes = redoStack.top();
            redoStack.pop();
            cout << "\nRedo Successful!" << endl;
        } else {
            cout << "\nNo Actions To Redo!" << endl;
        }
    }

void saveToFile() const {
    ofstream outFile("notes.txt");
    if (!outFile) {
        cout << "\nError Opening File for Saving!" << endl;
        return;
    }

    for (const auto &note : notes) {
        outFile << note.getTitle() << endl;
        outFile << (note.getEncryptionStatus() ? encryptDecrypt(note.getContent()) : note.getContent()) << endl;
        outFile << note.getCategory() << endl;
        outFile << note.getTags() << endl;
        outFile << note.getPriority() << endl;
        outFile << note.getTimestamp() << endl;
        outFile << note.getEncryptionStatus() << endl;

        // Save the password only if the note is encrypted
        if (note.getEncryptionStatus() && !note.getPassword().empty()) {
            outFile << encryptDecrypt(note.getPassword()) << endl;
        } else {
            outFile << "\n";  // Leave password field empty for non-encrypted notes
        }

        outFile << "---" << endl;  // Delimiter for each note
    }

    outFile.close();
    cout << "\nNotes Saved to File!" << endl;
}


void loadFromFile() {
    ifstream inFile("notes.txt");
    if (!inFile) {
        cout << "\nError Opening File for Loading!" << endl;
        return;
    }

    string title, content, category, tags, priority, timestamp, password, delimiter;
    bool isEncrypted;

    while (getline(inFile, title)) {
        if (title.empty()) continue;

        getline(inFile, content);
        getline(inFile, category);
        getline(inFile, tags);
        getline(inFile, priority);
        getline(inFile, timestamp);
        inFile >> isEncrypted;
        inFile.ignore();  // Skip newline after reading the boolean

        if (isEncrypted) {
            getline(inFile, password);  // Only read password if the note is encrypted
            if (!password.empty()) {
                password = encryptDecrypt(password);  // Decrypt the password
            }
        } else {
            password = "";  // Ensure password is empty for non-encrypted notes
        }

        getline(inFile, delimiter);  // Skip delimiter line

        Note note(title, content, category, tags, priority, isEncrypted, password);

        // Only toggle encryption if the note is marked as encrypted in the file
        if (isEncrypted) {
            note.toggleEncryption();  // Restore encryption state
        }

        notes.push_back(note);
    }

    inFile.close();
    cout << "\nNotes Loaded From Saved File!" << endl;
}





    void addStickyNotes(string title, string content) {
        StickyNote newSticky(title, content);
        stickyNotes.push_back(newSticky);
    }

    void displayStickyNotes() const {
        clearScreen();
        if (stickyNotes.empty()) {
            cout << "\nNo Sticky Notes Available!" << endl;
            return;
        }
        cout << "\nYour Sticky Notes:\n";
        cout << "====================\n";
        for (size_t i = 0; i < stickyNotes.size(); ++i) {
            cout << "[" << i + 1 << "] ";
            stickyNotes[i].displayStickyNote();
            cout << "------------------------\n";
        }
    }
};

int main() {
    Notebook notebook;
    int choice;

    do {
        clearScreen();
        cout << "\033[1;37;44m+-------------------------------------------------------------+\033[0m" << endl;
cout << "\033[1;37;44m+                  WELCOME TO DIGITAL NOTEBOOK                +\033[0m" << endl;
cout << "\033[1;37;44m+-------------------------------------------------------------+\033[0m" << endl;
cout << "\033[5;1;32m###############################################################\033[0m" << endl;
cout << "\033[5;1;32m#                                                             #\033[0m" << endl;
cout << "\033[5;1;32m#   1. Create a Note                                          #\033[0m" << endl;
cout << "\033[5;1;32m#   2. Display All Notes                                      #\033[0m" << endl;
cout << "\033[5;1;32m#   3. Edit a Note                                            #\033[0m" << endl;
cout << "\033[5;1;32m#   4. Delete Notes                                           #\033[0m" << endl;
cout << "\033[5;1;32m#   5. Display Recycle Bin                                    #\033[0m" << endl;
cout << "\033[5;1;32m#   6. Restore from Recycle Bin                               #\033[0m" << endl;
cout << "\033[5;1;32m#   7. Search Notes                                           #\033[0m" << endl;
cout << "\033[5;1;32m#   8. Undo                                                   #\033[0m" << endl;
cout << "\033[5;1;32m#   9. Redo                                                   #\033[0m" << endl;
cout << "\033[5;1;32m#  10. Save Notes to Local File                               #\033[0m" << endl;
cout << "\033[5;1;32m#  11. Load from File                                         #\033[0m" << endl;
cout << "\033[5;1;32m#  12. Add Sticky Notes                                       #\033[0m" << endl;
cout << "\033[5;1;32m#  13. Display Sticky Notes                                   #\033[0m" << endl;
cout << "\033[5;1;32m#  14. Exit                                                   #\033[0m" << endl;
cout << "\033[5;1;32m###############################################################\033[0m" << endl;
cout << "\033[1;37;44m+-------------------------------------------------------------+\033[0m" << endl;
cout << "\033[1;37;44m+        Select an Option (1 - 14) :                          +\033[0m" << endl;
cout << "\033[1;37;44m+-------------------------------------------------------------+\033[0m" << endl;

        cin >> choice;

        cin.ignore();

        switch (choice) {
          case 1: {
    string title, content, category, tags, priority;
    bool isEncrypted;
    string pass;

    cout << "\nEnter Title : ";
    getline(cin, title);

    cout << "Enter Content (Press Enter twice on an empty line to finish) :" << endl;
    string line;
    content.clear();
    while (true) {
        getline(cin, line);
        if (line.empty()) break;
        content += line + '\n';
    }

    cout << "Enter Category : ";
    getline(cin, category);
    cout << "Enter Tags : ";
    getline(cin, tags);
    cout << "Enter Priority : ";
    getline(cin, priority);
    cout << "Is this note encrypted (1 for Yes, 0 for No) : ";
    cin >> isEncrypted;
    cin.ignore();

    if (isEncrypted) {
    cout << "Enter Password for Encryption: ";
    getline(cin, pass);
} else {
    pass = ""; // Clear the password for non-encrypted notes
}


    notebook.addNote(Note(title, content, category, tags, priority, isEncrypted, pass));
    break;
}

            
            case 2:
                notebook.displayAllNotes();
                break;
                
			 case 3: {
    int index;
    cout << "\nEnter Note Index to Edit : ";
    cin >> index;
    cin.ignore();  // Ignore the remaining newline
    notebook.editNoteContent(index - 1);  // Pass the note index to edit
    break;
}

            case 4: {
                int index;
                cout << "\nEnter Note Index to Delete : ";
                cin >> index;
                notebook.deleteNote(index - 1);
                break;
            }
            case 5:
                notebook.displayRecycleBin();
                break;
            case 6: {
                int index;
                cout << "\nEnter Recycle Bin Index to Restore : ";
                cin >> index;
                notebook.restoreFromRecycleBin(index - 1);
                break;
            }
            case 7: {
                string keyword;
                cout << "\nEnter Keyword to Search : ";
                cin >> keyword;
                notebook.searchNotes(keyword);
                break;
            }
            case 8:
                notebook.undo();
                break;
            case 9:
                notebook.redo();
                break;
            case 10:
                notebook.saveToFile();
                break;
            case 11:
                notebook.loadFromFile();
                break;
            case 12: {
                string title, content;
                cout << "\nEnter Sticky Note Title: ";
                getline(cin, title);
                cout << "Enter Sticky Note Content: ";
                getline(cin, content);
                notebook.addStickyNotes(title, content);
                break;
            }
            case 13:
                notebook.displayStickyNotes();
                break;
            case 14:
                cout << "\nExiting the application..." << endl;
                break;
            default:
                cout << "\nInvalid choice. Please try again!" << endl;
                break;
        }

        cout << "\nPress Enter to Continue...";
        cin.ignore();
        cin.get();

    } while (choice != 14);

    return 0;
}

