// Saikiran Annareddy
// Programming Assignment #4
// 03-20-23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 19

typedef struct customer {
    char name[MAXLEN + 1];  
    int points;             
} customer;


typedef struct treenode {
    customer* cPtr;         
    int size;               
    struct treenode* left;  
    struct treenode* right; 
} treenode;

// function to create a new customer with the given name and points
customer* createCustomer(const char* name, int points) {
    customer* cPtr = malloc(sizeof(customer)); 
    strcpy(cPtr->name, name);                   
    cPtr->points = points;                      
    return cPtr;                               
}

// function to create a new tree node with the given customer pointer
treenode* createNode(customer* cPtr) {
    treenode* new_node = malloc(sizeof(treenode)); 
    new_node->cPtr = cPtr;                        
    new_node->left = new_node->right = NULL;       
    new_node->size = 1;                            
    return new_node;                               
}

// function to find the node with maximum value in a subtree
treenode* findMax(treenode* node) {
    while (node->right != NULL)
        node = node->right;
    return node;
}

// function to update the size of a node based on its children's sizes
void updateSize(treenode* node) {
    if (node == NULL)
        return;
    node->size = 1;
    if (node->left)
        node->size += node->left->size;
    if (node->right)
        node->size += node->right->size;
}

// function to remove a customer pointer from the array of customers
void removeFromCustomersArray(customer* cPtr, customer** customers, int* customerCount) {
    int i;
    for (i = 0; i < *customerCount; i++) {
        if (customers[i] == cPtr) {
            customers[i]->points = 0; 
            break;
        }
    }
    if (i < *customerCount) {
        for (int j = i; j < *customerCount - 1; j++) {
            customers[j] = customers[j + 1];
        }
        (*customerCount)--;
    }
}

// function to insert a new customer into the binary search tree
treenode* insert(treenode* root, customer* cPtr, int* is_new_node) {
    if (root == NULL) {
        *is_new_node = 1;
        return createNode(cPtr);
    }
    root->size++; 
    int cmp = strcmp(cPtr->name, root->cPtr->name);
    if (cmp < 0) {
        root->left = insert(root->left, cPtr, is_new_node);
    } else if (cmp > 0) {
        root->right = insert(root->right, cPtr, is_new_node);
    } else { 
        root->cPtr->points += cPtr->points;
        *is_new_node = 0;
    }
    return root;
}

// function to find a customer node with the given name in the binary search tree
treenode* find(treenode* root, const char* name, int* depth) {
    *depth = 0;
    while (root != NULL) {
        int cmp = strcmp(name, root->cPtr->name);
        if (cmp < 0) {
            root = root->left;
        } else if (cmp > 0) {
            root = root->right;
        } else {
            return root;
        }
        (*depth)++;
    }
    return NULL;
}

// function to copy customer data from source to destination
void copyCustomerData(customer* dest, customer* src) {
    strcpy(dest->name, src->name);
    dest->points = src->points;
}

// function to delete a node with the given name from the binary search tree
treenode* deleteNode(treenode* root, const char* name, customer** customers, int* customerCount) {
    if (root == NULL)
        return NULL;

    int cmp = strcmp(name, root->cPtr->name);
    if (cmp < 0) {
        root->left = deleteNode(root->left, name, customers, customerCount);
    } else if (cmp > 0) {
        root->right = deleteNode(root->right, name, customers, customerCount);
    } else {
        if (root->left == NULL || root->right == NULL) {
            treenode* temp = root->left ? root->left : root->right;
            removeFromCustomersArray(root->cPtr, customers, customerCount);
            free(root->cPtr);
            free(root);
            return temp;
        } else {
            treenode* temp = findMax(root->left);
            for (int i = 0; i < *customerCount; i++) {
                if (strcmp(customers[i]->name, root->cPtr->name) == 0) {
                    customers[i]->points = temp->cPtr->points;
                    break;
                }
            }
            copyCustomerData(root->cPtr, temp->cPtr); 
            root->left = deleteNode(root->left, temp->cPtr->name, customers, customerCount);
        }
    }
    updateSize(root);
    return root;
}

//function to count # of customers with names smaller than the given name
int countSmaller(treenode* root, const char* name) {
    int count = 0;
    while (root != NULL) {
        int cmp = strcmp(name, root->cPtr->name);
        if (cmp > 0) {
            count += (root->left ? root->left->size : 0) + 1;
            root = root->right;
        } else {
            root = root->left;
        }
    }
    return count;
}

// comparison function for sorting customers by points
int compareCustomersByPoints(const void* a, const void* b) {
    customer* c1 = *(customer**)a;
    customer* c2 = *(customer**)b;
    if (c1->points != c2->points)
        return c2->points - c1->points; 
    return strcmp(c1->name, c2->name);    
}

// helper function for quicksort
int partition(customer** arr, int low, int high) {
    customer* pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (compareCustomersByPoints(&arr[j], &pivot) < 0) {
            i++;
            customer* temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    customer* temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return (i + 1);
}

// quicksort function to sort an array of customers by points
void quickSort(customer** arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// function to sort the array of customers using quicksort
void solve(customer** customers, int customerCount) {
    quickSort(customers, 0, customerCount - 1);
}

// function to free memory allocated for the binary search tree
void freeTree(treenode* root) {
    if (root == NULL)
        return;
    freeTree(root->left);
    freeTree(root->right);
    free(root->cPtr);
    free(root);
}

int main() {
    int n, is_new_node, depth, points;
    char command[10], name[MAXLEN + 1];
    treenode* root = NULL;
    treenode* node;
    customer* cPtr;
    fscanf(stdin, "%d", &n);

    customer** customers = malloc(n * sizeof(customer*));
    int customerCount = 0;

    for (int i = 0; i < n; ++i) {
        fscanf(stdin, "%s", command); 
        if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0) {
            fscanf(stdin, "%s %d", name, &points); // read customer name and points from input
            if (strcmp(command, "sub") == 0)
                points = -points;

            // find the node corresponding to the customer name in the binary search tree
            node = find(root, name, &depth);

            // if node exists, update the points or delete the customer if points drop below zero
            if (node) {
                node->cPtr->points += points;
                if (node->cPtr->points < 0) {
                    root = deleteNode(root, name, customers, &customerCount);
                    printf("%s deleted\n", name);
                } else {
                    printf("%s %d\n", node->cPtr->name, node->cPtr->points);
                }
            } else {
                // if node doesn't exist, create a new customer if points are positive
                if (points > 0) {
                    cPtr = createCustomer(name, points);
                    root = insert(root, cPtr, &is_new_node);
                    if (is_new_node)
                        customers[customerCount++] = cPtr;
                    printf("%s %d\n", cPtr->name, cPtr->points);
                } else {
                    printf("%s not found\n", name);
                }
            }
        } else if (strcmp(command, "del") == 0) {
            fscanf(stdin, "%s", name); 
            node = find(root, name, &depth);
            if (node) {
                root = deleteNode(root, name, customers, &customerCount);
                printf("%s deleted\n", name);
            } else {
                printf("%s not found\n", name);
            }
        } else if (strcmp(command, "search") == 0) {
            fscanf(stdin, "%s", name); 
            node = find(root, name, &depth);
            if (node) {
                printf("%s %d %d\n", name, node->cPtr->points, depth);
            } else {
                printf("%s not found\n", name);
            }
        } else if (strcmp(command, "count_smaller") == 0) {
            fscanf(stdin, "%s", name); 
            printf("%d\n", countSmaller(root, name));
        }
    }

    solve(customers, customerCount);

    for (int i = 0; i < customerCount; ++i) {
        if (customers[i]->points > 0) {
            printf("%s %d\n", customers[i]->name, customers[i]->points);
        }
    }

    freeTree(root);
    free(customers);

    return 0;
}

