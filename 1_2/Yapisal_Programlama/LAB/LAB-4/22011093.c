#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct FlightNode
{
    int id;
    char depCity[20];
    char arrCity[20];
    char depTime[10];
    int typeOfDay;
    int runwayNumber;
    struct FlightNode *nextFlight;
} FlightNode;

FlightNode *createFlightNode(int id, char *depCity, char *arrCity, char *depTime, int typeOfDay, int runwayNumber)
{
    FlightNode *newNode = (FlightNode *)malloc(sizeof(FlightNode));
    newNode->id = id;
    strcpy(newNode->depCity, depCity);
    strcpy(newNode->arrCity, arrCity);
    strcpy(newNode->depTime, depTime);
    newNode->typeOfDay = typeOfDay;
    newNode->runwayNumber = runwayNumber;
    newNode->nextFlight = NULL;
    return newNode;
}

void addFlight(FlightNode **head, FlightNode *newNode)
{
    FlightNode *current;
    if (*head == NULL || (*head)->id > newNode->id)
    {
        newNode->nextFlight = *head;
        *head = newNode;
    }
    else
    {
        current = *head;
        while (current->nextFlight != NULL && current->nextFlight->id < newNode->id)
        {
            current = current->nextFlight;
        }
        newNode->nextFlight = current->nextFlight;
        current->nextFlight = newNode;
    }
}

void displayFlights(FlightNode *head)
{
    FlightNode *temp = head;
    int index = 0;
    while (temp != NULL)
    {
        printf("%d- %d %s %s %s %d %d\n", index, temp->id, temp->depCity, temp->arrCity, temp->depTime, temp->typeOfDay, temp->runwayNumber);
        temp = temp->nextFlight;
        index++;
    }
}

void writeFlightsToFile(FlightNode *head, char *fileName)
{
    FILE *filePtr = fopen(fileName, "w");
    FlightNode *temp = head;
    while (temp != NULL)
    {
        fprintf(filePtr, "%d %s %s %s %d %d\n", temp->id, temp->depCity, temp->arrCity, temp->depTime, temp->typeOfDay, temp->runwayNumber);
        temp = temp->nextFlight;
    }
    fclose(filePtr);
}

int main()
{
    FlightNode *head = NULL;
    FILE *inputFile = fopen("ucusBilgi.txt", "r");
    if (inputFile == NULL)
    {
        printf("File can't open\n");
        return 1;
    }

    int id, typeOfDay, runwayNumber;
    char depCity[20], arrCity[20], depTime[10];
    while (fscanf(inputFile, "%d %s %s %s %d %d", &id, depCity, arrCity, depTime, &typeOfDay, &runwayNumber) != EOF)
    {
        FlightNode *newNode = createFlightNode(id, depCity, arrCity, depTime, typeOfDay, runwayNumber);
        addFlight(&head, newNode);
    }
    fclose(inputFile);

    printf("Flight List:\n");
    displayFlights(head);

    for (int i = 1; i <= 10; i++)
    {
        char fileName[20];
        sprintf(fileName, "pist%d.txt", i);
        FlightNode *runwayFlights = NULL;
        FlightNode *temp = head;

        while (temp != NULL)
        {
            if (temp->runwayNumber == i)
            {
                FlightNode *newNode = createFlightNode(temp->id, temp->depCity, temp->arrCity, temp->depTime, temp->typeOfDay, temp->runwayNumber);
                addFlight(&runwayFlights, newNode);
            }
            temp = temp->nextFlight;
        }

        if (runwayFlights != NULL)
        {
            writeFlightsToFile(runwayFlights, fileName);
        }
    }

    return 0;
}
