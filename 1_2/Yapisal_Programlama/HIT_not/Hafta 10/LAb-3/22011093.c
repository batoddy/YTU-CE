#include <stdio.h>
#include <stdlib.h>
typedef struct Node
{
    int v1;
    int v2;
    struct Node *next;
} Node;
void printList(Node *n);
void insert(struct Node *head_ref, int v1, int v2);
Node *intersection(Node *start1, Node *start2, Node (*reduce_f)(Node *, Node *));
int compare_by_v1(Node *Node1, Node *Node2);
int compare_by_v2(Node *Node1, Node *Node2);
Node *reduce_f(Node *, Node *);

int main()
{
    Node *head1, *head2, *reduced_head;
    int res_v1, res_v2;

    head1 = calloc(1, sizeof(Node));
    head2 = calloc(1, sizeof(Node));
    head1->next = NULL;
    head2->next = NULL;
    insert(head1, 1, 10);
    insert(head1, 2, 20);
    insert(head1, 3, 5);
    insert(head2, 2, 5);
    insert(head2, 1, 25);
    insert(head2, 4, 20);

    printList(head1);
    printList(head2);

    res_v1 = compare_by_v1(head1, head2);
    res_v2 = compare_by_v2(head1, head2);
    printf("\nres_v1:%d", res_v1);
    printf("\nres_v2:%d", res_v2);

    reduced_head = intersection(head1, head2, reduce_f);
    printList(reduced_head);
}

void printList(Node *n)
{
    Node *current = n;
    printf("\nliste elemanlari:\n");
    while (current->next != NULL)
    {
        printf("---v1:%d & v2:%d---", current->v1, current->v2);
        current = current->next;
    }
    printf("v1:%d & v2:%d", current->v1, current->v2);
}

void insert(struct Node *head_ref, int v1, int v2)
{
    Node *current = head_ref;
    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = malloc(sizeof(Node));
    current->next->v1 = v1;
    current->next->v2 = v2;
    current->next->next = NULL;
}

Node *intersection(Node *start1, Node *start2, Node (*reduce_f)(Node *, Node *))
{
    Node *current1 = start1;
    Node *current2 = start2;
    Node *reduce_head;
    Node *reduce_node;
    reduce_head = calloc(1, sizeof(Node));
    reduce_head->next = NULL;

    while (current1->next != NULL)
    {
        current1 = current1->next;
        while (current2->next != NULL)
        {
            current2 = current2->next;
            *reduce_node = reduce_f(current1->next, current2->next);
            insert(reduce_head, reduce_node->v1, reduce_node->v2);
        }
        current2 = start2;
    }
    return reduce_head;
}

int compare_by_v1(Node *Node1, Node *Node2)
{
    Node *current1 = Node1;
    Node *current2 = Node2;
    int flag = 0;

    while (current1->next != NULL && flag == 0)
    {
        while (current2->next != NULL && flag == 0)
        {
            if (current1->next->v1 == current2->next->v1)
                flag = 1;
            current2 = current2->next;
        }
        current1 = current1->next;
        current2 = Node2;
    }
    /*
        while (current1->next != NULL && current2->next != NULL && flag == 0)
        {
            if (current1->next->v1 == current2->next->v1)
                flag = 1;
            current1 = current1->next;
            current2 = current2->next;
        }*/
    return flag;
}

int compare_by_v2(Node *Node1, Node *Node2)
{
    Node *current1 = Node1;
    Node *current2 = Node2;
    int flag = 0;

    while (current2->next != NULL && flag == 0)
    {
        while (current1->next != NULL && flag == 0)
        {
            if (current2->next->v1 == current1->next->v1)
                flag = 1;
            current1 = current1->next;
        }
        current2 = current2->next;
        current1 = Node1;
    }
    /*
    while (current1->next != NULL && current2->next != NULL && flag == 0)
    {
        if (current1->next->v2 == current2->next->v2)
            flag = 1;
        current1 = current1->next;
        current2 = current2->next;
    }*/
    return flag;
}

Node *reduce_f(Node *node1, Node *node2)
{
    Node *reduced_node;
    if (node1->v1 == node2->v2)
    {
        reduced_node->v1 = node1->v1;
        reduced_node->v2 = node1->v2;
    }
}