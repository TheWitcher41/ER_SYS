#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define MAX_BUFFER_SIZE 256
#define MAX_PATIENTS 4
#define MAX_VISITS 10
#define MAX_DOCTORS 10
#define MAX_ALLERGIES 8

// Date Structure
typedef struct Date {
    int year;
    int month;
    int day;
    int hour;
    int min;
} Date;

// Doctor Structure
typedef struct Doc {
    char *name;
    char nLicense[8];
    int nPatients;
} Doc;

// Visit Structure
typedef struct Visit {
    Date tArrival;
    Date tDismissed;
    float duration;
    Doc *doctor;
    char *vSummary;
} Visit;

// Patient Structure
typedef struct Patient {
    char *name;
    char ID[11];
    char allergies;
    Visit *visits[MAX_VISITS];
    int nVisits;
} Patient;

// Binary Search Tree Node Structure
typedef struct pInTree {
    Patient tpatient;
    struct pInTree *right;
    struct pInTree *left;
} pInTree;

// Structure of a Patient in the Emergency Room Queue
typedef struct pInLine {
    Patient *lpatient;
    struct pInLine *next;
} pInLine;

// Doctor Node Structure
typedef struct DocNode {
    Doc doctor;
    struct DocNode *next;
} DocNode;

// Global Variables
pInTree *pTree = NULL;
pInLine *pLine = NULL;
DocNode *pDoc = NULL;

// Function Prototypes
void insertPatient(pInTree **pTree, Patient patient);
pInTree *searchPatient(pInTree *pTree, char ID[]);
void displayPatient(Patient patient);
void displayAllPatients(pInTree *pTree);
void deletePatient(pInTree **pTree, char ID[]);
void enqueuePatient(pInLine **pLine, Patient *patient);
Patient *dequeuePatient(pInLine **pLine);
void displayQueue(pInLine *pLine);
void movePatientToFront(pInLine **pLine, char ID[]);
void insertDoctor(DocNode **pDoc, Doc doctor);
DocNode *searchDoctor(DocNode *pDoc, char nameOrLicense[]);
void displayDoctor(Doc doctor);
void displayAllDoctors(DocNode *pDoc);
void assignDoctorToPatient(Patient *patient, DocNode *pDoc);
void loadPatients(pInTree **pTree);
void loadDoctors(DocNode **pDoc);
void loadLine(pInLine **pLine);
void updateFiles(pInTree *pTree, pInLine *pLine, DocNode *pDoc);
void displayError(int errorCode);
int validateInput(int inputType, char input[]);
void *allocateMemory(size_t size);
void freeMemory(void *ptr);
void admitPatient();
void checkPatientAllergies();
void displayAllPatients1();
void displayAllAdmissions();
void displayPatientsInLine();
void advancePatientInLine();
void displayAllDoctors1();
void displayAssignedPatients();
void displayPatientsOfDoctor(pInTree *pTree, char *license);
void dischargePatient();
void removeVisit();
void removePatient();
void freeAllPatients(pInTree *pTree);
void freeAllPatientsInLine(pInLine *pLine);
void freeAllDoctors(DocNode *pDoc);
void closeHospital();
void saveData();
void displayMenu();
void mainMenu();
void initEmergencyRoom();

// Function to insert a new patient into the BST
void insertPatient(pInTree **pTree, Patient patient) {
    if (*pTree == NULL) {
        *pTree = (pInTree *)malloc(sizeof(pInTree));
        (*pTree)->tpatient = patient;
        (*pTree)->left = NULL;
        (*pTree)->right = NULL;
    } else {
        if (strcmp(patient.ID, (*pTree)->tpatient.ID) < 0) {
            insertPatient(&(*pTree)->left, patient);
        } else if (strcmp(patient.ID, (*pTree)->tpatient.ID) > 0) {
            insertPatient(&(*pTree)->right, patient);
        }
    }
}

// Function to search for a patient by ID
pInTree *searchPatient(pInTree *pTree, char ID[]) {
    if (pTree == NULL) {
        return NULL;
    }
    if (strcmp(ID, pTree->tpatient.ID) == 0) {
        return pTree;
    } else if (strcmp(ID, pTree->tpatient.ID) < 0) {
        return searchPatient(pTree->left, ID);
    } else {
        return searchPatient(pTree->right, ID);
    }
}

// Function to display information about a single patient
void displayPatient(Patient patient) {
    printf("Name: %s\n", patient.name);
    printf("ID: %s\n", patient.ID);
    printf("Allergies: ");
    if (patient.allergies == 0) {
        printf("None\n");
    } else {
        if (patient.allergies & 1) {
            printf("Penicillin ");
        }
        if (patient.allergies & 2) {
            printf("Sulfa ");
        }
        if (patient.allergies & 4) {
            printf("Opioids ");
        }
        if (patient.allergies & 8) {
            printf("Anesthetics ");
        }
        if (patient.allergies & 16) {
            printf("Eggs ");
        }
        if (patient.allergies & 32) {
            printf("Latex ");
        }
        if (patient.allergies & 64) {
            printf("Preservatives ");
        }
        printf("\n");
    }
    printf("Number of Visits: %d\n", patient.nVisits);
}

// Function to display all patients in the BST (in-order traversal)
void displayAllPatients(pInTree *pTree) {
    if (pTree != NULL) {
        displayAllPatients(pTree->left);
        displayPatient(pTree->tpatient);
        displayAllPatients(pTree->right);
    }
}

// Function to delete a patient by ID
void deletePatient(pInTree **pTree, char ID[]) {
    if (*pTree == NULL) {
        return;
    }
    if (strcmp(ID, (*pTree)->tpatient.ID) < 0) {
        deletePatient(&(*pTree)->left, ID);
    } else if (strcmp(ID, (*pTree)->tpatient.ID) > 0) {
        deletePatient(&(*pTree)->right, ID);
    } else {
        if ((*pTree)->left == NULL && (*pTree)->right == NULL) {
            free(*pTree);
            *pTree = NULL;
        } else if ((*pTree)->left == NULL) {
            pInTree *temp = *pTree;
            *pTree = (*pTree)->right;
            free(temp);
        } else if ((*pTree)->right == NULL) {
            pInTree *temp = *pTree;
            *pTree = (*pTree)->left;
            free(temp);
        } else {
            pInTree *temp = (*pTree)->right;
            while (temp->left != NULL) {
                temp = temp->left;
            }
            (*pTree)->tpatient = temp->tpatient;
            deletePatient(&(*pTree)->right, temp->tpatient.ID);
        }
    }
}

// Function to add a patient to the queue
void enqueuePatient(pInLine **pLine, Patient *patient) {
    pInLine *newNode = (pInLine *)malloc(sizeof(pInLine));
    newNode->lpatient = patient;
    newNode->next = NULL;
    if (*pLine == NULL) {
        *pLine = newNode;
    } else {
        pInLine *temp = *pLine;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to remove and return the patient at the front of the queue
Patient *dequeuePatient(pInLine **pLine) {
    if (*pLine == NULL) {
        return NULL;
    }
    pInLine *temp = *pLine;
    *pLine = (*pLine)->next;
    Patient *patient = temp->lpatient;
    free(temp);
    return patient;
}

// Function to display patients in the queue
void displayQueue(pInLine *pLine) {
    pInLine *temp = pLine;
    while (temp != NULL) {
        displayPatient(*(temp->lpatient));
        temp = temp->next;
    }
}

// Function to move a patient to the front of the queue
void movePatientToFront(pInLine **pLine, char ID[]) {
    if (*pLine == NULL) {
        return;
    }
    if (strcmp(ID, (*pLine)->lpatient->ID) == 0) {
        return;
    }
    pInLine *prev = *pLine;
    pInLine *curr = (*pLine)->next;
    while (curr != NULL) {
        if (strcmp(ID, curr->lpatient->ID) == 0) {
            prev->next = curr->next;
            curr->next = *pLine;
            *pLine = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Function to insert a new doctor into the linked list
void insertDoctor(DocNode **pDoc, Doc doctor) {
    DocNode *newNode = (DocNode *)malloc(sizeof(DocNode));
    newNode->doctor = doctor;
    newNode->next = NULL;
    if (*pDoc == NULL) {
        *pDoc = newNode;
    } else {
        DocNode *temp = *pDoc;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to search for a doctor by name or license
DocNode *searchDoctor(DocNode *pDoc, char nameOrLicense[]) {
    DocNode *temp = pDoc;
    while (temp != NULL) {
        if (strcmp(nameOrLicense, temp->doctor.name) == 0 || strcmp(nameOrLicense, temp->doctor.nLicense) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

// Function to display information about a single doctor
void displayDoctor(Doc doctor) {
    printf("Name: %s\n", doctor.name);
    printf("License Number: %s\n", doctor.nLicense);
    printf("Number of Patients: %d\n", doctor.nPatients);
}

// Function to display all doctors in the linked list
void displayAllDoctors(DocNode *pDoc) {
    DocNode *temp = pDoc;
    while (temp != NULL) {
        displayDoctor(temp->doctor);
        temp = temp->next;
    }
}

// Function to assign a doctor to a patient based on availability
void assignDoctorToPatient(Patient *patient, DocNode *pDoc) {
    if (!patient) {
        fprintf(stderr, "Patient is NULL\n");
        return;
    }
    if (patient->nVisits < 0 || patient->nVisits >= MAX_VISITS) {
        fprintf(stderr, "Invalid visit index\n");
        return;
    }
    if (!patient->visits[patient->nVisits]) {
        patient->visits[patient->nVisits] = malloc(sizeof(Visit));
        if (!patient->visits[patient->nVisits]) {
            perror("Failed to allocate memory for visit");
            return;
        }
    }
    patient->visits[patient->nVisits]->doctor = malloc(sizeof(Doc));
    if (!patient->visits[patient->nVisits]->doctor) {
        perror("Failed to allocate memory for doctor");
        return;
    }
    DocNode *tempDoc = pDoc;
    while (tempDoc != NULL) {
        if (tempDoc->doctor.nPatients < MAX_PATIENTS) {
            *(patient->visits[patient->nVisits]->doctor) = tempDoc->doctor;
            tempDoc->doctor.nPatients++;
            return;
        }
        tempDoc = tempDoc->next;
    }
    displayError(7);
}

//cfaskdfj
void parseDate(const char *str, Date *date) {
    sscanf(str, "%d/%d/%d %d:%d", &date->year, &date->month, &date->day, &date->hour, &date->min);
}

void parseDuration(const char *str, float *duration) {
    int hours, minutes;
    sscanf(str, "%d:%d", &hours, &minutes);
    *duration = hours * 60 + minutes;
}

Patient* readPatientsFromFile(const char *filename, int *numPatients) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    Patient *patients = NULL;
    int patientCount = 0;
    char line[256];
    Patient *currentPatient = NULL;
    Visit *currentVisit = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "========================")) {
            continue;
        }

        if (isdigit(line[0])) {
            // New patient
            patientCount++;
            Patient *temp = realloc(patients, patientCount * sizeof(Patient));
            if (!temp) {
                perror("Failed to allocate memory for patients");
                free(patients);
                fclose(file);
                return NULL;
            }
            patients = temp;
            currentPatient = &patients[patientCount - 1];
            currentPatient->nVisits = 0;

            char *token = strtok(line, ";");
            currentPatient->name = strdup(token);
            if (!currentPatient->name) {
                perror("Failed to allocate memory for patient name");
                free(patients);
                fclose(file);
                return NULL;
            }

            token = strtok(NULL, ";");
            strncpy(currentPatient->ID, token, sizeof(currentPatient->ID) - 1);
            currentPatient->ID[sizeof(currentPatient->ID) - 1] = '\0';

            token = strtok(NULL, ";");
            currentPatient->allergies = strdup(token);
            if (!currentPatient->allergies) {
                perror("Failed to allocate memory for patient allergies");
                free(currentPatient->name);
                free(patients);
                fclose(file);
                return NULL;
            }
        } else if (strstr(line, "Arrival:")) {
            // New visit
            currentPatient->nVisits++;
            currentVisit = malloc(sizeof(Visit));
            if (!currentVisit) {
                perror("Failed to allocate memory for visit");
                free(currentPatient->name);
                free(currentPatient->allergies);
                free(patients);
                fclose(file);
                return NULL;
            }
            currentPatient->visits[currentPatient->nVisits - 1] = currentVisit;

            char *token = strtok(line + 8, "\n");
            parseDate(token, &currentVisit->tArrival);
        } else if (strstr(line, "Dismissed:")) {
            char *token = strtok(line + 10, "\n");
            if (strlen(token) > 0) {
                parseDate(token, &currentVisit->tDismissed);
            } else {
                memset(&currentVisit->tDismissed, 0, sizeof(Date));
            }
        } else if (strstr(line, "Duration:")) {
            char *token = strtok(line + 9, "\n");
            if (strlen(token) > 0) {
                parseDuration(token, &currentVisit->duration);
            } else {
                currentVisit->duration = 0;
            }
        } else if (strstr(line, "Doctor:")) {
            char *token = strtok(line + 7, "\n");
            currentVisit->doctor = malloc(sizeof(Doc));
            if (!currentVisit->doctor) {
                perror("Failed to allocate memory for doctor");
                free(currentPatient->name);
                free(currentPatient->allergies);
                free(patients);
                fclose(file);
                return NULL;
            }
            currentVisit->doctor->name = strdup(token);
            if (!currentVisit->doctor->name) {
                perror("Failed to allocate memory for doctor name");
                free(currentVisit->doctor);
                free(currentPatient->name);
                free(currentPatient->allergies);
                free(patients);
                fclose(file);
                return NULL;
            }
        } else if (strstr(line, "Summary:")) {
            char *token = strtok(line + 8, "\n");
            currentVisit->vSummary = strdup(token);
            if (!currentVisit->vSummary) {
                perror("Failed to allocate memory for visit summary");
                free(currentVisit->doctor->name);
                free(currentVisit->doctor);
                free(currentPatient->name);
                free(currentPatient->allergies);
                free(patients);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    *numPatients = patientCount;
    return patients;
}


// Function to load patient data from Patients.txt into the BST
void loadPatients(pInTree **pTree) {
    int numPatients;
    Patient *patients = readPatientsFromFile("Patients.txt", &numPatients);
    if (!patients) {
        return;
    }
    for (int i = 0; i < numPatients; i++) {
        insertPatient(pTree, patients[i]);
    }
    free(patients);
}


// Function to read doctors from file
Doc* readDoctorsFromFile(const char *filename, int *numDoctors) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    Doc *doctors = NULL;
    int doctorCount = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "========================") || strstr(line, "Full Name")) {
            continue;
        }
        doctorCount++;
        Doc *temp = realloc(doctors, doctorCount * sizeof(Doc));
        if (!temp) {
            perror("Failed to allocate memory for doctors");
            free(doctors);
            fclose(file);
            return NULL;
        }
        doctors = temp;

        char *token = strtok(line, ";");
        doctors[doctorCount - 1].name = strdup(token);
        if (!doctors[doctorCount - 1].name) {
            perror("Failed to allocate memory for doctor name");
            free(doctors);
            fclose(file);
            return NULL;
        }

        token = strtok(NULL, ";");
        strcpy(doctors[doctorCount - 1].nLicense, token);

        token = strtok(NULL, ";");
        doctors[doctorCount - 1].nPatients = atoi(token);
    }
    fclose(file);
    *numDoctors = doctorCount;
    return doctors;
}

// Function to load doctor data from Doctors.txt into the linked list
void loadDoctors(DocNode **pDoc) {
    int numDoctors;
    Doc *doctors = readDoctorsFromFile("Doctors.txt", &numDoctors);
    if (!doctors) {
        return;
    }
    for (int i = 0; i < numDoctors; i++) {
        insertDoctor(pDoc, doctors[i]);
    }
    free(doctors);
}

// Function to read patient IDs from Line.txt into the queue
char** readPatientsInLineFromFile(const char *filename, int *numPatients) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    char **patients = NULL;
    int patientCount = 0;
    char line[256];

    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        char *id = strchr(line, '.');
        if (id) {
            id++;
            while (*id == ' ') id++;
        } else {
            id = line;
        }

        patientCount++;
        char **temp = realloc(patients, patientCount * sizeof(char *));
        if (!temp) {
            perror("Failed to allocate memory for patients");
            for (int i = 0; i < patientCount - 1; i++) {
                free(patients[i]);
            }
            free(patients);
            fclose(file);
            return NULL;
        }
        patients = temp;
        patients[patientCount - 1] = strdup(id);
        if (!patients[patientCount - 1]) {
            perror("Failed to allocate memory for patient ID");
            for (int i = 0; i < patientCount; i++) {
                free(patients[i]);
            }
            free(patients);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    *numPatients = patientCount;
    return patients;
}

// Function to load patient IDs from Line.txt into the queue
void loadLine(pInLine **pLine) {
    int numPatients;
    char **patientIDs = readPatientsInLineFromFile("Line.txt", &numPatients);
    if (!patientIDs) {
        return;
    }
    for (int i = 0; i < numPatients; i++) {
        char *newline = strchr(patientIDs[i], '\n');
        if (newline) {
            *newline = '\0';
        }
        pInTree *patientNode = searchPatient(pTree, patientIDs[i]);
        if (patientNode) {
            enqueuePatient(pLine, &(patientNode->tpatient));
        }
        free(patientIDs[i]);
    }
    free(patientIDs);
}

// Function to save data to the respective files
void updateFiles(pInTree *pTree, pInLine *pLine, DocNode *pDoc) {
    FILE *file = fopen("Patients.txt", "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "Name; ID; Allergies\n");
    fprintf(file, "========================\n");
    // Write patient data here
    fclose(file);

    file = fopen("Doctors.txt", "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "Full Name; License Number; Number of Patients\n");
    fprintf(file, "========================\n");
    // Write doctor data here
    fclose(file);

    file = fopen("Line.txt", "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "Patients in Line\n");
    fprintf(file, "========================\n");
    pInLine *temp = pLine;
    int count = 1;
    while (temp != NULL) {
        fprintf(file, "%d. %s\n", count++, temp->lpatient->ID);
        temp = temp->next;
    }
    fclose(file);
}

// Function to display error messages based on codes
void displayError(int errorCode) {
    switch (errorCode) {
    case 1:
        printf("Invalid input. Please try again.\n");
        break;
    case 2:
        printf("Patient not found.\n");
        break;
    case 3:
        printf("Doctor not found.\n");
        break;
    case 4:
        printf("Patient with the same ID already exists.\n");
        break;
    case 5:
        printf("Doctor with the same license number already exists.\n");
        break;
    case 6:
        printf("Patient queue is empty.\n");
        break;
    case 7:
        printf("Doctor list is empty.\n");
        break;
    default:
        break;
    }
}

// Function to validate different types of user input
int validateInput(int inputType, char input[]) {
    switch (inputType) {
    case 1:
        for (int i = 0; i < strlen(input); i++) {
            if (!isalpha(input[i]) && input[i] != ' ') {
                return 0;
            }
        }
        return 1;
    case 2:
        if (strlen(input) != 10) {
            return 0;
        }
        for (int i = 0; i < strlen(input); i++) {
            if (!isdigit(input[i])) {
                return 0;
            }
        }
        return 1;
    case 3:
        if (strlen(input) != 8) {
            return 0;
        }
        for (int i = 0; i < strlen(input); i++) {
            if (!isdigit(input[i])) {
                return 0;
            }
        }
        return 1;
    case 4:
        if (strlen(input) != 7) {
            return 0;
        }
        for (int i = 0; i < strlen(input); i++) {
            if (!isdigit(input[i])) {
                return 0;
            }
        }
        return 1;
    case 5:
        if (strlen(input) != 5) {
            return 0;
        }
        for (int i = 0; i < strlen(input); i++) {
            if (!isdigit(input[i])) {
                return 0;
            }
        }
        return 1;
    default:
        return 0;
    }
}

// Function to dynamically allocate memory
void *allocateMemory(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        perror("Failed to allocate memory");
        exit(1);
    }
    return ptr;
}

// Function to free dynamically allocated memory
void freeMemory(void *ptr) {
    if (ptr) {
        free(ptr);
    }
}

// Function to admit a patient to the emergency room
void admitPatient() {
    if (pDoc == NULL) {
        displayError(7);
        return;
    }
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    pInTree *patientNode = searchPatient(pTree, ID);
    if (patientNode == NULL) {
        Patient patient;
        patient.name = allocateMemory(MAX_BUFFER_SIZE);
        printf("Enter the patient's name: ");
        scanf(" %[^\n]", patient.name);
        if (!validateInput(1, patient.name)) {
            displayError(1);
            freeMemory(patient.name);
            return;
        }
        strcpy(patient.ID, ID);
        int allergy_value;
        printf("Enter the patient's allergies (bitwise encoded value): ");
        scanf("%d", &allergy_value);
        patient.allergies = (char)allergy_value;
        patient.nVisits = 0;
        insertPatient(&pTree, patient);
        patientNode = searchPatient(pTree, ID);
    }
    if (patientNode->tpatient.nVisits > 0 && patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.year == -1) {
        displayError(4);
        return;
    }
    Visit visit;
    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    visit.tArrival.year = current->tm_year + 1900;
    visit.tArrival.month = current->tm_mon + 1;
    visit.tArrival.day = current->tm_mday;
    visit.tArrival.hour = current->tm_hour;
    visit.tArrival.min = current->tm_min;
    visit.tDismissed.year = -1;
    visit.tDismissed.month = -1;
    visit.tDismissed.day = -1;
    visit.tDismissed.hour = -1;
    visit.tDismissed.min = -1;
    visit.duration = -1;
    visit.vSummary = NULL;
    patientNode->tpatient.visits[patientNode->tpatient.nVisits] = &visit;
    patientNode->tpatient.nVisits++;
    assignDoctorToPatient(&(patientNode->tpatient), pDoc);
    enqueuePatient(&pLine, &(patientNode->tpatient));
}

// Function to display the list of patients in the queue
void checkPatientAllergies() {
    if (pLine == NULL) {
        displayError(6);
        return;
    }
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    pInTree *patientNode = searchPatient(pTree, ID);
    if (patientNode == NULL) {
        displayError(2);
        return;
    }
    displayPatient(patientNode->tpatient);
}

// Function to display the details of all patients in the BST
void displayAllPatients1() {
    if (pTree == NULL) {
        displayError(7);
        return;
    }
    displayAllPatients(pTree);
}

// Function to display all visits of a patient in reverse chronological order
void displayAllAdmissions() {
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    pInTree *patientNode = searchPatient(pTree, ID);
    if (patientNode == NULL) {
        displayError(2);
        return;
    }
    for (int i = patientNode->tpatient.nVisits - 1; i >= 0; i--) {
        printf("Visit %d\n", patientNode->tpatient.nVisits - i);
        printf("Arrival: %d/%d/%d %d:%d\n", patientNode->tpatient.visits[i]->tArrival.year, patientNode->tpatient.visits[i]->tArrival.month, patientNode->tpatient.visits[i]->tArrival.day, patientNode->tpatient.visits[i]->tArrival.hour, patientNode->tpatient.visits[i]->tArrival.min);
        if (patientNode->tpatient.visits[i]->tDismissed.year != -1) {
            printf("Dismissed: %d/%d/%d %d:%d\n", patientNode->tpatient.visits[i]->tDismissed.year, patientNode->tpatient.visits[i]->tDismissed.month, patientNode->tpatient.visits[i]->tDismissed.day, patientNode->tpatient.visits[i]->tDismissed.hour, patientNode->tpatient.visits[i]->tDismissed.min);
            printf("Duration: %.0f:%.0f\n", patientNode->tpatient.visits[i]->duration / 60, fmod(patientNode->tpatient.visits[i]->duration, 60));
            printf("Doctor: %s\n", patientNode->tpatient.visits[i]->doctor->name);
            printf("Summary: %s\n", patientNode->tpatient.visits[i]->vSummary);
        } else {
            printf("Dismissed: Not Dismissed\n");
        }
    }
}

// Function to display the details of patients in the queue
void displayPatientsInLine() {
    if (pLine == NULL) {
        displayError(6);
        return;
    }
    displayQueue(pLine);
}

// Function to move a patient to the front of the queue
void advancePatientInLine() {
    if (pLine == NULL) {
        displayError(6);
        return;
    }
    displayQueue(pLine);
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    movePatientToFront(&pLine, ID);
    displayPatientsInLine();
}

// Function to display all doctors in the linked list
void displayAllDoctors1() {
    if (pDoc == NULL) {
        displayError(7);
        return;
    }
    displayAllDoctors(pDoc);
}

// Function to display the details of patients assigned to a doctor
void displayAssignedPatients() {
    char license[MAX_BUFFER_SIZE];
    printf("Enter the doctor's license: ");
    scanf(" %[^\n]", license);

    DocNode *doctorNode = searchDoctor(pDoc, license);
    if (doctorNode == NULL) {
        displayError(3);
        return;
    }

    printf("Patients assigned to Dr. %s:\n", doctorNode->doctor.name);
    displayPatientsOfDoctor(pTree, doctorNode->doctor.nLicense);
}

void displayPatientsOfDoctor(pInTree *pTree, char *license) {
    if (pTree == NULL) {
        return;
    }
    for (int i = 0; i < pTree->tpatient.nVisits; i++) {
        if (strcmp(pTree->tpatient.visits[i]->doctor->nLicense, license) == 0) {
            printf("Name: %s\n", pTree->tpatient.name);
            printf("ID: %s\n", pTree->tpatient.ID);
            printf("Arrival: %d/%d/%d %d:%d\n", pTree->tpatient.visits[i]->tArrival.year, pTree->tpatient.visits[i]->tArrival.month, pTree->tpatient.visits[i]->tArrival.day, pTree->tpatient.visits[i]->tArrival.hour, pTree->tpatient.visits[i]->tArrival.min);
            printf("Dismissed: %d/%d/%d %d:%d\n", pTree->tpatient.visits[i]->tDismissed.year, pTree->tpatient.visits[i]->tDismissed.month, pTree->tpatient.visits[i]->tDismissed.day, pTree->tpatient.visits[i]->tDismissed.hour, pTree->tpatient.visits[i]->tDismissed.min);
            printf("Summary: %s\n", pTree->tpatient.visits[i]->vSummary);
            printf("\n");
        }
    }
    displayPatientsOfDoctor(pTree->left, license);
    displayPatientsOfDoctor(pTree->right, license);
}

// Function to discharge a patient from the emergency room
void dischargePatient() {
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    pInTree *patientNode = searchPatient(pTree, ID);
    if (patientNode == NULL) {
        displayError(2);
        return;
    }
    if (patientNode->tpatient.nVisits == 0 || patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.year != -1) {
        displayError(6);
        return;
    }
    time_t now = time(NULL);
    struct tm *current = localtime(&now);
    patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.year = current->tm_year + 1900;
    patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.month = current->tm_mon + 1;
    patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.day = current->tm_mday;
    patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.hour = current->tm_hour;
    patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.min = current->tm_min;
    patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->duration = difftime(now, mktime(current)) / 60;
    Patient *patient = dequeuePatient(&pLine);
    patient->visits[patient->nVisits - 1]->doctor->nPatients--;
    displayPatientsInLine();
}

// Function to remove a visit from a patient's visit stack
void removeVisit() {
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    pInTree *patientNode = searchPatient(pTree, ID);
    if (patientNode == NULL) {
        displayError(2);
        return;
    }
    if (patientNode->tpatient.nVisits == 0 || patientNode->tpatient.visits[patientNode->tpatient.nVisits - 1]->tDismissed.year == -1) {
        displayError(6);
        return;
    }
    Date date;
    printf("Enter the visit date (YYYY/MM/DD HH:MM): ");
    scanf("%d/%d/%d %d:%d", &date.year, &date.month, &date.day, &date.hour, &date.min);
    int i;
    for (i = 0; i < patientNode->tpatient.nVisits; i++) {
        if (patientNode->tpatient.visits[i]->tArrival.year == date.year && patientNode->tpatient.visits[i]->tArrival.month == date.month && patientNode->tpatient.visits[i]->tArrival.day == date.day && patientNode->tpatient.visits[i]->tArrival.hour == date.hour && patientNode->tpatient.visits[i]->tArrival.min == date.min) {
            break;
        }
    }
    if (i == patientNode->tpatient.nVisits) {
        displayError(2);
        return;
    }
    freeMemory(patientNode->tpatient.visits[i]->vSummary);
    freeMemory(patientNode->tpatient.visits[i]);
    for (int j = i; j < patientNode->tpatient.nVisits - 1; j++) {
        patientNode->tpatient.visits[j] = patientNode->tpatient.visits[j + 1];
    }
    patientNode->tpatient.nVisits--;
    displayAllAdmissions();
}

// Function to remove a patient from the emergency room
void removePatient() {
    char ID[11];
    printf("Enter the patient's ID: ");
    scanf("%s", ID);
    pInTree *patientNode = searchPatient(pTree, ID);
    if (patientNode == NULL) {
        displayError(2);
        return;
    }
    deletePatient(&pTree, ID);
    displayAllPatients(pTree);
}

// Free all patients in the BST
void freeAllPatients(pInTree *pTree) {
    if (pTree == NULL) {
        return;
    }
    freeAllPatients(pTree->left);
    freeAllPatients(pTree->right);
    for (int i = 0; i < pTree->tpatient.nVisits; i++) {
        freeMemory(pTree->tpatient.visits[i]->vSummary);
        freeMemory(pTree->tpatient.visits[i]);
    }
    freeMemory(pTree->tpatient.name);
    freeMemory(pTree);
}

// Free all patients in the queue
void freeAllPatientsInLine(pInLine *pLine) {
    if (pLine == NULL) {
        return;
    }
    freeAllPatientsInLine(pLine->next);
    freeMemory(pLine);
}

// Free all doctors in the linked list
void freeAllDoctors(DocNode *pDoc) {
    if (pDoc == NULL) {
        return;
    }
    freeAllDoctors(pDoc->next);
    freeMemory(pDoc);
}

// Function to close the emergency room
void closeHospital() {
    freeAllPatients(pTree);
    freeAllPatientsInLine(pLine);
    freeAllDoctors(pDoc);
    exit(0);
}

// Function to save data to the respective files
void saveData() {
    updateFiles(pTree, pLine, pDoc);
}

// Function to display the main menu design it in frame
void displayMenu() {
    printf("Emergency Room Management System\n");
    printf("====================================\n");
    printf("= Menu Options                     =\n");
    printf("====================================\n");
    printf("= 1. Admit Patient                 =\n");
    printf("= 2. Check Patient Allergies       =\n");
    printf("= 3. Display All Patients          =\n");
    printf("= 4. Display All Admissions        =\n");
    printf("= 5. Display Patients in Line      =\n");
    printf("= 6. Advance Patient in Line       =\n");
    printf("= 7. Display All Doctors           =\n");
    printf("= 8. Display Assigned Patients     =\n");
    printf("= 9. Discharge Patient             =\n");
    printf("= 10. Remove Visit                 =\n");
    printf("= 11. Remove Patient               =\n");
    printf("= 12. Close Hospital               =\n");
    printf("= 0. Exit                          =\n");
    printf("=====================================\n");
}

// Function to display the main menu
void mainMenu() {
    int choice;
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            admitPatient();
            break;
        case 2:
            checkPatientAllergies();
            break;
        case 3:
            displayAllPatients1();
            break;
        case 4:
            displayAllAdmissions();
            break;
        case 5:
            displayPatientsInLine();
            break;
        case 6:
            advancePatientInLine();
            break;
        case 7:
            displayAllDoctors1();
            break;
        case 8:
            displayAssignedPatients();
            break;
        case 9:
            dischargePatient();
            break;
        case 10:
            removeVisit();
            break;
        case 11:
            removePatient();
            break;
        case 12:
            closeHospital();
            break;
        case 0:
            saveData();
            closeHospital();
            break;
        default:
            displayError(1);
            break;
        }
    } while (choice != 0);
}

// Function to initialize the emergency room
void initEmergencyRoom() {
    loadPatients(&pTree);
    loadLine(&pLine);
    loadDoctors(&pDoc);
    mainMenu();
}

// Main function
int main() {
    initEmergencyRoom();
    return 0;
}