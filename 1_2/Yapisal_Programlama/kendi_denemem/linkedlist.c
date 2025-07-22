#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char name[20];
    int val;
    struct Node *next;
} Node;

void add_list_end(Node *, Node);
void print_ll(Node *head);
void delete_node(Node *head, Node input);

void main()
{
    Node *head, input;
    head = (Node *)calloc(2, sizeof(Node));

    printf("Name | id:");
    scanf("%s", input.name);
    scanf("%d", &input.val);
    add_list_end(head, input);

    printf("Name | id:");
    scanf("%s", input.name);
    scanf("%d", &input.val);

    printf("Name | id:");
    scanf("%s", input.name);
    scanf("%d", &input.val);

    printf("Name | id:");
    scanf("%s", input.name);
    scanf("%d", &input.val);

    add_list_end(head, input);
    print_ll(head);
    printf("Delete:");
    scanf("%s", input.name);

    delete_node(head, input);
    print_ll(head);

    return;
}

void add_list_end(Node *head, Node new_node)
{
    Node *current;
    current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = (Node *)malloc(sizeof(Node));
    strcpy(current->next->name, new_node.name);
    current->next->val = new_node.val;
    current->next->next = NULL;
}

void print_ll(Node *head)
{
    Node *current;
    current = head->next;
    while (current->next != NULL)
    {
        printf("%s:%d-", current->name, current->val);
        current = current->next;
    }
    printf("%s:%d\n", current->name, current->val);
}

void delete_node(Node *head, Node input)
{
    Node *prev = NULL;
    Node *current = head->next;
    while (current->next != NULL && strcmp(current->name, input.name))
    {
        prev = current;
        current = current->next;
    }
    if (current->next == NULL)
        printf("silinmek istenen eleman listede yok\n");
    else
    {
        prev->next = current->next;
        free(current);
    }
}