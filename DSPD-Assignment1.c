#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FAMILY 100
#define MAX_USERS 1000
#define MAX_EXPENSES 1000
#define MAX_MEMBERS 4
#define MAX_CATEGORIES 5
#define FAMILY_LIMIT 10
#define INDIVIDUAL_LIMIT 25

// Enum for expense categories
typedef enum {
    RENT = 0,    // R
    UTILITY,     // U
    GROCERY,     // G
    STATIONARY,  // S
    LEISURE      // L
} ExpenseCategory;

// Mapping between category enum and string representation
const char* CATEGORY_NAMES[] = {"R", "U", "G", "S", "L"};
const char* CATEGORY_FULL_NAMES[] = {"Rent", "Utility", "Grocery", "Stationary", "Leisure"};

// Convert string category to enum
ExpenseCategory string_to_category(const char* category_str) {
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        if (strcmp(category_str, CATEGORY_NAMES[i]) == 0) {
            return (ExpenseCategory)i;
        }
    }
    return RENT; // Default to RENT if invalid
}

// Convert enum category to string
const char* category_to_string(ExpenseCategory category) {
    if (category >= 0 && category < MAX_CATEGORIES) {
        return CATEGORY_NAMES[category];
    }
    return CATEGORY_NAMES[0]; // Default to "R" if invalid
}

struct Expense {
    int expenseID;
    int userID;
    ExpenseCategory category;  // Using enum instead of string
    float exp_amt;
    char dateOfExpense[11];  // Increased to 11 for null terminator
};

struct Individual {
    int userID;
    char userName[50];
    float Income;
    int familyID;
};

struct Family {
    int familyID;
    char familyName[50];
    int userIDs[MAX_MEMBERS];  // Limited to MAX_MEMBERS
    int numberFamily;
    float Tot_Income;
    float Mon_Expense;
};

int Ucnt = 0;
int exp_cnt = 0;
int fam_cnt = 0;

struct Individual users[MAX_USERS];
struct Expense exps[MAX_EXPENSES];
struct Family families[MAX_FAMILY];

void Initialize_Individual() {
    for (int i = 0; i < MAX_USERS; i++) {
        users[i].userID = 0;
        users[i].userName[0] = '\0';
        users[i].Income = 0;
        users[i].familyID = 0;
    }
}

void Initialize_Expense() {
    for (int i = 0; i < MAX_EXPENSES; i++) {
        exps[i].expenseID = 0;
        exps[i].userID = 0;
        exps[i].category = RENT;
        exps[i].exp_amt = 0;
        exps[i].dateOfExpense[0] = '\0';
    }
}

void Initialize_Family() {
    for (int i = 0; i < MAX_FAMILY; i++) {
        families[i].familyID = 0;
        families[i].familyName[0] = '\0';
        
        for (int j = 0; j < MAX_MEMBERS; j++) {
            families[i].userIDs[j] = 0;
        }
        families[i].numberFamily = 0;
        families[i].Tot_Income = 0;
        families[i].Mon_Expense = 0;
    }
}

void Save_users_data() {
    FILE *fout = fopen("users.txt", "w");
    if (fout == NULL) {
        printf("Error opening users.txt for writing.\n");
        return;
    }

    for (int i = 0; i < Ucnt; i++) {
        fprintf(fout, "%d %s %.2f %d\n", 
                users[i].userID,
                users[i].userName,
                users[i].Income,
                users[i].familyID);
    }

    fclose(fout);
}

void Save_expenses_data() {
    FILE *fout = fopen("expenses.txt", "w");
    if (fout == NULL) {
        printf("Error opening expenses.txt for writing.\n");
        return;
    }

    for (int i = 0; i < exp_cnt; i++) {
        fprintf(fout, "%d %d %s %.2f %s\n", 
                exps[i].expenseID,
                exps[i].userID,
                category_to_string(exps[i].category),
                exps[i].exp_amt,
                exps[i].dateOfExpense);
    }

    fclose(fout);
}

void Save_families_data() {
    FILE *fout = fopen("families.txt", "w");
    if (fout == NULL) {
        printf("Error opening families.txt for writing.\n");
        return;
    }

    for (int i = 0; i < fam_cnt; i++) {
        fprintf(fout, "%d %s %d", 
                families[i].familyID,
                families[i].familyName,
                families[i].numberFamily);
        
        for (int j = 0; j < families[i].numberFamily; j++) {
            fprintf(fout, " %d", families[i].userIDs[j]);
        }
        
        fprintf(fout, " %.2f %.2f\n", 
                families[i].Tot_Income,
                families[i].Mon_Expense);
    }

    fclose(fout);
}

void Input_userdata() {
    FILE *fin = fopen("users.txt", "r");
    if (fin == NULL) {
        printf("Warning: users.txt not found. Starting with empty user database.\n");
        return;
    }

    while (fscanf(fin, "%d %s %f %d",
                &users[Ucnt].userID,
                users[Ucnt].userName,
                &users[Ucnt].Income,
                &users[Ucnt].familyID) != EOF) {
        Ucnt++;
        if (Ucnt >= MAX_USERS) {
            printf("Warning: Maximum user limit reached. Some users not loaded.\n");
            break;
        }
    }

    printf("Loaded %d users from file.\n", Ucnt);
    fclose(fin);
}

void Input_expdata() {
    FILE *fout = fopen("expenses.txt", "r");
    if (fout == NULL) {
        printf("Warning: expenses.txt not found. Starting with empty expense database.\n");
        return;
    }

    char category_str[10];
    while (fscanf(fout, "%d %d %s %f %s",
                &exps[exp_cnt].expenseID,
                &exps[exp_cnt].userID,
                category_str,
                &exps[exp_cnt].exp_amt,
                exps[exp_cnt].dateOfExpense) != EOF) {
        // Convert string category to enum
        exps[exp_cnt].category = string_to_category(category_str);
        
        exp_cnt++;
        if (exp_cnt >= MAX_EXPENSES) {
            printf("Warning: Maximum expense limit reached. Some expenses not loaded.\n");
            break;
        }
    }

    printf("Loaded %d expenses from file.\n", exp_cnt);
    fclose(fout);
}

void Input_family_data() {
    FILE *fin = fopen("families.txt", "r");
    if (fin == NULL) {
        printf("Warning: families.txt not found. Starting with empty family database.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fin) != NULL && fam_cnt < MAX_FAMILY) {
        char *token = strtok(line, " ");
        if (token == NULL) continue;
        
        families[fam_cnt].familyID = atoi(token);
        
        token = strtok(NULL, " ");
        if (token == NULL) continue;
        strncpy(families[fam_cnt].familyName, token, 49);
        families[fam_cnt].familyName[49] = '\0';
        
        token = strtok(NULL, " ");
        if (token == NULL) continue;
        families[fam_cnt].numberFamily = atoi(token);
        
        for (int i = 0; i < families[fam_cnt].numberFamily && i < MAX_MEMBERS; i++) {
            token = strtok(NULL, " ");
            if (token == NULL) break;
            families[fam_cnt].userIDs[i] = atoi(token);
        }
        
        token = strtok(NULL, " ");
        if (token == NULL) continue;
        families[fam_cnt].Tot_Income = atof(token);
        
        token = strtok(NULL, " ");
        if (token == NULL) continue;
        families[fam_cnt].Mon_Expense = atof(token);
        
        fam_cnt++;
    }

    printf("Loaded %d families from file.\n", fam_cnt);
    fclose(fin);
}

void Bubble_sort() {
    for (int i = 0; i < exp_cnt - 1; i++) {
        int flag = 0;
        for (int j = 0; j < exp_cnt - i - 1; j++) {
            if (exps[j].userID > exps[j + 1].userID ||
                (exps[j].userID == exps[j + 1].userID && exps[j].expenseID > exps[j + 1].expenseID)) {
                struct Expense temp = exps[j];
                exps[j] = exps[j + 1];
                exps[j + 1] = temp;
                flag = 1;
            }
        }
        if (!flag) {
            return;
        }
    }
}

void Sort_users() {
    for (int i = 0; i < Ucnt - 1; i++) {
        for (int j = 0; j < Ucnt - i - 1; j++) {
            if (users[j].userID > users[j + 1].userID) {
                struct Individual temp = users[j];
                users[j] = users[j + 1];
                users[j + 1] = temp;
            }
        }
    }
}

void AddUser() {
    if (Ucnt >= MAX_USERS) {
        printf("User limit reached. Cannot add more users.\n");
        return;
    }

    struct Individual newUser;
    printf("Enter User ID: ");
    if (scanf("%d", &newUser.userID) != 1) {
        printf("Invalid input for User ID.\n");
        return;
    }
    
    // Check if user ID already exists
    for (int i = 0; i < Ucnt; i++) {
        if (users[i].userID == newUser.userID) {
            printf("User ID already exists. Please choose a different ID.\n");
            return;
        }
    }

    printf("Enter User Name: ");
    scanf("%49s", newUser.userName);

    printf("Enter Income: ");
    if (scanf("%f", &newUser.Income) != 1) {
        printf("Invalid input for Income.\n");
        return;
    }
    
    // Initialize familyID to 0 (not part of any family yet)
    newUser.familyID = 0;
    
    // Insert user in sorted position by user ID
    int i;
    for (i = Ucnt - 1; i >= 0 && users[i].userID > newUser.userID; i--) {
        users[i + 1] = users[i];
    }
    users[i + 1] = newUser;
    Ucnt++;
    
    // Save updated users data
    Save_users_data();
    printf("User added successfully.\n");
}

void Print_Udata() {
    if (Ucnt == 0) {
        printf("No users available.\n");
        return;
    }
    
    printf("Users List:\n");
    printf("-----------------------------------------------------------\n");
    printf("| User ID |     Name     |   Income   | Family ID |\n");
    printf("-----------------------------------------------------------\n");
    
    for (int i = 0; i < Ucnt; i++) {
        printf("| %-7d | %-12s | %-10.2f | %-9d |\n",
            users[i].userID,
            users[i].userName,
            users[i].Income,
            users[i].familyID);
    }
    printf("-----------------------------------------------------------\n");
}

void Add_exp() {
    if (exp_cnt >= MAX_EXPENSES) {
        printf("Expense limit reached.\n");
        return;
    }

    struct Expense newExp;
    printf("Enter User ID: ");
    if (scanf("%d", &newExp.userID) != 1) {
        printf("Invalid input. Please enter a valid User ID.\n");
        return;
    }
    
    // Check if user exists
    int found = 0;
    for (int i = 0; i < Ucnt; i++) {
        if (users[i].userID == newExp.userID) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Error: User ID %d does not exist. Please add a valid user first.\n", newExp.userID);
        return;
    }

    printf("Enter Expense ID: ");
    if (scanf("%d", &newExp.expenseID) != 1) {
        printf("Invalid input. Please enter a valid Expense ID.\n");
        return;
    }
    
    // Check if expense ID already exists
    for (int i = 0; i < exp_cnt; i++) {
        if (exps[i].expenseID == newExp.expenseID) {
            printf("Error: Expense ID %d already exists.\n", newExp.expenseID);
            return;
        }
    }

    printf("Enter Category (R=Rent, U=Utility, G=Grocery, S=Stationary, L=Leisure): ");
    char cat_str[2];
    scanf("%1s", cat_str);
    
    // Validate category
    if (strlen(cat_str) != 1 || !strchr("RUGSL", cat_str[0])) {
        printf("Invalid category! Please use R, U, G, S, or L.\n");
        return;
    }
    
    // Convert to enum
    newExp.category = string_to_category(cat_str);

    printf("Enter Amount: ");
    if (scanf("%f", &newExp.exp_amt) != 1) {
        printf("Invalid input. Please enter a valid amount.\n");
        return;
    }

    printf("Enter Date (DD-MM-YYYY): ");
    scanf("%10s", newExp.dateOfExpense);
    if (strlen(newExp.dateOfExpense) != 10 || newExp.dateOfExpense[2] != '-' || newExp.dateOfExpense[5] != '-') {
        printf("Invalid date format. Please enter in DD-MM-YYYY format.\n");
        return;
    }

    // Add the expense
    exps[exp_cnt++] = newExp;
    
    // Update family expense if user belongs to a family
    int familyID = 0;
    for (int i = 0; i < Ucnt; i++) {
        if (users[i].userID == newExp.userID) {
            familyID = users[i].familyID;
            break;
        }
    }
    
    if (familyID > 0) {
        for (int i = 0; i < fam_cnt; i++) {
            if (families[i].familyID == familyID) {
                families[i].Mon_Expense += newExp.exp_amt;
                break;
            }
        }
        // Save family data as it's been updated
        Save_families_data();
    }
    
    // Sort expenses
    Bubble_sort();
    
    // Save updated expense data
    Save_expenses_data();
    printf("Expense added successfully.\n");
}

void Print_struct() {
    Bubble_sort();  
    
    if (exp_cnt == 0) {
        printf("No expenses available.\n");
        return;
    }
    
    printf("Expenses List:\n");
    printf("-------------------------------------------------------------------------\n");
    printf("| Exp ID | User ID |  Category  |  Amount  |    Date    |\n");
    printf("-------------------------------------------------------------------------\n");
    
    for (int i = 0; i < exp_cnt; i++) {
        printf("| %-6d | %-7d | %-10s | %-8.2f | %-10s |\n",
            exps[i].expenseID,
            exps[i].userID,
            CATEGORY_FULL_NAMES[exps[i].category],
            exps[i].exp_amt,
            exps[i].dateOfExpense);
    }
    printf("-------------------------------------------------------------------------\n");
}

void UpdateFamilyTotals(int familyID) {
    for (int i = 0; i < fam_cnt; i++) {
        if (families[i].familyID == familyID) {
            // Reset totals
            families[i].Tot_Income = 0.0;
            families[i].Mon_Expense = 0.0;

            // Recalculate income
            for (int j = 0; j < families[i].numberFamily; j++) {
                int memberID = families[i].userIDs[j];
                for (int k = 0; k < Ucnt; k++) {
                    if (users[k].userID == memberID) {
                        families[i].Tot_Income += users[k].Income;
                        break;
                    }
                }
            }

            // Recalculate expenses
            for (int j = 0; j < families[i].numberFamily; j++) {
                int memberID = families[i].userIDs[j];
                for (int k = 0; k < exp_cnt; k++) {
                    if (exps[k].userID == memberID) {
                        families[i].Mon_Expense += exps[k].exp_amt;
                    }
                }
            }
            
            break;
        }
    }
}

int check_month(const char *date, const char *month) {
    return strncmp(date + 3, month, 2) == 0;
}

void CreateFamily() {
    if (fam_cnt >= MAX_FAMILY) {
        printf("Family limit reached. Cannot add more families.\n");
        return;
    }

    struct Family family = {0};
    family.Tot_Income = 0.0;
    family.Mon_Expense = 0.0;

    char month_year[8];
    printf("Enter the month (MM-YYYY): ");
    scanf("%7s", month_year);

    int members;
    printf("Enter the number of family members (1-%d): ", MAX_MEMBERS);
    scanf("%d", &members);
    if (members < 1 || members > MAX_MEMBERS) {
        printf("Invalid number of family members.\n");
        return;
    }

    printf("Enter the Family ID: ");
    if (scanf("%d", &family.familyID) != 1) {
        printf("Invalid input for Family ID.\n");
        return;
    }
    
    // Check if family ID already exists
    for (int i = 0; i < fam_cnt; i++) {
        if (families[i].familyID == family.familyID) {
            printf("Family ID already exists. Please choose a different ID.\n");
            return;
        }
    }

    printf("Enter the Family Name: ");
    scanf("%49s", family.familyName);

    family.numberFamily = members;
    for (int i = 0; i < members; i++) {
        int userID;
        printf("Enter User ID for member %d: ", i + 1);
        if (scanf("%d", &userID) != 1) {
            printf("Invalid input for User ID.\n");
            return;
        }

        // Check if user exists
        int found = 0;
        int userIndex = -1;
        for (int k = 0; k < Ucnt; k++) {
            if (users[k].userID == userID) {
                found = 1;
                userIndex = k;
                break;
            }
        }

        if (!found) {
            printf("User ID %d does not exist. Please enter a valid User ID.\n", userID);
            i--;
            continue;
        }
        
        // Check if user already belongs to a family
        if (users[userIndex].familyID != 0) {
            printf("User ID %d already belongs to family ID %d. Please choose another user.\n", 
                   userID, users[userIndex].familyID);
            i--;
            continue;
        }

        family.userIDs[i] = userID;
        
        // Update user's family ID
        users[userIndex].familyID = family.familyID;
        
        // Add income to family total
        family.Tot_Income += users[userIndex].Income;
        
        // Add user's expenses for the given month to family total
        for (int j = 0; j < exp_cnt; j++) {
            if (exps[j].userID == userID && check_month(exps[j].dateOfExpense, month_year)) {
                family.Mon_Expense += exps[j].exp_amt;
            }
        }
    }

    // Add family to array
    families[fam_cnt++] = family;
    
    // Save updated data
    Save_users_data();
    Save_families_data();
    
    printf("Family created successfully.\n");
}

void Print_family() {
    if (fam_cnt == 0) {
        printf("No families available.\n");
        return;
    }
    
    printf("Families List:\n");
    printf("----------------------------------------------------------------\n");
    printf("| Family ID |    Name    | Members |  Tot Income | Tot Expense |\n");
    printf("----------------------------------------------------------------\n");
    
    for (int i = 0; i < fam_cnt; i++) {
        printf("| %-9d | %-10s | %-7d | %-11.2f | %-11.2f |\n",
            families[i].familyID,
            families[i].familyName,
            families[i].numberFamily,
            families[i].Tot_Income,
            families[i].Mon_Expense);
            
        printf("| Member IDs: ");
        for (int j = 0; j < families[i].numberFamily; j++) {
            printf("%d ", families[i].userIDs[j]);
        }
        printf("\n");
        printf("----------------------------------------------------------------\n");
    }
}

void UpdateOrDeleteIndividualFamilyDetails() {
    int choice;
    printf("1. Update Individual Details\n");
    printf("2. Delete Individual\n");
    printf("3. Update Family Details\n");
    printf("4. Delete Family\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            // Update Individual Details
            int userID;
            printf("Enter User ID to update: ");
            scanf("%d", &userID);
            
            int found = 0;
            for (int i = 0; i < Ucnt; i++) {
                if (users[i].userID == userID) {
                    found = 1;
                    printf("Current Name: %s, Current Income: %.2f\n", 
                           users[i].userName, users[i].Income);
                    
                    printf("Enter new Name: ");
                    scanf("%49s", users[i].userName);
                    
                    float oldIncome = users[i].Income;
                    printf("Enter new Income: ");
                    scanf("%f", &users[i].Income);
                    
                    // Update family totals if user belongs to a family
                    if (users[i].familyID > 0) {
                        for (int j = 0; j < fam_cnt; j++) {
                            if (families[j].familyID == users[i].familyID) {
                                families[j].Tot_Income = families[j].Tot_Income - oldIncome + users[i].Income;
                                break;
                            }
                        }
                    }
                    
                    // Save updated data
                    Save_users_data();
                    Save_families_data();
                    
                    printf("User updated successfully!\n");
                    break;
                }
            }
            
            if (!found) {
                printf("User not found!\n");
            }
            break;
        }
        case 2: {
            // Delete Individual
            int userID;
            printf("Enter User ID to delete: ");
            scanf("%d", &userID);
            
            int found = 0;
            int userIndex = -1;
            for (int i = 0; i < Ucnt; i++) {
                if (users[i].userID == userID) {
                    found = 1;
                    userIndex = i;
                    break;
                }
            }
            
            if (!found) {
                printf("User not found!\n");
                break;
            }
            
            int familyID = users[userIndex].familyID;
            
            // If user belongs to a family
            if (familyID > 0) {
                int familyIndex = -1;
                for (int j = 0; j < fam_cnt; j++) {
                    if (families[j].familyID == familyID) {
                        familyIndex = j;
                        break;
                    }
                }
                
                if (familyIndex != -1) {
                    // If this is the only member in family, delete the family
                    if (families[familyIndex].numberFamily == 1) {
                        // Delete family
                        for (int k = familyIndex; k < fam_cnt - 1; k++) {
                            families[k] = families[k + 1];
                        }
                        fam_cnt--;
                        printf("Associated family deleted as user was the only member.\n");
                    } else {
                        // Remove user from family
                        for (int k = 0; k < families[familyIndex].numberFamily; k++) {
                            if (families[familyIndex].userIDs[k] == userID) {
                                // Shift remaining members
                                for (int l = k; l < families[familyIndex].numberFamily - 1; l++) {
                                    families[familyIndex].userIDs[l] = families[familyIndex].userIDs[l + 1];
                                }
                                families[familyIndex].numberFamily--;
                                
                                // Update family totals
                                UpdateFamilyTotals(familyID);
                                break;
                            }
                        }
                    }
                }
            }
            
            // Delete all expenses associated with this user
            int origExpCount = exp_cnt;
            for (int i = 0; i < exp_cnt; i++) {
                if (exps[i].userID == userID) {
                    // Shift remaining expenses
                    for (int j = i; j < exp_cnt - 1; j++) {
                        exps[j] = exps[j + 1];
                    }
                    exp_cnt--;
                    i--; // Recheck the current index as it now contains next element
                }
            }
            printf("Deleted %d expenses associated with user.\n", origExpCount - exp_cnt);
            
            // Delete the user
            for (int k = userIndex; k < Ucnt - 1; k++) {
                users[k] = users[k + 1];
            }
            Ucnt--;
            
            // Save updated data
            Save_users_data();
            Save_expenses_data();
            Save_families_data();
            
            printf("User deleted successfully!\n");
            break;
        }
        case 3: {
            // Update Family Details
            int familyID;
            printf("Enter Family ID to update: ");
            scanf("%d", &familyID);
            
            int found = 0;
            for (int i = 0; i < fam_cnt; i++) {
                if (families[i].familyID == familyID) {
                    found = 1;
                    printf("Current Family Name: %s\n", families[i].familyName);
                    printf("Enter new Family Name: ");
                    scanf("%49s", families[i].familyName);
                    
                    // Save updated data
                    Save_families_data();
                    
                    printf("Family details updated successfully!\n");
                    break;
                }
            }
            
            if (!found) {
                printf("Family not found!\n");
            }
            break;
        }
        case 4: {
            // Delete Family
            int familyID;
            printf("Enter Family ID to delete: ");
            scanf("%d", &familyID);
            
            int found = 0;
            int familyIndex = -1;
            for (int i = 0; i < fam_cnt; i++) {
                if (families[i].familyID == familyID) {
                    found = 1;
                    familyIndex = i;
                    break;
                }
            }
            
            if (!found) {
                printf("Family not found!\n");
                break;
            }
            
            // Update users to no longer belong to this family
            for (int i = 0; i < families[familyIndex].numberFamily; i++) {
                int memberID = families[familyIndex].userIDs[i];
                for (int j = 0; j < Ucnt; j++) {
                    if (users[j].userID == memberID) {
                        users[j].familyID = 0; // Set to no family
                        break;
                    }
                }
            }
            
            // Delete the family
            for (int j = familyIndex; j < fam_cnt - 1; j++) {
                families[j] = families[j + 1];
            }
            fam_cnt--;
            
            // Save updated data
            Save_users_data();
            Save_families_data();
            
            printf("Family deleted successfully!\n");
            break;
        }
        default:
            printf("Invalid choice!\n");
    }
}

void update_delete_expenses(int userID, float new_exp_amt) {
    printf("Expenses for User ID %d:\n", userID);
    printf("-----------------------------------------------------------\n");
    printf("| Index | Exp ID | Category |  Amount  |    Date    |\n");
    printf("-----------------------------------------------------------\n");
    // Display all expenses for the user
    int count = 0;
    for (int i = 0; i < exp_cnt; i++) {
        if (exps[i].userID == userID) {
            printf("| %-5d | %-6d | %-8s | %-8.2f | %-10s |\n",
                   count++,
                   exps[i].expenseID,
                   CATEGORY_FULL_NAMES[exps[i].category],
                   exps[i].exp_amt,
                   exps[i].dateOfExpense);
        }
    }
    printf("-----------------------------------------------------------\n");

    if (count == 0) {
        printf("No expenses found for this user.\n");
        return;
    }

    int choice;
    printf("1. Update Expense\n");
    printf("2. Delete Expense\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    int expIndex;
    printf("Enter Index of expense to modify: ");
    scanf("%d", &expIndex);

    if (expIndex < 0 || expIndex >= count) {
        printf("Invalid index!\n");
        return;
    }

    // Find the actual expense index in the exps array
    int actualIndex = -1;
    count = 0;
    for (int i = 0; i < exp_cnt; i++) {
        if (exps[i].userID == userID) {
            if (count == expIndex) {
                actualIndex = i;
                break;
            }
            count++;
        }
    }

    if (actualIndex == -1) {
        printf("Error finding expense.\n");
        return;
    }

    // Get the family ID of the user for updating family expense later
    int familyID = 0;
    for (int i = 0; i < Ucnt; i++) {
        if (users[i].userID == userID) {
            familyID = users[i].familyID;
            break;
        }
    }

    switch (choice) {
        case 1: {
            // Update expense
            float oldAmount = exps[actualIndex].exp_amt;
            
            printf("Current Category: %s, Current Amount: %.2f, Current Date: %s\n",
                   CATEGORY_FULL_NAMES[exps[actualIndex].category],
                   exps[actualIndex].exp_amt,
                   exps[actualIndex].dateOfExpense);

            printf("Enter new Category (R=Rent, U=Utility, G=Grocery, S=Stationary, L=Leisure): ");
            char cat_str[2];
            scanf("%1s", cat_str);
            
            // Validate category
            if (strlen(cat_str) != 1 || !strchr("RUGSL", cat_str[0])) {
                printf("Invalid category! Please use R, U, G, S, or L.\n");
                return;
            }
            
            // Convert to enum
            exps[actualIndex].category = string_to_category(cat_str);

            printf("Enter new Amount: ");
            scanf("%f", &exps[actualIndex].exp_amt);

            printf("Enter new Date (DD-MM-YYYY): ");
            scanf("%10s", exps[actualIndex].dateOfExpense);
            if (strlen(exps[actualIndex].dateOfExpense) != 10 || 
                exps[actualIndex].dateOfExpense[2] != '-' || 
                exps[actualIndex].dateOfExpense[5] != '-') {
                printf("Invalid date format. Please enter in DD-MM-YYYY format.\n");
                return;
            }

            // Update family expense if user belongs to a family
            if (familyID > 0) {
                for (int i = 0; i < fam_cnt; i++) {
                    if (families[i].familyID == familyID) {
                        families[i].Mon_Expense = families[i].Mon_Expense - oldAmount + exps[actualIndex].exp_amt;
                        break;
                    }
                }
            }

            // Save updated data
            Save_expenses_data();
            Save_families_data();
            
            printf("Expense updated successfully!\n");
            break;
        }
        case 2: {
            // Delete expense
            float deletedAmount = exps[actualIndex].exp_amt;

            // Update family expense if user belongs to a family
            if (familyID > 0) {
                for (int i = 0; i < fam_cnt; i++) {
                    if (families[i].familyID == familyID) {
                        families[i].Mon_Expense -= deletedAmount;
                        break;
                    }
                }
            }

            // Delete the expense
            for (int i = actualIndex; i < exp_cnt - 1; i++) {
                exps[i] = exps[i + 1];
            }
            exp_cnt--;

            // Save updated data
            Save_expenses_data();
            Save_families_data();
            
            printf("Expense deleted successfully!\n");
            break;
        }
        default:
            printf("Invalid choice!\n");
    }
}

// Function to get total family expense
void Get_total_expense() {
    int familyID;
    printf("Enter Family ID: ");
    scanf("%d", &familyID);
    
    // Find the family
    int found = 0;
    int familyIndex = -1;
    for (int i = 0; i < fam_cnt; i++) {
        if (families[i].familyID == familyID) {
            found = 1;
            familyIndex = i;
            break;
        }
    }
    
    if (!found) {
        printf("Family not found!\n");
        return;
    }
    
    float totalExpense = families[familyIndex].Mon_Expense;
    float totalIncome = families[familyIndex].Tot_Income;
    
    printf("\nFamily: %s (ID: %d)\n", 
           families[familyIndex].familyName, 
           families[familyIndex].familyID);
    printf("Total Monthly Family Expense: %.2f\n", totalExpense);
    printf("Total Family Income: %.2f\n", totalIncome);
    
    if (totalExpense <= totalIncome) {
        printf("The expense is within the family income.\n");
        printf("Remaining balance: %.2f\n", totalIncome - totalExpense);
    } else {
        printf("The expense surpasses the family income!\n");
        printf("Overspent by: %.2f\n", totalExpense - totalIncome);
    }
}

// Function to get categorical expense
void Get_categorical_expense() {
    int familyID;
    printf("Enter Family ID: ");
    scanf("%d", &familyID);
    
    char category_str[2];
    printf("Enter Category (R=Rent, U=Utility, G=Grocery, S=Stationary, L=Leisure): ");
    scanf("%1s", category_str);
    
    // Validate category
    if (strlen(category_str) != 1 || !strchr("RUGSL", category_str[0])) {
        printf("Invalid category! Please use R, U, G, S, or L.\n");
        return;
    }
    
    // Convert to enum
    ExpenseCategory category = string_to_category(category_str);
    
    // Find the family
    int found = 0;
    int familyIndex = -1;
    for (int i = 0; i < fam_cnt; i++) {
        if (families[i].familyID == familyID) {
            found = 1;
            familyIndex = i;
            break;
        }
    }
    
    if (!found) {
        printf("Family not found!\n");
        return;
    }
    
    // Calculate total family expense for the given category
    float totalCategoryExpense = 0.0;
    float individualExpenses[MAX_MEMBERS] = {0};
    int memberIDs[MAX_MEMBERS] = {0};
    int memberCount = families[familyIndex].numberFamily;
    
    // Copy member IDs
    for (int i = 0; i < memberCount; i++) {
        memberIDs[i] = families[familyIndex].userIDs[i];
    }
    
    // Calculate expenses for each member
    for (int i = 0; i < exp_cnt; i++) {
        if (exps[i].category == category) {
            for (int j = 0; j < memberCount; j++) {
                if (exps[i].userID == memberIDs[j]) {
                    individualExpenses[j] += exps[i].exp_amt;
                    totalCategoryExpense += exps[i].exp_amt;
                }
            }
        }
    }
    
    // Sort members by their expenses (descending)
    for (int i = 0; i < memberCount - 1; i++) {
        for (int j = 0; j < memberCount - i - 1; j++) {
            if (individualExpenses[j] < individualExpenses[j + 1]) {
                // Swap expenses
                float tempExp = individualExpenses[j];
                individualExpenses[j] = individualExpenses[j + 1];
                individualExpenses[j + 1] = tempExp;
                
                // Swap member IDs
                int tempID = memberIDs[j];
                memberIDs[j] = memberIDs[j + 1];
                memberIDs[j + 1] = tempID;
            }
        }
    }
    
    printf("\nFamily: %s (ID: %d)\n", 
           families[familyIndex].familyName, 
           families[familyIndex].familyID);
    printf("Total expense for %s: %.2f\n", 
           CATEGORY_FULL_NAMES[category], 
           totalCategoryExpense);
    printf("\nIndividual Contributions (sorted):\n");
    printf("---------------------------------------------------\n");
    printf("| User ID |     Name     |  Amount  | Percentage |\n");
    printf("---------------------------------------------------\n");
    
    for (int i = 0; i < memberCount; i++) {
        // Find user name
        char userName[50] = "Unknown";
        for (int j = 0; j < Ucnt; j++) {
            if (users[j].userID == memberIDs[i]) {
                strcpy(userName, users[j].userName);
                break;
            }
        }
        
        float percentage = (totalCategoryExpense > 0) ? 
                          (individualExpenses[i] / totalCategoryExpense) * 100 : 0;
        
        printf("| %-7d | %-12s | %-8.2f | %-9.2f%% |\n",
               memberIDs[i],
               userName,
               individualExpenses[i],
               percentage);
    }
    printf("---------------------------------------------------\n");
}

// Function to get highest expense day
void Get_highest_expense_day() {
    int familyID;
    printf("Enter Family ID: ");
    scanf("%d", &familyID);
    
    // Find the family
    int found = 0;
    int familyIndex = -1;
    for (int i = 0; i < fam_cnt; i++) {
        if (families[i].familyID == familyID) {
            found = 1;
            familyIndex = i;
            break;
        }
    }
    
    if (!found) {
        printf("Family not found!\n");
        return;
    }
    
    // Create an array to store expenses for each day
    char dates[MAX_EXPENSES][11];
    float dailyExpenses[MAX_EXPENSES] = {0};
    int dateCount = 0;
    
    // Calculate expenses for each day
    for (int i = 0; i < exp_cnt; i++) {
        int memberFound = 0;
        // Check if expense belongs to a family member
        for (int j = 0; j < families[familyIndex].numberFamily; j++) {
            if (exps[i].userID == families[familyIndex].userIDs[j]) {
                memberFound = 1;
                break;
            }
        }
        
        if (memberFound) {
            // Check if date already exists in our dates array
            int dateIndex = -1;
            for (int k = 0; k < dateCount; k++) {
                if (strcmp(dates[k], exps[i].dateOfExpense) == 0) {
                    dateIndex = k;
                    break;
                }
            }
            
            if (dateIndex == -1) {
                // New date
                strcpy(dates[dateCount], exps[i].dateOfExpense);
                dailyExpenses[dateCount] = exps[i].exp_amt;
                dateCount++;
            } else {
                // Existing date, add to daily expense
                dailyExpenses[dateIndex] += exps[i].exp_amt;
            }
        }
    }
    
    if (dateCount == 0) {
        printf("No expenses found for this family.\n");
        return;
    }
    
    // Find the day with highest expense
    int maxIndex = 0;
    for (int i = 1; i < dateCount; i++) {
        if (dailyExpenses[i] > dailyExpenses[maxIndex]) {
            maxIndex = i;
        }
    }
    
    printf("\nFamily: %s (ID: %d)\n", 
           families[familyIndex].familyName, 
           families[familyIndex].familyID);
    printf("Day with highest expense: %s\n", dates[maxIndex]);
    printf("Amount spent: %.2f\n", dailyExpenses[maxIndex]);
    
    // Show expenses on that day
    printf("\nExpenses on %s:\n", dates[maxIndex]);
    printf("------------------------------------------------------------------\n");
    printf("| User ID |     Name     |  Category  |  Amount  |\n");
    printf("------------------------------------------------------------------\n");
    
    for (int i = 0; i < exp_cnt; i++) {
        int memberFound = 0;
        for (int j = 0; j < families[familyIndex].numberFamily; j++) {
            if (exps[i].userID == families[familyIndex].userIDs[j]) {
                memberFound = 1;
                break;
            }
        }
        
        if (memberFound && strcmp(exps[i].dateOfExpense, dates[maxIndex]) == 0) {
            // Find user name
            char userName[50] = "Unknown";
            for (int j = 0; j < Ucnt; j++) {
                if (users[j].userID == exps[i].userID) {
                    strcpy(userName, users[j].userName);
                    break;
                }
            }
            
            printf("| %-7d | %-12s | %-10s | %-8.2f |\n",
                   exps[i].userID,
                   userName,
                   CATEGORY_FULL_NAMES[exps[i].category],
                   exps[i].exp_amt);
        }
    }
    printf("------------------------------------------------------------------\n");
}

// Function to get individual expense
void Get_individual_expense() {
    int userID;
    printf("Enter User ID: ");
    scanf("%d", &userID);
    
    // Check if user exists
    int found = 0;
    int userIndex = -1;
    for (int i = 0; i < Ucnt; i++) {
        if (users[i].userID == userID) {
            found = 1;
            userIndex = i;
            break;
        }
    }
    
    if (!found) {
        printf("User not found!\n");
        return;
    }
    
    // Calculate total expense for the user
    float totalExpense = 0.0;
    float categoryExpenses[MAX_CATEGORIES] = {0};
    
    for (int i = 0; i < exp_cnt; i++) {
        if (exps[i].userID == userID) {
            totalExpense += exps[i].exp_amt;
            categoryExpenses[exps[i].category] += exps[i].exp_amt;
        }
    }
    
    printf("\nUser: %s (ID: %d)\n", users[userIndex].userName, userID);
    printf("Total monthly expense: %.2f\n", totalExpense);
    
    // Create a temporary array to sort categories
    struct CategoryExpense {
        ExpenseCategory category;
        float amount;
    };
    
    struct CategoryExpense catExp[MAX_CATEGORIES];
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        catExp[i].category = i;
        catExp[i].amount = categoryExpenses[i];
    }
    
    // Sort categories by expense (descending)
    for (int i = 0; i < MAX_CATEGORIES - 1; i++) {
        for (int j = 0; j < MAX_CATEGORIES - i - 1; j++) {
            if (catExp[j].amount < catExp[j + 1].amount) {
                struct CategoryExpense temp = catExp[j];
                catExp[j] = catExp[j + 1];
                catExp[j + 1] = temp;
            }
        }
    }
    
    printf("\nCategory-wise Expenses (sorted by amount):\n");
    printf("----------------------------------------------------\n");
    printf("|  Category  |  Amount  | Percentage of Total |\n");
    printf("----------------------------------------------------\n");
    
    for (int i = 0; i < MAX_CATEGORIES; i++) {
        if (catExp[i].amount > 0) {
            float percentage = (totalExpense > 0) ? 
                              (catExp[i].amount / totalExpense) * 100 : 0;
            
            printf("| %-10s | %-8.2f | %-18.2f%% |\n",
                   CATEGORY_FULL_NAMES[catExp[i].category],
                   catExp[i].amount,
                   percentage);
        }
    }
    printf("----------------------------------------------------\n");
}

// Function to prepare initial dataset
void PrepareInitialData() {
    // Check if files already exist
    FILE *test = fopen("users.txt", "r");
    if (test != NULL) {
        fclose(test);
        printf("Data files already exist. Skipping initial data creation.\n");
        return;
    }
    
    // Create initial dataset
    printf("Creating initial dataset...\n");
    
    // Create 25 individuals
    for (int i = 1; i <= 25; i++) {
        sprintf(users[Ucnt].userName, "User%d", i);
        users[Ucnt].userID = i;
        users[Ucnt].Income = 5000.0 + (rand() % 5000);
        users[Ucnt].familyID = 0;  // Will be assigned later
        Ucnt++;
    }
    
    // Create 10 families
    int userIndex = 0;
    for (int i = 1; i <= 10; i++) {
        sprintf(families[fam_cnt].familyName, "Family%d", i);
        families[fam_cnt].familyID = i;
        
        // Randomly decide number of members (1-4)
        int members = 1 + (rand() % 4);
        if (userIndex + members > 25) {
            members = 25 - userIndex;  // Ensure we don't exceed available users
        }
        
        families[fam_cnt].numberFamily = members;
        families[fam_cnt].Tot_Income = 0;
        
        // Add members to family
        for (int j = 0; j < members; j++) {
            int userId = userIndex + 1;  // User IDs start from 1
            families[fam_cnt].userIDs[j] = userId;
            users[userIndex].familyID = i;
            families[fam_cnt].Tot_Income += users[userIndex].Income;
            userIndex++;
        }
        
        fam_cnt++;
    }
    
    // Create 40 random expenses
    for (int i = 1; i <= 40; i++) {
        exps[exp_cnt].expenseID = i;
        
        // Random user ID (1-25)
        int userId = 1 + (rand() % 25);
        exps[exp_cnt].userID = userId;
        
        // Random category
        exps[exp_cnt].category = (ExpenseCategory)(rand() % MAX_CATEGORIES);
        
        // Random amount between 100 and 1000
        exps[exp_cnt].exp_amt = 100.0 + (rand() % 900) + ((float)(rand() % 100) / 100.0);
        
        // Random date (DD-MM-YYYY) with day 1-10
        int day = 1 + (rand() % 10);
        sprintf(exps[exp_cnt].dateOfExpense, "%02d-05-2023", day);
        
        exp_cnt++;
    }
    
    // Update family expenses
    for (int i = 0; i < fam_cnt; i++) {
        families[i].Mon_Expense = 0;
        
        for (int j = 0; j < families[i].numberFamily; j++) {
            int memberId = families[i].userIDs[j];
            
            for (int k = 0; k < exp_cnt; k++) {
                if (exps[k].userID == memberId) {
                    families[i].Mon_Expense += exps[k].exp_amt;
                }
            }
        }
    }
    
    // Sort data
    Sort_users();
    Bubble_sort();
    
    // Save data to files
    Save_users_data();
    Save_expenses_data();
    Save_families_data();
    
    printf("Initial dataset created and saved to files successfully.\n");
}

int main() {
    Initialize_Individual();
    Initialize_Expense();
    Initialize_Family();
    
    // Read data from files
    Input_userdata();
    Input_expdata();
    Input_family_data();
    
    // If no data loaded, prepare initial dataset
    if (Ucnt == 0 && exp_cnt == 0 && fam_cnt == 0) {
        PrepareInitialData();
    }
    
    // Sort the data
    Sort_users();
    Bubble_sort();
    
    int choice;
    do {
        printf("\n--- Expense Tracking Application ---\n");
        printf("1. Add User\n");
        printf("2. Display Users\n");
        printf("3. Add Expense\n");
        printf("4. Display Expenses\n");
        printf("5. Create Family\n");
        printf("6. Display Families\n");
        printf("7. Update/Delete Individual/Family Details\n");
        printf("8. Update/Delete Expense\n");
        printf("9. Get Total Family Expense\n");
        printf("10. Get Categorical Expense\n");
        printf("11. Get Highest Expense Day\n");
        printf("12. Get Individual Expense\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                AddUser();
                break;
            case 2:
                Print_Udata();
                break;
            case 3:
                Add_exp();
                break;
            case 4:
                Print_struct();
                break;
            case 5:
                CreateFamily();
                break;
            case 6:
                Print_family();
                break;
            case 7:
                UpdateOrDeleteIndividualFamilyDetails();
                break;
            case 8: {
                int userID;
                printf("Enter User ID: ");
                scanf("%d", &userID);
                update_delete_expenses(userID, 0);
                break;
            }
            case 9:
                Get_total_expense();
                break;
            case 10:
                Get_categorical_expense();
                break;
            case 11:
                Get_highest_expense_day();
                break;
            case 12:
                Get_individual_expense();
                break;
            case 0:
                printf("Exiting program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    return 0;
}