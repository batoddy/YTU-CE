#include <stdio.h>
#include <stdlib.h>

// Node structure for a linked list
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Function to insert a node at the beginning of the linked list
Node *insertAtBeginning(Node *head, int data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    newNode->data = data;
    newNode->next = head;

    return newNode;
}

// Function to print the linked list
void printList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

// Function to reverse the linked list (iterative approach)
Node *reverseList(Node *head) {
    Node *prev = NULL;
    Node *current = head;
    Node *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev; // 'prev' becomes the new head of the reversed list
}

int main() {
    // Create a linked list with a few nodes
    Node *head = NULL;
    head = insertAtBeginning(head, 3);
    head = insertAtBeginning(head, 7);
    head = insertAtBeginning(head, 10);
    head = insertAtBeginning(head, 5);

    // Print the original linked list
    printf("Original Linked List: ");
    printList(head);

    // Reverse the linked list
    head = reverseList(head);

    // Print the reversed linked list
    printf("Reversed Linked List: ");
    printList(head);

    // Free memory allocated for the linked list
    Node *current = head;
    Node *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    return 0;
}
