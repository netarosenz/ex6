/******************
Name: Neta Rosenzweig
ID: 323885582
Assignment: ex.6
*******************/

#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
#define MAX_POKEMON 151
#define MAX_NAME_LENGTH 20

int main() {
    mainMenu();
    freeAllOwners();
    return 0;
}

void mainMenu() {
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice) {
            case 1:
                openPokedexMenu();
            break;
            case 2:
                if(!ownerHead) {
                    printf("No existing Pokedexes.\n");
                    break;
                }
                enterExistingPokedexMenu();
            break;
            case 3:
                if(!ownerHead) {
                    printf("No existing Pokedexes to delete.\n");
                    break;
                }
                deletePokedex();
            break;
            case 4:
                if(!ownerHead) {
                    printf("Not enough owners to merge.\n");
                    break;
                }
                mergePokedexMenu();
            break;
            case 5:
                if(!ownerHead || !ownerHead->next) {
                    printf("0 or 1 owners only => no need to sort.\n");
                    break;
                }
                sortOwners();
            break;
            case 6:
                if(!ownerHead) {
                    printf("No owners.\n");
                    break;
                }
                printOwnersCircular();
            break;
            case 7:
                printf("Goodbye!\n");
            break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 7);
}

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str) {
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0) {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src) {
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest) {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;

}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type) {
    switch (type) {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput() {
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (size + 1 >= capacity) {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp) {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node) {
    // Check if the node exist
    if (!node) {
        return;
    }
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// Creating a new pokemon node
PokemonNode *createPokemonNode(const PokemonData *data) {
    // Check if the data is valid
    if (data == NULL) {
        exit(1);
        }
    // Allocate memory for new pokemon node
    PokemonNode* newNode = (PokemonNode*) malloc(sizeof(PokemonNode));
    // Check if the allocation failed
    if (newNode == NULL) {
        printf("Allocation error.\n");
        exit(1);
    }
    // Allocate memory for the pokemon data
    PokemonData* tempData = (PokemonData*) malloc(sizeof(PokemonData));
    // Check if the allocation failed
    if (tempData == NULL) {
        // Free the memory that already allocated and get out
        free(newNode);
        exit(1);
    }
    // Use temp to keep the data
    *(tempData) = *data;
    newNode->data = tempData;
    // Initialize the left and right pointers
    newNode->left = NULL;
    newNode->right = NULL;
    // return the new node that we created
    return newNode;
}

// Free a single pokemon node
void freePokemonNode(PokemonNode *node) {
    // Check if there is data to free in the node
    if (node == NULL) {
        // If there isn't data to free get out of the func
        return;
    }
    // Free the data in the node
    free(node->data);
    // Finally, free the node itself
    free(node);
}

// Create a new single owner node for the circular linked list
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    // Check if owner name is NULL or empty
    if (ownerName == NULL || ownerName[0] == '\0') {
        return NULL;
    }
    // Check name length
    if (strlen(ownerName) > MAX_NAME_LENGTH) {
        return NULL;
    }
    // Allocate memory for the owner node
    OwnerNode *newNode = malloc(sizeof(OwnerNode));
    // Check if the allocation failed
    if (newNode == NULL) {
        return NULL;
    }
    // Copy the owner name into the new owner node struct
    newNode->ownerName = myStrdup(ownerName);
    // Check if the name copied failed
    if(newNode->ownerName == NULL) {
        // If it's failed, free the memory that already allocated
        free(newNode);
        return NULL;
    }
    // Initialize the pokemon that we already created and placed in the root of the binary tree
    newNode->pokedexRoot = starter;
    // Initialize the pointers of the owner's linked list
    newNode->next = NULL;
    newNode->prev = NULL;
    // The func return the new owner node that just created
    return newNode;
}

// Create the circular linked list of the owners
void linkOwnerInCircularList(OwnerNode *newOwner) {
    // Check if the node exist
    if (newOwner == NULL) {
        return;
    }
    // Check duplicates pokedex names
    if (ownerHead != NULL) {
        OwnerNode *current = ownerHead;
        do {
            if (strcmp(current->ownerName, newOwner->ownerName) == 0) {
            // There is pokedex with this name
            return;
            }
            current = current->next;
            // Stop when all the list was checked
        } while (current != ownerHead);
    }
    // Check if there is any owner in the list
    if (ownerHead == NULL) {
        // The new node will place first in the head of the list
        ownerHead = newOwner;
        // Because there is only single node in the list, it next and previous simultaneously
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
        return;
    }
    // The list isn't empty
    OwnerNode *current = ownerHead;
    // Update the current node that will point to the next node
    do {
        current = current->next;
    // Stop the loop when we arrived again to the head node
    } while (current->next != ownerHead);
    // The new node point forward to the head
    newOwner->next = ownerHead;
    // The new node point back to the last current node
    newOwner->prev = current;
    // The last current node point forward to the new node
    current->next = newOwner;
    // The head point back to the new node
    ownerHead->prev = newOwner;
}

// Search specific owner by name
OwnerNode *findOwnerByName(const char *name) {
    // Check if there is name like the one we search
    if (name == NULL) {
        return NULL;
    }
    // Check if there is any owner in the list
    if (ownerHead == NULL) {
        return NULL;
    }
    // Start the search from the head of the list
    OwnerNode* currentNode = ownerHead;
    do {
        // Compare the name in the current node to the name that we are looking for
        if (strcmp(currentNode->ownerName, name) == 0) {
            // If we found the specific name, return it
            return currentNode;
        }
        // Keep searching and move to the next node
        currentNode = currentNode->next;
        // Stop the loop if we've arrived again to the head of the list
    } while (currentNode != ownerHead);
    // The name not found, return null
    return NULL;
    }

// Create a new pokedex for new owner
void openPokedexMenu() {
    printf("Your name: ");
    // Read the owner name by the function thar read string and print it
    char *ownerName = getDynamicInput();
    // Check if the allocation succeeded
    if (ownerName == NULL) {
        // If the allocation failed, get out of the function
        return;
    }
    // Check if the owner name is already exist
    if (findOwnerByName(ownerName)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", ownerName);
        // Free the memory of the owner name
        free(ownerName);
        // Get out of the function
        return;
    }
    printf("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    // Read the choice by the function thar read numbers
    int choice = readIntSafe("Your choice: ");
    // Check if the choice is valid
    if (choice < 1 || choice > 3) {
        // if isn't valid free the memory of the owner name
        free(ownerName);
        // Get out of the function
        return;
    }
    // Array for the starter ID's
    const int starterPokemonID[] = {1, 4, 7};
    // The array start from 0, fix it by -1
    int pokemonID = starterPokemonID[choice - 1];
    // Create new pokemon node by the data from the array of all the pokemons
    PokemonNode *starterPokemon = createPokemonNode(&pokedex[pokemonID - 1]); // need to write the func from the h file createPokemonNode
    // Check if the allocation succeeded
    if (starterPokemon == NULL) {
        free(ownerName);
        return;
    }
    // Create new owner node
    OwnerNode* newOwner = createOwner(ownerName, starterPokemon);
    // Check if the allocation succeeded
    if (newOwner == NULL) {
        // Free the memory of the starter pokemon
        freePokemonNode(starterPokemon); // need to write the func from the h file freePokemonNode
        // Free the memory of the owner name
        free(ownerName);
        // If the allocation failed, get out of the function
        return;
    }
    linkOwnerInCircularList(newOwner); // need to write the func from the h file linkOwnerInCircularList
    printf("New Pokedex created for %s with starter %s.\n", ownerName, pokedex[pokemonID - 1].name);
    free(ownerName);
}

// Search owner by number
OwnerNode* findOwnerByNumber(int choice) {
    // Check if valid
    if (ownerHead == NULL || choice < 1) {
        return NULL;
    }
    // Start the search from the head of the list
    OwnerNode* current = ownerHead;
    int count = 1;
    // Search all over the circular list
    do {
        // If we found the chosen owner
        if (count == choice) {
            // return this owner
            return current;
        }
        count++;
        // Move to the next owner in the list
        current = current->next;
        // Stop when we arrived again to the head
    } while (current != ownerHead);
    // the chosen owner didn't found
    return NULL;
}

// Place the pokemon node in the right position
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    // If the root is empty
    if (root == NULL) {
        // The new node become the root of the tree
        return newNode;
    }
    // If the id that we found is the same id in the root
    if (newNode->data->id == root->data->id) {
        // Free the new node
        freePokemonNode(newNode);
        // return the root
        return root;
    }
    // Place the node in the proper position recursively. smaller- to the left, bigger- to the right.
    if (newNode->data->id < root->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    }
    else if (newNode->data->id > root->data->id) {
        root->right = insertPokemonNode(root->right, newNode);
    }
    // Return the current root
    return root;
}

// Search pokemon by BFS searching
PokemonNode* searchPokemonBFS(PokemonNode* root, int id) {
    // Check if valid
    if (root == NULL || id < 0) {
        return NULL;
    }
    // Create queue for search
    PokemonNode** queue = malloc(MAX_POKEMON * sizeof(PokemonNode*));
    // Check if failed
    if (queue == NULL) {
        return NULL;
    }
    // initial the queue
    int front = 0;
    int rear = 0;
    // Insert the root to queue
    queue[rear++] = root;
    // Search tree
    while (front < rear) {
        // Choose node from the queue
        PokemonNode* current = queue[front++];
        // Check if this is the node we looked for
        if (current->data->id == id) {
            free(queue);
            return current;
        }
        // Insert the 'children' to the queue
        if (current->left && rear < MAX_POKEMON) {
            queue[rear++] = current->left;
        }
        if (current->right && rear < MAX_POKEMON) {
            queue[rear++] = current->right;
        }
    }
    // Free the queue memory
    free(queue);
    // We didn't find the pokemon
    return NULL;
}

// Add pokemon to tree
void addPokemon(OwnerNode *owner) {
    // Check if the owner exist
    if (owner == NULL) {
        return;
    }
    // Get the ID from the user and print it
    int id = readIntSafe("Enter ID to add: ");
    // Check if the ID is valid
    if (id < 1 || id > MAX_POKEMON) {
        printf("Invalid ID.\n");
        return;
    }
    // Check if the pokemon already exist
    PokemonNode* existingPokemon = searchPokemonBFS(owner->pokedexRoot, id);
    // Check if the pointer isn't null
    if (existingPokemon != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }
    // Create new node with the pokemon from the pokedex array according the index
    PokemonNode* newNode = createPokemonNode(&pokedex[id - 1]);
    if (newNode == NULL) {
        exit(1);
    }
    // Insert the new node to the tree
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Pokemon %s (ID %d) added.\n", pokedex[id - 1].name, id);
}

// Generic BFS function
void BFSGeneric(PokemonNode* root, VisitNodeFunc visit) {
    // Check if valid
    if (root == NULL || visit == NULL) {
        return;
    }
    // Create queue for max pokemons
    PokemonNode** queue = malloc(MAX_POKEMON * sizeof(PokemonNode*));
    if (queue == NULL) {
        return;
    }
    // Queue exit location
    int front = 0;
    // Queue entry location
    int rear = 0;
    // Entry the root to queue
    queue[rear++] = root;
    // While the queue not empty
    while (front < rear) {
        // Remove node from the queue
        PokemonNode* current = queue[front++];
        // Visit at node and print
        visit(current);
        // entry the children to queue if they exist
        if (current->left) {
            queue[rear++] = current->left;
        }
        if (current->right) {
            queue[rear++] = current->right;
        }
    }
    // Free the memory of the queue
    free(queue);
}

// Generic pre-order function
void preOrderGeneric(PokemonNode* root, VisitNodeFunc visit) {
    // Check if valid
    if (visit == NULL) {
        return;
    }
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Visit at the current node
    visit(root);
    // Recursive searching of left sub tree
    preOrderGeneric(root->left, visit);
    // Recursive searching of right sub tree
    preOrderGeneric(root->right, visit);
}

// Generic in-order function
void inOrderGeneric(PokemonNode* root, VisitNodeFunc visit) {
    // Check if valid
    if (visit == NULL) {
        return;
    }
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Recursive searching of left sub tree
    inOrderGeneric(root->left, visit);
    // Visit at the current node
    visit(root);
    // Recursive searching of right sub tree
    inOrderGeneric(root->right, visit);
}

// Generic post-order function
void postOrderGeneric(PokemonNode* root, VisitNodeFunc visit) {
    // Check if valid
    if (visit == NULL) {
        return;
    }
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Recursive searching of left sub tree
    postOrderGeneric(root->left, visit);
    // Recursive searching of right sub tree
    postOrderGeneric(root->right, visit);
    // Visit at the current node
    visit(root);
}

// Display the pokemon in the tree by BFS
void displayBFS(PokemonNode* root) {
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Call to the generic function and print the tree
    BFSGeneric(root, printPokemonNode);
}

// Display the pokemon in the tree by pre-order
void preOrderTraversal(PokemonNode* root) {
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Call to the generic function and print the tree
    preOrderGeneric(root, printPokemonNode);
}

// Display the pokemon in the tree by in-order
void inOrderTraversal(PokemonNode* root) {
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Call to the generic function and print the tree
    inOrderGeneric(root, printPokemonNode);
}

// Display the pokemon in the tree by in-order
void postOrderTraversal(PokemonNode* root) {
    // Check if the tree is empty
    if (root == NULL) {
        return;
    }
    // Call to the generic function and print the tree
    postOrderGeneric(root, printPokemonNode);
}

// Initial the nodes array
void initNodeArray(NodeArray* na, int initialCapacity) {
    // Check that the pointer to the array exist
    if (na == NULL || initialCapacity <= 0) {
        return;
    }
    na->nodes = (PokemonNode**)calloc(initialCapacity, sizeof(PokemonNode*));
    // Check the allocate
    if (na->nodes == NULL) {
        // Initial the array
        na->size = 0;
        na->capacity = 0;
    } else {
        na->size = 0;
        na->capacity = initialCapacity;
    }
}

// Add node to the array
void addNode(NodeArray* na, PokemonNode* node) {
    // Check if valid
    if (na == NULL || node == NULL || na->nodes == NULL) {
        return;
    }
    // If the array is already full
    if (na->size == na->capacity) {
        int newCapacity = na->capacity * 2;
        // Increase the array
        PokemonNode** newNodes = realloc(na->nodes, newCapacity * sizeof(PokemonNode*));
        // Check if the allocation failed
        if (!newNodes) {
            return;
        }
        // Update the array with the new node
        na->nodes = newNodes;
        na->capacity = newCapacity;
    }
    // Add the node to the array
    na->nodes[na->size++] = node;
}

// Collect all node from the tree to array
void collectAll(PokemonNode* root, NodeArray* na) {
    // Check the array exist
    if (na == NULL || na->nodes == NULL) {
        return;
    }
    // Check the tree exist
    if (!root) {
        return;
    }
    // Add the node to the array
    addNode(na, root);
    // Collect the left sub tree
    collectAll(root->left, na);
    // Collect the right sub tree
    collectAll(root->right, na);
}

// Compare nodes by name for alphabetical display
int compareByNameNode(const void* a, const void* b) {
    // Check if valid
    if (a == NULL || b == NULL) {
        return 0;
    }
    // Swap the generic pointers to PokemonNode pointers type
    PokemonNode* node1 = *(PokemonNode**)a;
    PokemonNode* node2 = *(PokemonNode**)b;
    // Check if exist
    if (node1 == NULL || node2 == NULL ||
        node1->data == NULL || node2->data == NULL ||
        node1->data->name == NULL || node2->data->name == NULL) {
        return 0;
        }
    // Compare between the names
    return strcmp(node1->data->name, node2->data->name);
}

// Sort by Alphabetical
void displayAlphabetical(PokemonNode* root) {
    // Check the tree isn't empty
    if (!root) {
        return;
    }
    // Create the array and initial it
    NodeArray na;
    initNodeArray(&na, MAX_POKEMON);
    if (na.capacity == 0) {
        return;
    }
    // Collect all the nodes to the array
    collectAll(root, &na);
    // Check if the collect failed
    if (na.size == 0) {
        free(na.nodes);
        return;
    }
    // Sort the array by names
    qsort(na.nodes, na.size, sizeof(PokemonNode*), compareByNameNode);
    // Print the nodes sorted
    for(int i = 0; i < na.size; i++) {
        printPokemonNode(na.nodes[i]);
    }
    // Free the sort from memory
    free(na.nodes);
}

// Print sub menu for Display options
void displayMenu(OwnerNode *owner) {
    // Check if owner exist
    if (owner == NULL) {
        return;
    }
    // Check if there is any pokemons in the tree
    if (!owner->pokedexRoot) {
        printf("Pokedex is empty.\n");
        return;
    }
    // Print the sub menu
    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");
    // Get the choice from the user and print it
    int choice = readIntSafe("Your choice: ");
    // Handle the user choice by calling to the proper func
    switch (choice)
    {
        case 1:
            displayBFS(owner->pokedexRoot);
        break;
        case 2:
            preOrderTraversal(owner->pokedexRoot);
        break;
        case 3:
            inOrderTraversal(owner->pokedexRoot);
        break;
        case 4:
            postOrderTraversal(owner->pokedexRoot);
        break;
        case 5:
            displayAlphabetical(owner->pokedexRoot);
        break;
        default:
            printf("Invalid choice.\n");
    }
}

// Remove pokemon according ID
PokemonNode* removePokemonByID(PokemonNode* root, int id) {
    // Check if the tree is empty
    if (root == NULL) {
        return NULL;
    }
    // Search the node
    if (id < root->data->id) {
        root->left = removePokemonByID(root->left, id);
    }
    else if (id > root->data->id) {
        root->right = removePokemonByID(root->right, id);
    }
    // We found the node to remove
    else {
        // Case 1: there is no children or there is 1 child
        if (root->left == NULL) {
            PokemonNode* temp = root->right;
            freePokemonNode(root);
            return temp;
        }
        else if (root->right == NULL) {
            PokemonNode* temp = root->left;
            freePokemonNode(root);
            return temp;
        }
        // Case 2: 2 children- find the bigger child from the right sub tree
        PokemonNode* successor = root->right;
        while (successor->left != NULL) {
            successor = successor->left;
        }
        // Copy the data from the child
        PokemonData* tempData = root->data;
        root->data = successor->data;
        successor->data = tempData;
        // Delete the child
        root->right = removePokemonByID(root->right, successor->data->id);
    }
    return root;
}

// Free single pokemon
void freePokemon(OwnerNode* owner) {
    // Check if valid
    if (owner == NULL || owner->pokedexRoot == NULL) {
        printf("No Pokemon to release.\n");
        return;
    }
    // Get pokemon's ID from the user
    int id = readIntSafe("Enter Pokemon ID to release: ");
    // Check if the pokemon exist
    PokemonNode* pokemon = searchPokemonBFS(owner->pokedexRoot, id);
    if (pokemon == NULL) {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }
    // Remove the chosen pokemon from the tree
    printf("Removing Pokemon %s (ID %d).\n", pokemon->data->name, id);
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, id);
}

// Fight pokemon
void pokemonFight(OwnerNode* owner) {
    // Check if valid
    if (owner == NULL || owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    // Get the pair of pokemon's ID from the user
    int id1 = readIntSafe("Enter ID of the first Pokemon: ");
    int id2 = readIntSafe("Enter ID of the second Pokemon: ");
    // Search the chosen pokemon in the tree
    PokemonNode* pokemon1 = searchPokemonBFS(owner->pokedexRoot, id1);
    PokemonNode* pokemon2 = searchPokemonBFS(owner->pokedexRoot, id2);
    // Check if the pokemons found
    if (pokemon1 == NULL || pokemon2 == NULL) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    // Calculate the scores of the pokemons
    double score1 = pokemon1->data->attack * 1.5 + pokemon1->data->hp * 1.2;
    double score2 = pokemon2->data->attack * 1.5 + pokemon2->data->hp * 1.2;
    // Print the scores
    printf("Pokemon 1: %s (Score = %.2f)\n", pokemon1->data->name, score1);
    printf("Pokemon 2: %s (Score = %.2f)\n", pokemon2->data->name, score2);
    // Check which pokemon won
    if (score1 > score2) {
        printf("%s wins!\n", pokemon1->data->name);
    } else if (score2 > score1) {
        printf("%s wins!\n", pokemon2->data->name);
        // If it's tie, print it
    } else {
        printf("It's a tie!\n");
    }
}

// Evolve pokemon
void evolvePokemon(OwnerNode* owner) {
    // Check if valid
    if (owner == NULL || owner->pokedexRoot == NULL) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    // Get the pokemon's ID from the user
    int id = readIntSafe("Enter ID of Pokemon to evolve: ");
    // Search the chosen pokemon in the tree
    PokemonNode* pokemon = searchPokemonBFS(owner->pokedexRoot, id);
    // Check if the pokemon found
    if (pokemon == NULL) {
        printf("Pokemon with ID %d not found.\n", id);
        return;
    }
    // Check if the pokemon can evolve
    if (pokemon->data->CAN_EVOLVE != CAN_EVOLVE) {
        printf("Pokemon %s cannot evolve.\n", pokemon->data->name);
        return;
    }
    // Save the original name before the evolve
    char* originalName = myStrdup(pokemon->data->name);
    // Check if allocation failed
    if (originalName == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    // Check if the evolve version of the pokemon already exist
    PokemonNode* evolvedForm = searchPokemonBFS(owner->pokedexRoot, id + 1);
    // Check that there is an evolve version of the pokemon in the pokemons array
    if (id < 1 || id >= MAX_POKEMON) {
        // If there isn't, print it
        printf("Cannot evolve this Pokemon.\n");
        // Free the memory of the not evolve version of the pokemon
        free(originalName);
        return;
    }
    // Initial the data of the pokemon
    const PokemonData* evolvedData = NULL;
    // Look for evolve version of the pokemon
    for (int i = 0; i < MAX_POKEMON; i++) {
        if (pokedex[i].id == id + 1) {
            evolvedData = &pokedex[i];
            break;
        }
    }
    // Check if there is an evolve version of the pokemon
    if (evolvedData == NULL) {
        printf("No evolution found for Pokemon with ID %d.\n", id);
        // Free the memory of the not evolve version of the pokemon
        free(originalName);
        return;
    }
    // Remove the original pokemon removed
    printf("Removing Pokemon %s (ID %d).\n", originalName, id);
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, id);
    // Print the evolve
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",
           originalName, id, evolvedData->name, id + 1);
    // If the evolve version already exist, no need to add
    if (evolvedForm == NULL) {
        // Create new node with the evolve version
        PokemonNode* newNode = createPokemonNode(evolvedData);
        if (newNode != NULL) {
            PokemonNode* insertedNode = insertPokemonNode(owner->pokedexRoot, newNode);
            // Check if the adding failed
            if (insertedNode != owner->pokedexRoot) {
                // Adding succeed
                owner->pokedexRoot = insertedNode;
            } else {
                // Adding failed
                printf("Failed to insert evolved Pokemon.\n");
                // Free the node that just created
                freePokemonNode(newNode);
            }
        }
        // If the creating failed
        else {
            printf("Failed to create evolved Pokemon node.\n");
        }
    }
    // Free the memory of the original pokemon
    free(originalName);
}

// Display the options for exist chosen pokemon
void enterExistingPokedexMenu() {
    // Check if there is any owner in the list
    if (ownerHead == NULL) {
        return;
    }
    printf("\nExisting Pokedexes:\n");
    // Initialize owners count to start from the first owner
    int count = 1;
    // Start the list from the head
    OwnerNode* currentOwner = ownerHead;
    // Print the exist owners and update the count
    do {
        printf("%d. %s\n", count, currentOwner->ownerName);
        count++;
        currentOwner = currentOwner->next;
        //Stop to print when we arrived again to the start of the list
    } while (currentOwner != ownerHead);
    // Get the chosen owner from the user and print it
    int choice = readIntSafe("Choose a Pokedex by number: ");
    // Casting the number of the owner to the name of the owner
    OwnerNode* chosenOwner = findOwnerByNumber(choice);
    // Check if the chosen owner point to null
    if (chosenOwner == NULL) {
        return;
    }
    printf("\nEntering %s's Pokedex...\n", chosenOwner->ownerName);
    int subChoice;
    // Print the sub menu
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", chosenOwner->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");
        // Get the choice from the user and print it
        subChoice = readIntSafe("Your choice: ");
        // Handle the user's choice
        switch (subChoice)
        {
            case 1:
                addPokemon(chosenOwner);
            break;
            case 2:
                displayMenu(chosenOwner);
            break;
            case 3:
                freePokemon(chosenOwner);
            break;
            case 4:
                pokemonFight(chosenOwner);
            break;
            case 5:
                evolvePokemon(chosenOwner);
            break;
            case 6:
                printf("Back to Main Menu.\n");
            break;
            default:
                printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// Free pokemons tree
void freePokemonTree(PokemonNode* root) {
    // Case base: if the tree is empty
    if (root == NULL) {
        return;
    }
    // Free recursively the sub-trees, left and right
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    // Free the current node by the free function
    freePokemonNode(root);
}

// Remove owner from the owners list
void removeOwnerFromCircularList(OwnerNode* target) {
    // Check if valid
    if (target == NULL || ownerHead == NULL) {
        return;
    }
    // Check the owner exist in the list
    OwnerNode* current = ownerHead;
    do {
        // The node found
        if (current == target) {
            break;
        }
        // Move to the next node
        current = current->next;
        // Stop when we arrived again to the head
    } while (current != ownerHead);
    // If we search all the list tnd the node didn't found
    if (current != target) {
        return;
    }
    // Case 1: there is only single in the list
    if (target->next == target) {
        ownerHead = NULL;
        return;
    }
    // Case 2: remove the head
    if (target == ownerHead) {
        ownerHead = ownerHead->next;
    }
    // Update the next and prev pointers
    target->prev->next = target->next;
    target->next->prev = target->prev;
    // Initial the pointers of the removed node
    target->next = NULL;
    target->prev = NULL;
}

// Free owner node
void freeOwnerNode(OwnerNode* owner) {
    // Check if valid
    if (owner == NULL) {
        return;
    }
    // Free the owner's name
    if (owner->ownerName != NULL) {
        free(owner->ownerName);
    }
    // Free the tree
    if (owner->pokedexRoot != NULL) {
        freePokemonTree(owner->pokedexRoot);
    }
    // Free the node itself
    free(owner);
}

// Delete pokedex
void deletePokedex() {
    // Check that there is pokedex
    if (ownerHead == NULL) {
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    // Count the number of owners
    int totalOwners = 0;
    OwnerNode* current = ownerHead;
    do {
        totalOwners++;
        current = current->next;
    } while (current != ownerHead);
    // Display the owners list
    current = ownerHead;
    int count = 1;
    do {
        printf("%d. %s\n", count++, current->ownerName);
        current = current->next;
    } while (current != ownerHead);
    // Get the chosen pokedex to delete from the user
    int choice = readIntSafe("Choose a Pokedex to delete by number: ");
    // Check if the choice is valid
    if (choice < 1 || choice > totalOwners) {
        printf("Invalid Pokedex selection.\n");
        return;
    }
    // Search the chosen owner
    OwnerNode* ownerToDelete = findOwnerByNumber(choice);
    // Check if valid
    if (ownerToDelete == NULL) {
        return;
    }
    printf("Deleting %s's entire Pokedex...\n", ownerToDelete->ownerName);
    // Delete the pokedex tree of the owner
    freePokemonTree(ownerToDelete->pokedexRoot);
    ownerToDelete->pokedexRoot = NULL;
    // Delete the owner from the list
    removeOwnerFromCircularList(ownerToDelete);
    // Free the memory of the owner himself
    freeOwnerNode(ownerToDelete);
    printf("Pokedex deleted.\n");
}

// Move all the pokemons from the first owner to the second owner
void mergeTrees(OwnerNode* firstOwner, OwnerNode* secondOwner) {
    // Check if there is pokemons in the second tree
    if (secondOwner->pokedexRoot == NULL) {
        return;
    }
    // Create an queue for BFS
    PokemonNode** queue = malloc(MAX_POKEMON * sizeof(PokemonNode*));
    if (queue == NULL) {
        return;
    }
    // Initialize the variable of the queue
    int indexToRemove = 0;
    int indexToInsert = 0;
    int failedInsertions = 0;
    // Insert the root of the second tree to the queue
    queue[indexToInsert ++] = secondOwner->pokedexRoot;
    // Check all the nodes in the second tree
    while (indexToRemove  < indexToInsert ) {
        PokemonNode* current = queue[indexToRemove ++];
        // Create copy of the current pokemon
        PokemonNode* newNode = createPokemonNode(current->data);
        // Insert to the first pokemon
        if (newNode != NULL) {
            PokemonNode* insertResult = insertPokemonNode(firstOwner->pokedexRoot, newNode);
            // Check if the insert failed
            if (insertResult == firstOwner->pokedexRoot) {
                failedInsertions++;
            }
            else {
                firstOwner->pokedexRoot = insertResult;
            }
        }
        else {
            failedInsertions++;
        }
        // Insert the children to the queue
        if (current->left && indexToInsert < MAX_POKEMON) {
            queue[indexToInsert++] = current->left;
        }
        if (current->right && indexToInsert < MAX_POKEMON) {
            queue[indexToInsert++] = current->right;
        }
    }
    // free the queue from the memory
    free(queue);
}

// Function that count the number of the pokemons in the tree
int countPokemonInTree(PokemonNode* root) {
    // Check if the tree exist
    if (root == NULL) {
        return 0;
    }
    // Count the number of the pokemons in the tree by order
    return 1 + countPokemonInTree(root->left) + countPokemonInTree(root->right);
}

// Merge pokemons
void mergePokedexMenu() {
    // Check that there is enough owners to merge
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    // Get the first name to the merging
    printf("Enter name of first owner: ");
    char* firstName = getDynamicInput();
    // Check if valid
    if (firstName == NULL || strlen(firstName) == 0) {
        // Free the memory
        free(firstName);
        return;
    }
    // Get the second name to the merging
    printf("Enter name of second owner: ");
    char* secondName = getDynamicInput();
    // Check if valid
    if (secondName == NULL || strlen(secondName) == 0) {
        // Free the memory
        free(firstName);
        free(secondName);
        return;
    }
    // Search the owners by name
    OwnerNode* firstOwner = findOwnerByName(firstName);
    OwnerNode* secondOwner = findOwnerByName(secondName);
    // Free the memory of the names
    free(firstName);
    free(secondName);
    // Check if the owners found
    if (firstOwner == NULL || secondOwner == NULL) {
        printf("One or both owners not found.\n");
        return;
    }
    // Check that the owners for the merging are different
    if (firstOwner == secondOwner) {
        return;
    }
    printf("Merging %s and %s...\n", firstOwner->ownerName, secondOwner->ownerName);
    // The number of the pokemons before the merging
    int initialFirstCount = countPokemonInTree(firstOwner->pokedexRoot);
    // Move all the pokemons from the second owner to the first owner
    mergeTrees(firstOwner, secondOwner);
    printf("Merge completed.\n");
    // Check if the merge succeed
    int finalFirstCount = countPokemonInTree(firstOwner->pokedexRoot);
    if (finalFirstCount <= initialFirstCount) {
        printf("Merge failed. No Pokemon were transferred.\n");
        return;
    }
    // Delete the second owner
    printf("Owner '%s' has been removed after merging.\n", secondOwner->ownerName);
    removeOwnerFromCircularList(secondOwner);
    freeOwnerNode(secondOwner);
}

// Swap the data of pokemon pair
void swapOwnerData(OwnerNode *a, OwnerNode *b){
    // Swap names with temp variable
    char* tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;
    // Swap pokemons tree with temp variable
    PokemonNode* tempTree = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempTree;
}

// Sort owners by names
void sortOwners() {
    // Check that there is enough owners to sort
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        return;
    }
    int swapped;
    OwnerNode *current;
    // Start sorting from the head
    do {
        swapped = 0;
        current = ownerHead;
        // Check any close pair in the list
        do {
            // If the order wrong, swap
            if (strcmp(current->ownerName, current->next->ownerName) > 0) {
                swapOwnerData(current, current->next);
                swapped = 1;
            }
            // Move to th next pair
            current = current->next;
            // Stop when we arrived again to the head
        } while (current->next != ownerHead);
    // Stop when sorted
    } while (swapped);
    printf("Owners sorted by name.\n");
}

// Print the owner by chosen direction
void printOwnersCircular() {
    // Check that there is owner to print
    if (ownerHead == NULL) {
        printf("No owners.\n");
        return;
    }
    // Get the direction of printing from the user
    printf("Enter direction (F or B): ");
    char* direction = getDynamicInput();
    // Check if allocation succeed
    if (direction == NULL) {
        return;
    }
    // Check the length of the string
    if (strlen(direction) != 1) {
        printf("Direction must be a single character (F or B).\n");
        // Free the memory
        free(direction);
        return;
    }
    // Check if the direction is valid
    if (direction == NULL || (direction[0] != 'F' && direction[0] != 'f' &&
        direction[0] != 'B' && direction[0] != 'b')) {
        printf("Invalid direction.\n");
        free(direction);
        return;
        }
    // Get the number of printing
    int numPrints = readIntSafe("How many prints? ");
    // Check if the input is valid
    if (numPrints <= 0) {
        printf("Number of prints must be positive.\n");
        // Free the memory
        free(direction);
        return;
    }
    // Search all over the list
    OwnerNode* current = ownerHead;
    for (int i = 1; i <= numPrints; i++) {
        printf("[%d] %s\n", i, current->ownerName);
        // Check if the search is by the right direction
        if (direction[0] == 'F' || direction[0] == 'f') {
            current = current->next;
        } else if (direction[0] == 'B' || direction[0] == 'b') {
            current = current->prev;
        }
    }
    // Free the memory
    free(direction);
}

// Free any owner from the memory
void freeAllOwners() {
    // Check if there is any owner in the list
    if (ownerHead == NULL) {
        return;
    }
    // If there is only single owner in the list
    if (ownerHead->next == ownerHead) {
        // Free his tree
        freePokemonTree(ownerHead->pokedexRoot);
        // Free the owner himself
        freeOwnerNode(ownerHead);
        ownerHead = NULL;
        return;
    }
    // If there is more than one owner in the list
    OwnerNode* current = ownerHead;
    OwnerNode* next;
    // Check the entire list
    do {
        // Save the next in the list in temp
        next = current->next;
        // Free the current
        freeOwnerNode(current);
        // Move to the next
        current = next;
        // Stop when we arrived again to the head
    } while (current != ownerHead);
    ownerHead = NULL;
}
