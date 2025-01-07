#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define MAX_TRANSACTIONS 100
#define MAX_USERS 100

struct Date
{
    int month, day, year;
};

struct Transaction
{
    char transferId[10];
    char receivingId[10];
    double amount;
    char type[10];
    char message[50];
    struct Date transactionDate;
};

typedef struct
{
    char userId[10];
    char username[20];
    bool status;
    struct Transaction transactionHistory[MAX_TRANSACTIONS];
    int transactionCount;
    char password[10];
} AccountInfo;

typedef struct
{
    char userId[10];
    char name[20];
    struct Date dateOfBirth;
    bool gender;
    char phone[11];
    char email[100];
    AccountInfo account;
} User;

void disableEcho()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void enableEcho()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void inputPassword(char *password, size_t size)
{
    disableEcho();
    fgets(password, size, stdin);
    password[strcspn(password, "\n")] = 0;
    enableEcho();
    printf("\n");
}

bool checkLogin(User users[], int n, char *username, char *password)
{
    for (int i = 0; i < n; i++)
    {
        if (strcmp(users[i].account.username, username) == 0 || strcmp(users[i].email, username) == 0)
        {
            if (!users[i].account.status)
            {
                printf("Account is locked.\n");
                return false;
            }
            if (strcmp(users[i].account.password, password) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

void loginUser(User users[], int n) {
    char username[20];
    char password[10];
    while (1) {
        printf("\t   LOGIN\n");
        printf("=====================\n");
        printf("Enter Username (ID or Email): ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = 0;

        printf("Enter Password: ");
        inputPassword(password, sizeof(password));

        if (checkLogin(users, n, username, password)) {
            printf("Login successful.\n");
            break;
        } else {
            printf("Invalid username or password. Please try again.\n");
        }
    }
}

void saveFile(User users[], int n)
{
    FILE *f = fopen("users.bin", "wb");
    if (f == NULL)
    {
        perror("Error opening file for writing");
        return;
    }

    fwrite(&n, sizeof(int), 1, f);
    for (int i = 0; i < n; i++)
    {
        fwrite(&users[i], sizeof(User), 1, f);
    }

    fclose(f);
    printf("\nSaved file successfully\n");
}

void loadFile(User users[], int *n)
{
    FILE *f = fopen("users.bin", "rb");
    if (f == NULL)
    {
        printf("\nError opening file for reading.\n");
        return;
    }

    fread(n, sizeof(int), 1, f);
    fread(users, sizeof(User), *n, f);

    fclose(f);
    printf("\nData loaded successfully.\n");
}

void isValidDate(struct Date date)
{
    if (date.year < 1900 || date.year > 2100)
    {
        printf("\nError: Invalid year.\n");
        return;
    }
    if (date.month < 1 || date.month > 12)
    {
        printf("\nError: Invalid month.\n");
        return;
    }
    if (date.day < 1 || date.day > 31)
    {
        printf("\nError: Invalid day.\n");
        return;
    }

    if (date.month == 2)
    {
        bool isLeapYear = (date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0);
        if (date.day > (isLeapYear ? 29 : 28))
        {
            printf("\nError: Invalid day for February.\n");
            return;
        }
    }

    if (date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11)
    {
        if (date.day > 30)
        {
            printf("\nError: Invalid day for this month.\n");
            return;
        }
    }
}

void checkDuplicate(User users[], int n, char key[], char value[])
{
    for (int i = 0; i < n; i++)
    {
        if ((strcmp(key, "userId") == 0 && strcmp(users[i].userId, value) == 0) ||
            (strcmp(key, "phone") == 0 && strcmp(users[i].phone, value) == 0) ||
            (strcmp(key, "email") == 0 && strcmp(users[i].email, value) == 0) ||
            (strcmp(key, "username") == 0 && strcmp(users[i].account.username, value) == 0))
        {
            printf("\nError: Duplicate %s found.\n", key);
            return;
        }
    }
}

void inputAccount(User users[], int n, User u, AccountInfo *acc)
{
    getchar();
    do
    {
        printf("\nEnter Username: ");
        fgets(acc->username, sizeof(acc->username), stdin);
        acc->username[strcspn(acc->username, "\n")] = 0;
        if (strlen(acc->username) == 0 || strlen(acc->username) >= sizeof(acc->username))
        {
            printf("\nError: Invalid Username length");
        }
        else
        {
            checkDuplicate(users, n, "username", acc->username);
            break;
        }
    } while (1);

    strcpy(acc->userId, u.userId);
    acc->status = true;
    acc->transactionCount = 0;
}

void inputUser(User users[], int n, User *u)
{
    getchar();
    do
    {
        printf("\nEnter user ID: ");
        fgets(u->userId, sizeof(u->userId), stdin);
        u->userId[strcspn(u->userId, "\n")] = 0;
        if (strlen(u->userId) == 0 || strlen(u->userId) >= sizeof(u->userId))
        {
            printf("\nError: Invalid ID length");
        }
        else
        {
            checkDuplicate(users, n, "userId", u->userId);
            break;
        }
    } while (1);

    do
    {
        printf("\nEnter name: ");
        fgets(u->name, sizeof(u->name), stdin);
        u->name[strcspn(u->name, "\n")] = 0;
        if (strlen(u->name) == 0)
        {
            printf("\nError: Invalid name length");
        }
        else
        {
            break;
        }
    } while (1);

    do
    {
        printf("\nEnter birthday (dd/MM/yyyy): ");
        scanf("%d/%d/%d", &u->dateOfBirth.day, &u->dateOfBirth.month, &u->dateOfBirth.year);
        isValidDate(u->dateOfBirth);
        break;
    } while (1);

    int gender;
    while (1)
    {
        printf("\nEnter gender (1 - Male; 0 - Female): ");
        scanf("%d", &gender);
        if (gender != 0 && gender != 1)
        {
            printf("\nError: Invalid input. Please choose 1 or 0");
        }
        else
        {
            u->gender = gender == 1;
            break;
        }
    }

    do
    {
        printf("\nEnter phone: ");
        scanf("%s", u->phone);
        if (strlen(u->phone) != 10)
        {
            printf("\nError: Phone number must be 10 digits");
        }
        else
        {
            checkDuplicate(users, n, "phone", u->phone);
            break;
        }
    } while (1);

    do
    {
        printf("\nEnter email: ");
        scanf("%s", u->email);
        if (strlen(u->email) == 0 || strlen(u->email) >= sizeof(u->email))
        {
            printf("\nError: Invalid email length");
        }
        else
        {
            checkDuplicate(users, n, "email", u->email);
            break;
        }
    } while (1);

    inputAccount(users, n, *u, &u->account);
    printf("\nUser added successfully!\n");
}

void displayTransactionHistory(AccountInfo account)
{
    printf("\n| Transfer ID | Receiving ID | Amount   | Type       | Message          | Date       |\n");
    printf("|-------------|--------------|----------|------------|------------------|------------|\n");
    for (int i = 0; i < account.transactionCount; i++)
    {
        struct Transaction t = account.transactionHistory[i];
        printf("| %-11s | %-12s | %-8.2f | %-10s | %-16s | %02d/%02d/%04d |\n",
               t.transferId, t.receivingId, t.amount, t.type, t.message,
               t.transactionDate.day, t.transactionDate.month, t.transactionDate.year);
    }
}

void viewUserDetails(User users[], int n)
{
    char searchKey[20];
    printf("\nEnter the user ID or name to view details: ");
    getchar();
    fgets(searchKey, sizeof(searchKey), stdin);
    searchKey[strcspn(searchKey, "\n")] = 0;

    for (int i = 0; i < n; i++)
    {
        if (strcmp(users[i].userId, searchKey) == 0 || strcmp(users[i].name, searchKey) == 0)
        {
            printf("\nUser Details:\n");
            printf("ID: %s\n", users[i].userId);
            printf("Name: %s\n", users[i].name);
            printf("Date of Birth: %02d/%02d/%04d\n", users[i].dateOfBirth.day, users[i].dateOfBirth.month, users[i].dateOfBirth.year);
            printf("Gender: %s\n", users[i].gender ? "Male" : "Female");
            printf("Phone: %s\n", users[i].phone);
            printf("Email: %s\n", users[i].email);
            printf("Account Status: %s\n", users[i].account.status ? "Open" : "Locked");
            displayTransactionHistory(users[i].account);
            return;
        }
    }
    printf("\nError: User ID or name not found!\n");
}

void printUserList(User users[], int n)
{
    printf("| ID         | Name               | Email              | Phone      | Date Of Birth | Gender  | Status |\n");
    printf("|------------|--------------------|--------------------|------------|---------------|---------|--------|\n");
    for (int i = 0; i < n; i++)
    {
        printf("| %-10s | %-18s | %-18s | %-10s | %02d/%02d/%04d    | %-7s | %-6s |\n",
               users[i].userId,
               users[i].name,
               users[i].email,
               users[i].phone,
               users[i].dateOfBirth.day,
               users[i].dateOfBirth.month,
               users[i].dateOfBirth.year,
               users[i].gender ? "Male" : "Female",
               users[i].account.status ? "Open" : "Lock");
    }
}

void toggleUserLock(User users[], int n)
{
    char userId[10];
    printf("\nEnter the user ID to lock/unlock: ");
    getchar();
    fgets(userId, sizeof(userId), stdin);
    userId[strcspn(userId, "\n")] = 0;

    for (int i = 0; i < n; i++)
    {
        if (strcmp(users[i].userId, userId) == 0)
        {
            printf("\nUser Found!\n");
            if (users[i].account.status)
            {
                printf("The user is currently UNLOCKED.\n");
                printf("Do you want to LOCK this user? (yes/no): ");
            }
            else
            {
                printf("The user is currently LOCKED.\n");
                printf("Do you want to UNLOCK this user? (yes/no): ");
            }

            char response[10];
            fgets(response, sizeof(response), stdin);
            response[strcspn(response, "\n")] = 0;

            if (strcmp(response, "yes") == 0)
            {
                users[i].account.status = !users[i].account.status;
                printf("\nThe user's status has been successfully updated.\n");
                printf("New status: %s\n", users[i].account.status ? "UNLOCKED" : "LOCKED");
            }
            else
            {
                printf("\nNo changes made to the user's status.\n");
            }
            return;
        }
    }

    printf("\nError: User ID not found!\n");
}

void sortUsers(User users[], int n, int criteria)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            bool condition = false;
            switch (criteria)
            {
            case 1:
                condition = strcmp(users[i].name, users[j].name) > 0;
                break;
            case 2:
                if (users[i].dateOfBirth.year != users[j].dateOfBirth.year)
                {
                    condition = users[i].dateOfBirth.year > users[j].dateOfBirth.year;
                }
                else if (users[i].dateOfBirth.month != users[j].dateOfBirth.month)
                {
                    condition = users[i].dateOfBirth.month > users[j].dateOfBirth.month;
                }
                else
                {
                    condition = users[i].dateOfBirth.day > users[j].dateOfBirth.day;
                }
                break;
            case 3:
                condition = users[i].account.status < users[j].account.status;
                break;
            }

            if (condition)
            {
                User temp = users[i];
                users[i] = users[j];
                users[j] = temp;
            }
        }
    }
}

void searchUserByName(User users[], int n)
{
    char searchName[20];
    printf("\nEnter the name to search: ");
    getchar();
    fgets(searchName, sizeof(searchName), stdin);
    searchName[strcspn(searchName, "\n")] = 0;

    bool found = false;
    printf("\n| ID         | Name               | Email              | Phone      | Date Of Birth | Gender  | Status |\n");
    printf("|------------|--------------------|--------------------|------------|---------------|---------|--------|\n");
    for (int i = 0; i < n; i++)
    {
        if (strstr(users[i].name, searchName) != NULL)
        {
            printf("| %-10s | %-18s | %-18s | %-10s | %02d/%02d/%04d    | %-7s | %-6s |\n",
                   users[i].userId,
                   users[i].name,
                   users[i].email,
                   users[i].phone,
                   users[i].dateOfBirth.day,
                   users[i].dateOfBirth.month,
                   users[i].dateOfBirth.year,
                   users[i].gender ? "Male" : "Female",
                   users[i].account.status ? "Open" : "Lock");
            found = true;
        }
    }

    if (!found)
    {
        printf("\nNo users found with the given name.\n");
    }
}

void menuSortUsers(User users[], int n)
{
    int choice;
    do
    {
        printf("\n***** Sort Users *****\n");
        printf("[1] Sort by Name\n");
        printf("[2] Sort by Date of Birth\n");
        printf("[3] Sort by Account Status\n");
        printf("[0] Back to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice >= 1 && choice <= 3)
        {
            sortUsers(users, n, choice);
            printf("\nUsers sorted successfully!\n");
            printUserList(users, n);
        }
        else if (choice != 0)
        {
            printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

void menuUserManagement()
{
    printf("***** Bank Management System *****\n");
    printf("\t    CHOOSE YOUR ROLE\n");
    printf("\t======================\n");
    printf("\t[1] Admin\n");
    printf("\t[2] User\n");
    printf("\t[0] Exit\n");
    printf("\t======================\n");
    printf("Enter your choice: ");
}

void menuAdmin()
{
    printf("***** Admin Menu *****\n");
    printf("\t[1] Add a New User\n");
    printf("\t[2] Show All Users\n");
    printf("\t[3] Show User Details\n");
    printf("\t[4] Lock (Unlock) an User\n");
    printf("\t[5] User Guideline\n");
    printf("\t[6] About us\n");
    printf("\t[0] Exit the Program\n");
    printf("Enter your choice: ");
}

int main()
{
    int chooseRole, chooseAdmin;
    User users[MAX_USERS];
    int n = 0;

    // Load data from file
    loadFile(users, &n);

    do
    {
        menuUserManagement();
        scanf("%d", &chooseRole);
        system("cls");
        switch (chooseRole)
        {
        case 1:
            loginUser(users, n);
            do
            {
                menuAdmin();
                scanf("%d", &chooseAdmin);
                system("cls");
                switch (chooseAdmin)
                {
                case 1:
                    printf("\n***** Add New User *****\n");
                    if (n < MAX_USERS)
                    {
                        inputUser(users, n, &users[n]);
                        n++; // Increase user count
                    }
                    else
                    {
                        printf("\nUser limit reached.\n");
                    }
                    break;
                case 2:
                    printf("\n***** Show All Users *****\n");
                    if (n > 0)
                    {
                        printUserList(users, n);
                    }
                    else
                    {
                        printf("\nNo data.\n");
                    }
                    break;
                case 3:
                    printf("\n***** Show User Details *****\n");
                    viewUserDetails(users, n);
                    break;
                case 4:
                    printf("\n***** Lock (Unlock) a User *****\n");
                    toggleUserLock(users, n);
                    break;
                case 0:
                    printf("\nReturning to main menu...\n");
                    break;
                default:
                    printf("\nInvalid selection.\n");
                    break;
                }
                if (chooseAdmin != 0)
                {
                    printf("\nPress Enter to return to Admin Menu...\n");
                    getchar();
                }
            } while (chooseAdmin != 0);
            break;
        case 2:
            loginUser(users, n);
            // printf("\nUser functionality is not implemented yet.\n");
            // printf("\nPress Enter to return to Main Menu...\n");
            getchar();
            getchar();
            break;
        case 0:
            // Save data to file before exiting
            saveFile(users, n);
            printf("\nExiting program. Goodbye!\n");
            break;
        default:
            printf("\nInvalid role selection.\n");
            break;
        }
    } while (chooseRole != 0);

    return 0;
}
