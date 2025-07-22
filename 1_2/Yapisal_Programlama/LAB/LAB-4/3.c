#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Uçuş bilgileri için struct tanimi
typedef struct Flight
{
    int flightID;
    char departureCity[20];
    char arrivalCity[20];
    char departureTime[10];
    int dayType;
    int runway;
    struct Flight *next;
} Flight;

Flight *createFlight(int flightID, char *departureCity, char *arrivalCity, char *departureTime, int dayType, int runway)
{
    Flight *newFlight = (Flight *)malloc(sizeof(Flight));
    newFlight->flightID = flightID;
    strcpy(newFlight->departureCity, departureCity);
    strcpy(newFlight->arrivalCity, arrivalCity);
    strcpy(newFlight->departureTime, departureTime);
    newFlight->dayType = dayType;
    newFlight->runway = runway;
    newFlight->next = NULL;
    return newFlight;
}

void insertFlight(Flight **head, Flight *newFlight)
{
    Flight *current;
    if (*head == NULL || (*head)->flightID > newFlight->flightID)
    {
        newFlight->next = *head;
        *head = newFlight;
    }
    else
    {
        current = *head;
        while (current->next != NULL && current->next->flightID < newFlight->flightID)
        {
            current = current->next;
        }
        newFlight->next = current->next;
        current->next = newFlight;
    }
}

void printFlights(Flight *head)
{
    Flight *temp = head;
    while (temp != NULL)
    {
        printf("%d %s %s %s %d %d - \n", temp->flightID, temp->departureCity, temp->arrivalCity, temp->departureTime, temp->dayType, temp->runway);
        temp = temp->next;
    }
}

void saveFlightsToFile(Flight *head, char *filename)
{
    FILE *file = fopen(filename, "w");
    Flight *temp = head;
    while (temp != NULL)
    {
        fprintf(file, "%d %s %s %s %d %d\n", temp->flightID, temp->departureCity, temp->arrivalCity, temp->departureTime, temp->dayType, temp->runway);
        temp = temp->next;
    }
    fclose(file);
}

int main()
{
    Flight *head = NULL;
    FILE *file = fopen("ucusBilgi.txt", "r");
    if (file == NULL)
    {
        printf("Dosya açilamadi\n");
        return 1;
    }

    int flightID, dayType, runway;
    char departureCity[20], arrivalCity[20], departureTime[10];
    while (fscanf(file, "%d %s %s %s %d %d", &flightID, departureCity, arrivalCity, departureTime, &dayType, &runway) != EOF)
    {
        Flight *newFlight = createFlight(flightID, departureCity, arrivalCity, departureTime, dayType, runway);
        insertFlight(&head, newFlight);
    }
    fclose(file);

    printf("Ucuslar:\n");
    printFlights(head);

    // Pistlere göre dosyalara kaydetme
    for (int i = 1; i <= 10; i++)
    {
        char filename[20];
        sprintf(filename, "pist%d.txt", i);
        Flight *runwayFlights = NULL;
        Flight *temp = head;

        while (temp != NULL)
        {
            if (temp->runway == i)
            {
                insertFlight(&runwayFlights, createFlight(temp->flightID, temp->departureCity, temp->arrivalCity, temp->departureTime, temp->dayType, temp->runway));
            }
            temp = temp->next;
        }

        if (runwayFlights != NULL)
        {
            saveFlightsToFile(runwayFlights, filename);
        }
    }

    return 0;
}
