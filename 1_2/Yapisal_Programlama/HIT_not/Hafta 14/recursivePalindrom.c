#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to check if a string is a palindrome recursively
int isPalindrome(char *str, int start, int end) {
    // Base case: if the string has one or zero characters, it's a palindrome
    if (start >= end) {
        return 1;
    }

    // Check if the characters at the start and end positions are equal
    if (str[start] == str[end]) {
        // Recursive call for the remaining substring
        return isPalindrome(str, start + 1, end - 1);
    } else {
        // If characters are not equal, the string is not a palindrome
        return 0;
    }
}

int main() {
    char inputString[100];

    // Get input from the user
	scanf("%s",inputString);
    // Calculate the length of the string
    int length = strlen(inputString);

    // Check if the input string is a palindrome using recursion
    if (isPalindrome(inputString, 0, length - 1)) {
        printf("The entered string is a palindrome.\n");
    } else {
        printf("The entered string is not a palindrome.\n");
    }

    return 0;
}
