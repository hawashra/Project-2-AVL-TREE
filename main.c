#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "stdbool.h"

#define NAME "Hamza Awashra"
#define ID "1201619"

#undef NAME
#undef ID

#define MAX_COURSE_CODE 30
#define MAX_COURSE_NAME 50
#define MAX_DEPARTMENT_NAME 50

#define MAX_COURSE_LINE 512
#define MAX_TOPICS_LINE 256

void addNewCourse();

typedef struct courseAVL_Node *AVLNode;
typedef struct node_str *topicNode;


AVLNode data = NULL;

struct node_str {
    char elem[MAX_COURSE_CODE];
    struct node_str *next;
};

struct course {
    char courseName[MAX_COURSE_NAME];
    int creditHours;
    char courseCode[MAX_COURSE_CODE];
    char department[MAX_DEPARTMENT_NAME];
    struct node_str *topics_list;
};

struct courseAVL_Node {
    struct course Element;
    AVLNode Left;
    AVLNode Right;
    int Height; //Balance information
};

void push(struct node_str* head, char* elem);
char* pop(struct node_str* head);
int isEmpty(struct node_str* head);
AVLNode Insert(AVLNode node, struct course course);
void printCourseTopics(topicNode topics);
void readFileContent();
void PrintInOrder(AVLNode t);
char *trimmed(char *str);
AVLNode findByCode(char code[], AVLNode T);
topicNode findPrev(topicNode topic, char topicName[]);
void deleteTopic(AVLNode T);
void addTopic(AVLNode T);
void printDepartmentCourses();
void listCourseTopics();
int getBalance(AVLNode T);
AVLNode deleteNode(AVLNode root, char key[]);
void deleteACourse();
void deleteCoursesThatStartWithLetter();
void insertSameDepartmentCodesToStack(AVLNode t, struct node_str* stack, char depName[]);
void deleteCoursesInDepartment();
void removeLineAtEnd(char string[]);
void printInFile(AVLNode T, FILE *fptr);
void saveToFile();
void insertSameLetterCodesToStack(AVLNode t, struct node_str* stack, char letter);
void updateCourse();
AVLNode MakeEmpty(AVLNode T);
void insertTopicLast(struct node_str *head, char topic[]);
void printSameDepartmentCourses(AVLNode t, char depName[]);
void traverseAndCallInsertByName (AVLNode originalTree, AVLNode treeByName);
AVLNode insertIntoBST(AVLNode root, struct course course);
void printSortedByCourseName ();

AVLNode findByName(char name[50], AVLNode T);

AVLNode rightRotate(AVLNode pNode);

AVLNode leftRotate(AVLNode pNode);

AVLNode newNode(struct course course);

AVLNode treeThatInsertInItIsByCourseName = NULL;

int main() {

    data = MakeEmpty(data);
    while (!false) {
        printf("\n1. Read the file courses.txt file and create the tree.\n"
               "2. Insert a new course from the user with all its associated data.\n"
               "3. findByCode a course and support updating of its information.\n"
               "4. List courses in lexicographic order with their associated information (credit hours, IDs, and topics).\n"
               "5. List all topics associated with a given course.\n"
               "6. List all courses in lexicographic order that belong to the same department.\n"
               "7. Delete a course.\n"
               "8. Delete all courses that start with a specific letter.\n"
               "9. Delete all courses belong to a given department.\n"
               "10. Save all words in file offered_courses.txt\n"
               "or 0 to close the program.\n");

        printf("Enter choice: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                readFileContent();
                break;

            case 2:
                addNewCourse();
                break;

            case 3:
                updateCourse();
                break;

            case 0:
                MakeEmpty(data);
            case0:
                exit(0);

            case 4:
                printf("\n%-35s%-15s%-15s%s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE", "DEPARTMENT");

                if (data != NULL) {
                    //
                    treeThatInsertInItIsByCourseName = MakeEmpty(treeThatInsertInItIsByCourseName);
                    treeThatInsertInItIsByCourseName = insertIntoBST(treeThatInsertInItIsByCourseName, data->Element);

                    traverseAndCallInsertByName(data, treeThatInsertInItIsByCourseName);
                }

                PrintInOrder(treeThatInsertInItIsByCourseName);

                break;

            case 5:
                listCourseTopics();
                break;

            case 6:
                printDepartmentCourses();
                break;

            case 7:
                deleteACourse();
                break;

            case 8:
                deleteCoursesThatStartWithLetter();
                break;

            case 9:
                deleteCoursesInDepartment();
                break;

            case 10:
                saveToFile();
                break;

            default:
                goto case0;
        }

    }
}

// read content from file and insert to the tree
void readFileContent() {

    FILE *infile = fopen("courses.txt", "r");

    if (infile == NULL)
        printf("Couldn't open the file.");

    char line[MAX_COURSE_LINE];

    while (fgets(line, MAX_COURSE_LINE, infile) != NULL) {

        struct course course;
        course.topics_list = (struct node_str *) malloc(sizeof(struct node_str));
        course.topics_list->next = NULL;

        char topicsLine[MAX_TOPICS_LINE];

        // scan the line using regex and store data
        sscanf(line, "%[^:]:%d#%[^#]#%[^/]/%[^\n]", course.courseName, &course.creditHours, course.courseCode,
               course.department, topicsLine);
        // split each token to insert it in the linked list of topics
        char *token;
        token = strtok(topicsLine, ",");
        insertTopicLast(course.topics_list, trimmed(token));

        while (token && *token) {
            token = strtok(NULL, ",");
            if (token && *token) {
                insertTopicLast(course.topics_list, trimmed(token));
            }
        }
        // insert the course into the tree
        data = Insert(data, course);
    }
    fclose(infile);
}
// method to insert the topic to the end of the linked list (to maintain the original order)
void insertTopicLast(struct node_str *head, char topic[]) {

    if (head == NULL) {
        printf("NULL head");
        return;
    }

    topicNode t = (topicNode) malloc(sizeof(struct node_str));
    strcpy(t->elem, topic);
    t->next = NULL;

    topicNode temp = head;
    while (temp->next)
        temp = temp->next;

    temp->next = t;
}

// method to delete all nodes in a tree and returns NULL
AVLNode MakeEmpty(AVLNode T) {
    if (T != NULL) {
        MakeEmpty(T->Left);
        MakeEmpty(T->Right);
        free(T);
    }
    return NULL;
}

// method to find the minimal node in the tree (based on the course code)
AVLNode FindMin(AVLNode T) {
    if (T == NULL)
        return NULL;
    else if (T->Left == NULL)
        return T;
    else
        return FindMin(T->Left);
}
// method to find the maximum node in the tree (based on the course code)
AVLNode FindMax(AVLNode T) {
    if (T == NULL)
        return NULL;
    else if (T->Right == NULL)
        return T;
    else
        return FindMax(T->Right);
}
// method to return the height of a tree
int height( AVLNode  N)
{
    if (N == NULL)
        return 0;

    return N->Height;
}

// method to compare two integers and return the greater one
int Max(int Lhs, int Rhs) {
    return Lhs > Rhs ? Lhs : Rhs;
}

AVLNode Insert(AVLNode node, struct course course)
{
    /* 1.  Perform the normal BST rotation */
    if (node == NULL)
        return(newNode(course));

    if (strcmp(course.courseCode, node->Element.courseCode) < 0)
        node->Left  = Insert(node->Left, course);
    else if (strcmp(course.courseCode, node->Element.courseCode) > 0)
        node->Right = Insert(node->Right, course);
    else // Equal keys not allowed
        return node;

    /* 2. Update height of this ancestor node */
    node->Height = 1 + Max(height(node->Left),
                           height(node->Right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced, then there are 4 cases

    // Left - Left Case
    if (balance > 1 && strcmp(course.courseCode, node->Left->Element.courseCode) < 0)
        return rightRotate(node);

    // Right - Right Case
    if (balance < -1 && strcmp(course.courseCode, node->Right->Element.courseCode) > 0)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && strcmp(course.courseCode, node->Left->Element.courseCode) > 0)
    {
        node->Left =  leftRotate(node->Left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && strcmp(course.courseCode, node->Right->Element.courseCode) < 0)
    {
        node->Right = rightRotate(node->Right);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

AVLNode newNode(struct course course) {
    AVLNode node = (AVLNode)malloc(sizeof(struct courseAVL_Node));

    node->Element =course;
    node->Left   = NULL;
    node->Right  = NULL;
    node->Height = 1;  // new node is initially added at leaf
    return(node);
}
/* END */

//in-order print to see the elements
void PrintInOrder(AVLNode t) {
    if (t != NULL) {
        PrintInOrder(t->Left);

//        printf("COURSE TOPICS:\n");


        printf("\n%-35s%-15d%-15s%s\n", t->Element.courseName, t->Element.creditHours, t->Element.courseCode,
               t->Element.department);
        printf("course topics are: \n");

        printCourseTopics(t->Element.topics_list);
        printf("\n");
        printf("...........................................................................................................");

        PrintInOrder(t->Right);
    }
}

// method to print a linked list of topics (strings)
void printCourseTopics(topicNode topics) {

    if (topics == NULL) {
        printf("NULL pointer elem");
        return;
    }

    topicNode temp = topics->next;

    while (temp != NULL) {

        if (temp->next == NULL) {
            printf("%s\n", temp->elem);
            return;
        }

        printf("%s, ", temp->elem);
        temp = temp->next;
    }
}

// method to return a trimmed copy of the string (remove leading and trailing spaces)
char *trimmed(char *str) {

    while (isspace((unsigned char) *str)) {
        str++;
    }
    if (*str == 0)
        return str;

    char *last;
    last = str + strlen(str) - 1;
    while ((last > str) && (isspace((unsigned char) *last))) {
        last--;
    }

    last[1] = '\0';

    /*return new trimmed string */
    return str;
}

// method to add a new course from the user
void addNewCourse() {

    getchar();

    struct course course;
    course.topics_list = (topicNode) malloc(sizeof(struct course));
    course.topics_list->next = NULL;

    printf("Enter the course information: ");
    printf("Course name, credit hours, course code, department/topic1, topic2...,topicN\n\n");

    char input[MAX_COURSE_LINE];
    // read the line in the specified format
    fgets(input, MAX_COURSE_LINE, stdin);
    char *topics_line1;
    // use str tok to split the line and take the topics line
    topics_line1 = strtok(input, "/");
    free(topics_line1); // takes the first part (we don't need it)
    topics_line1 = strtok(NULL, "\n"); // takes the thing after the / (which is the topics separated by commas).

    // trim the topics line
    char topics_line[MAX_TOPICS_LINE];
    strcpy(topics_line, trimmed(topics_line1));
    // split the topics to store them
    char *topic_i = strtok(topics_line, ",");
    insertTopicLast(course.topics_list, trimmed(topic_i));

    while (topic_i && *topic_i) {
        topic_i = strtok(NULL, ",");

        if (topic_i && *topic_i)
            insertTopicLast(course.topics_list, trimmed(topic_i));

    }
    // store the data by reading the line using regex
    sscanf(input, "%[^,], %d, %[^,], %[^/]/", course.courseName,
           &course.creditHours, course.courseCode, course.department);

    printf("\n\n%-35s%-15d%-20s%s\n\n", course.courseName, course.creditHours, course.courseCode, course.department);

    printCourseTopics(course.topics_list);
    printf("\n\n--------------------------------------------------------------------------------------------------\n\n");

    data = Insert(data, course); // insert the course to the tree
}

void updateCourse() {

    printf("Enter course code: ");
    getchar();
    char courseCode[MAX_COURSE_CODE];
    fgets(courseCode, MAX_COURSE_CODE, stdin);
    removeLineAtEnd(courseCode);


    printf("\n1. Update course name .\n"
           "2. Update course credit hours.\n"
           "3. Update course topics.\n"
           "4. Update course department.\n"
           "5. Update course code.\n"
    );
    // find the course to update it
    AVLNode T = findByCode(courseCode, data);

    if (!T) {
        printf("No such course");
        return;
    }

    printf("\nEnter what you want to edit: \n");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        getchar();
        printf("\nEnter new name for the course: \n");
        char newName[50];
        fgets(newName, 50, stdin);
        removeLineAtEnd(newName);

        memmove(T->Element.courseName, newName, 50); // change the name using memmove.

    } else if (choice == 2) {
        printf("Enter new course credit hours: ");
        int newCreditHours;
        scanf("%d", &newCreditHours);
        T->Element.creditHours = newCreditHours;


    } else if (choice == 3) {
        printf("\ndo you want to add a elem or delete?, choose 'd' for delete and 'a' for add\n");
        getchar();
        char choice2;
        scanf("%c", &choice2);

        printf("\n");
        if (choice2 == 'd' || choice2 == 'D') {
            deleteTopic(T);

        }

        else if (choice2 == 'A' || choice2 == 'a')
            addTopic(T);

        else
            printf("\ninvalid choice in adding or deleting.\n");

    } else if (choice == 4) {
        getchar();
        printf("\nEnter new name for the department: \n");
        char newDep[50];
        fgets(newDep, 50, stdin);
        removeLineAtEnd(newDep);

        memmove(T->Element.department, newDep, 50);

    } else if (choice == 5) {
        getchar();
        printf("Enter course new code: ");

        char newCode[MAX_COURSE_CODE];
        fgets(newCode, MAX_COURSE_CODE, stdin);
        removeLineAtEnd(newCode);

        // COPY DATA TO A NEW STRUCT, UPDATE CODE, DELETE OLD CODE COURSE, THEN INSERT THE COURSE AFTER UPDATING ITS CODE
        struct course course_updated;

        strcpy(course_updated.courseName, T->Element.courseName);
        strcpy(course_updated.department, T->Element.department);
        course_updated.creditHours = T->Element.creditHours;
        course_updated.topics_list = T->Element.topics_list;

//        course_updated = T->Element;

        memmove(course_updated.courseCode, newCode, 30);
        // DELETE AND INSERT AGAIN
        data = deleteNode(data, T->Element.courseCode);
        data = Insert(data, course_updated);


    } else {
        printf("\nInvalid choice\n");
    }
}

// FIND A NODE USING ITS CODE, RETURN NULL IF NOT FOUND
AVLNode findByCode(char code[], AVLNode T) {
    if (T == NULL)
        return NULL;
    if (strcmp(code, T->Element.courseCode) < 0)
        return findByCode(code, T->Left);
    else if (strcmp(code, T->Element.courseCode) > 0)
        return findByCode(code, T->Right);
    else
        return T;
}

// METHOD TO DELETE A TOPIC IN A COURSE
void deleteTopic(AVLNode T) {

    printf("Enter the elem name you want to delete: ");
    getchar();
    char topicName[MAX_COURSE_NAME];
    fgets(topicName, MAX_COURSE_NAME, stdin);
    removeLineAtEnd(topicName);

    topicNode prev = findPrev(T->Element.topics_list, topicName);
    topicNode temp = prev->next;
    prev->next = temp->next;
    free(temp);

    printf("Topic deleted successfully");
}
// METHOD TO FIND PREVIOUS NODE OF A TOPIC NODE
topicNode findPrev(topicNode topic, char topicName[]) {

    if (topic == NULL) {
        printf("Error, no header");
        return NULL;
    }

    topicNode temp = topic;

    while (temp->next != NULL && strcmp(temp->next->elem, topicName) != 0)
        temp = temp->next;

    return temp;
}

// METHOD TO INSERT A NEW NODE
void addTopic(AVLNode T) {

    printf("Enter the elem name you want to add");
    getchar();
    char topicName[MAX_COURSE_NAME];
    fgets(topicName, MAX_COURSE_NAME, stdin);
    removeLineAtEnd(topicName);

    topicNode temp = (topicNode) malloc(sizeof(struct node_str));
    temp->next = NULL;

    insertTopicLast(T->Element.topics_list, topicName);
}
// METHOD TO PRINT TOPICS OF A COURSE
void listCourseTopics() {
    getchar();
    printf("\nEnter course code: ");

    char courseCode[MAX_COURSE_CODE];

    fgets(courseCode, MAX_COURSE_CODE, stdin);
    removeLineAtEnd(courseCode);

    printf("\n\n");


    AVLNode temp = findByCode(courseCode, data);
    if (temp != NULL) {
        printf("\n%s topics are: ", temp->Element.courseName);
        printCourseTopics(temp->Element.topics_list);
        printf("\n");
    } else {
        printf("NULL");
    }

}
// METHOD TO PRINT SAME DEPARTMENT COURSES
void printDepartmentCourses() {
    printf("Enter department name: ");
    char depName1[MAX_DEPARTMENT_NAME];
    getchar();
    fgets(depName1, MAX_DEPARTMENT_NAME, stdin);
    removeLineAtEnd(depName1);
    printSameDepartmentCourses( treeThatInsertInItIsByCourseName, trimmed(depName1));

}

// this method traverses on the avl tree in-order to print courses with department name as depName.
void printSameDepartmentCourses(AVLNode t, char depName[]) {

    if (t != NULL) {
        printSameDepartmentCourses(t->Left, depName);

        if (strcasecmp(t->Element.department, depName) == 0) {
            printf("%-30s%-7d%-15s%-15s\n", t->Element.courseName, t->Element.creditHours, t->Element.courseCode,
                   t->Element.department);
            printCourseTopics(t->Element.topics_list);
            printf("\n");
        }

        printSameDepartmentCourses(t->Right, depName);
    }
}

// method to delete a course by entering the code of it
void deleteACourse() {
    getchar();

    printf("Enter course code: ");
    char code[MAX_COURSE_CODE];
    fgets(code, MAX_COURSE_CODE, stdin);
    removeLineAtEnd(code);

    // call the method for delete
    data = deleteNode(data, code);

}

// this function deletes a node from the tree
AVLNode deleteNode(AVLNode root, char key[]) {
    // STEP 1: PERFORM STANDARD BST DELETE
    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if (strcmp(key, root->Element.courseCode) < 0)
        root->Left = deleteNode(root->Left, key);

    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    if (strcmp(key, root->Element.courseCode) > 0)
        root->Right = deleteNode(root->Right, key);

        // if key is same as root's key, then This is
        // the node to be deleted

    else if (strcmp(key, root->Element.courseCode) == 0) {
        // node with only one child or no child
        if ((root->Left == NULL) || (root->Right == NULL)) {
            AVLNode temp = root->Left ? root->Left : root->Right;

            // No child case
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else  // One child case
            {
                *root = *temp; // Copy the contents of
            }
            // the non-empty child
            free(temp);
        } else {

            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            AVLNode temp = FindMin(root->Right);

            // Copy the inorder successor's data to this node
            strcpy(root->Element.courseCode, temp->Element.courseCode);
            strcpy(root->Element.courseName, temp->Element.courseName);
            strcpy(root->Element.department, temp->Element.department);
            root->Element.creditHours = temp->Element.creditHours;
            root->Element.topics_list = temp->Element.topics_list;

            // Delete the inorder successor
            root->Right = deleteNode(root->Right, temp->Element.courseCode);
        }
    }
    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->Height = 1 + Max(height(root->Left), height(root->Right));
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left of Left Case
    if (balance > 1 && getBalance(root->Left) >= 0)
        return rightRotate(root);

    // Left of Right Case
    if (balance > 1 && getBalance(root->Left) < 0) {
        return rightRotate(root);
    }

    // Right of Right Case
    if (balance < -1 && getBalance(root->Right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->Right) > 0) {
        return leftRotate(root);
    }

    return root;
}

// this method calculates the balance of a node T in the AVL tree
int getBalance(AVLNode T) {
    if (T == NULL)
        return 0;
    return height(T->Left) - height(T->Right);
}

void deleteCoursesThatStartWithLetter() {
    getchar(); // discard overflowed char
    printf("Enter the letter: ");
    char letter;
    scanf("%c", &letter);
    // make a stack to store all courses in it
    struct node_str* stack = (struct node_str*) malloc(sizeof(struct node_str));
    stack->next = NULL;
    // insert all the nodes in the tree into the stack
    insertSameLetterCodesToStack(data, stack,letter);

    while (!isEmpty(stack)) {
        data = deleteNode(data, pop(stack));
    }
}


void deleteCoursesInDepartment() {
    getchar();
    printf("Enter the department name: ");
    char departmentName[MAX_DEPARTMENT_NAME];
    fgets(departmentName, MAX_DEPARTMENT_NAME, stdin);
    removeLineAtEnd(departmentName);

    // make a stack to store all AVL tree nodes in it
    struct node_str* stack = (struct node_str*) malloc(sizeof(struct node_str));
    stack->next = NULL;
    // insert all AVL tree nodes to the stack
    insertSameDepartmentCodesToStack(data, stack, departmentName);

    while (!isEmpty(stack)) {
        data = deleteNode(data, pop(stack));
    }

}

// method to insert same department courses codes to as stack (for deletion)
void insertSameDepartmentCodesToStack(AVLNode t, struct node_str* stack, char depName[]) { // delete in postorder
//    printf("%-20s%-15s%-20s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE");
    if (t != NULL) {
        insertSameDepartmentCodesToStack(t->Left, stack, depName);
        insertSameDepartmentCodesToStack(t->Right, stack,depName);
        if (strcmp(t->Element.department,depName ) == 0 )
            push(stack, t->Element.courseCode);
    }
}
// method to insert course that start with a specific letter to the stack (for deletion)
void insertSameLetterCodesToStack(AVLNode t, struct node_str* stack, char letter) { // delete in postorder
//    printf("%-20s%-15s%-20s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE");
    if (t != NULL) {
        insertSameLetterCodesToStack(t->Left, stack, letter);
        insertSameLetterCodesToStack(t->Right, stack, letter);
        if (letter == t->Element.courseName[0])
            push(stack, t->Element.courseCode);
    }
}

// this method is used to remove the line separator at the end of the string. usually used after fgets.
void removeLineAtEnd(char string[]) {
    char *ptr = string;

    while (*ptr != '\n' && *ptr != '\0')
        ptr++;

    if (*ptr == '\n')
        *ptr = '\0';
}


// check if the stack is empty
int isEmpty(struct node_str* head) {

    return head->next == NULL;
}

// pop the last element added from the stack
char* pop(struct node_str* head) {

    if (!isEmpty(head)) {
        struct node_str* temp = head->next;
        head->next = head->next->next;

        return temp->elem;
    }

    printf("Empty stack");
    return NULL;
}

// push an element to the stack
void push(struct node_str* head, char* elem) {

    if (head == NULL) {
        printf("NULL reference, cannot push");
        return;
    }

    struct node_str* temp = (struct node_str*) malloc(sizeof(struct node_str));
    temp->next = NULL;
    strcpy(temp->elem, elem);

    temp->next = head->next;
    head->next = temp;

}

// Method to save the results onto the output file.
void saveToFile() {

    FILE *ptrOut = fopen("output.txt", "w");

    if (!ptrOut)
        printf("output file not found!");

    printInFile(data, ptrOut);

    fclose(ptrOut);

}
// method to print courses to the file
void printInFile(AVLNode T, FILE *fptr) {

    if (T != NULL) {

        printInFile(T->Left, fptr);

        fprintf(fptr, "%s:%d#%s#%s/", T->Element.courseName, T->Element.creditHours, T->Element.courseCode,
                T->Element.department);

        topicNode temp = T->Element.topics_list->next;

        while (temp != NULL) {
            if (temp->next != NULL)
                fprintf(fptr, "%s, ", temp->elem);
            else
                fprintf(fptr, "%s\n", temp->elem);

            temp = temp->next;
        }
        printInFile(T->Right, fptr);
    }
}

// d_done


// method to call insert by name method on all nodes in the original tree (to print them in a sorted order by name)
void traverseAndCallInsertByName (AVLNode originalTree, AVLNode treeByName) {

    if (originalTree != NULL) {
        traverseAndCallInsertByName (originalTree->Left, treeByName);
        (treeByName) = insertIntoBST(treeByName, originalTree->Element);
        traverseAndCallInsertByName (originalTree->Right, treeByName);
    }

    // else -> return to last caller
}

// insert method to insert by name (to print nodes sorted by name)
AVLNode insertIntoBST(AVLNode root, struct course course) {
    if (!root) {
        AVLNode new_node = malloc(sizeof(struct courseAVL_Node));
        new_node->Element = course;
        new_node->Left = NULL;
        new_node->Right = NULL;
        return new_node;
    }

    if (strcmp(root->Element.courseName, course.courseName) > 0 )
        root->Right = insertIntoBST(root->Right, course);
    else if (strcmp(root->Element.courseName, course.courseName) < 0)
        root->Left= insertIntoBST(root->Left, course);

    return root;
}


// a function that does a left rotation
AVLNode leftRotate(AVLNode x)
{
    AVLNode y = x->Right;
    AVLNode T2 = y->Left;

    // Perform rotation
    y->Left = x;
    x->Right = T2;

    //  Update heights
    x->Height = Max(height(x->Left), height(x->Right))+1;
    y->Height = Max(height(y->Left), height(y->Right))+1;

    // Return new root
    return y;
}
//  function that does a right rotation
AVLNode rightRotate(AVLNode y)
{
    AVLNode x = y->Left;
    AVLNode T2 = x->Right;

    // Perform rotation
    x->Right = y;
    y->Left = T2;

    // Update heights
    y->Height = Max(height(y->Left), height(y->Right))+1;
    x->Height = Max(height(x->Left), height(x->Right))+1;

    // Return new root
    return x;
}
