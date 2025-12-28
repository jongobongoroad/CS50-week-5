// Implements a dictionary's functionality

#include "dictionary.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const unsigned int N = 27; // 26 letters + apostrophe

unsigned int word_count = 0;
int c2i(char c);

// Represents a node in a hash table
typedef struct node
{
    char c;               // character stored in this node
    unsigned int hash;    // full-word hash (only valid at word end)
    struct node *next[N]; // children
} node;

void free_memory(node *n);

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    int wi = 0;                   // index
    int idx = c2i(word[wi]);      // character index [0-26]
    int idx1 = c2i(word[wi + 1]); // character+1 index [0-26]
    node *parent = table[idx];
    if (!parent)
        return false; // the word isn't in the dictionary
    while (word[wi] != '\0')
    {
        if (word[wi + 1] == '\0')                         // if it's the last character
            return parent->hash == hash(word);            // return true or false if hashes matches
        else if (!parent->next[idx1])                     // if the next character doesn't exist
            return false;                                 // then the word isn't in the dictionary
        else if (toupper(parent->c) == toupper(word[wi])) // if characters matches
            parent = parent->next[idx1];                  // then advance iteration
        wi++;                                             // increment iterator
        idx = c2i(word[wi]);                              // update character index [0-26]
        idx1 = c2i(word[wi + 1]);                         // update character+1 index [0-26]
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned int wi = 0, u_int = toupper(word[0]); // initial character ASCII
    if (!!word[1])                                 // if the word has a second character
    {
        u_int *= 100; // move first character to the left by 2 digits
        u_int += toupper(
            word[1]);    // set the new 2 digits on the right to the second character ASCII value
        u_int *= 100000; // make space for the checksum of the ASCII values of the word
    }
    else
        u_int *= 10000000;   // make space for the checksum of the ASCII value of the single letter
    while (word[wi] != '\0') // while not at the end of the word
    {
        u_int += toupper(word[wi]); // add ASCII value of the current character for checksum
        wi++;                       // increment iterator to reach end of word
    }
    return u_int; // return hashed word, for example input "cat" return "676500216" (67 = C, 65 = A,
                  // 00 = unused digits by checksum, 216 = checksum of c+a+t ASCII values)
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *source = fopen(dictionary, "r"); // load it up
    node *previous_node = NULL;            // initialize node
    if (source == NULL)                    // fail on NULL value
        return false;
    char ch;                       // current character being read
    char current_word[LENGTH + 1]; // current word
    int length = 0;                // words length iterator
    int idx = 0;                   // character's index
    for (int i = 0; i < N; i++)    // cleanup starting point
        table[i] = NULL;
    while ((ch = fgetc(source)) != EOF) // for each character
    {
        idx = c2i(ch); // stored the character's index
        if (ch != ' ' &&
            ch != '\n') // if not a separator but a valid character (ABCDEFGHIJKLMNOPQRSTUVWXYZ')
        {
            if (!length) // if length == 0 aka first letter of the word
            {
                if (!table[idx]) // check if character node is not existing already
                {
                    node *current_node = calloc(1, sizeof(node)); // initialize node
                    if (current_node == NULL)                     // fail on NULL value
                        return false;
                    current_node->c = ch;      // init new node property c
                    table[idx] = current_node; // if so populate table with node accordingly
                }
                previous_node = table[idx]; // init and configure node
            }
            else // else length != 0 aka the remaining next characters
            {
                if (previous_node->next[idx] == NULL) // if previous_node child = NULL
                {
                    previous_node->next[idx] = calloc(1, sizeof(node)); // initialize child node
                    if (!previous_node->next[idx])                      // fail on child invalid
                        return false;
                    previous_node->next[idx]->c = ch; // init new node property c
                }
                previous_node = previous_node->next[idx];
            }
            current_word[length] = ch; // update the current forming word
            length++;                  // increment iteration
        }
        else if (length > 0)
        {
            current_word[length] = '\0';              // close string current_word for next word
            word_count++;                             // increment word count
            previous_node->hash = hash(current_word); // set the word hash from the function
            length = 0;                               // reset length counter for next word
        }
    }
    fclose(source);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return word_count;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    for (int i = 0; i < N; i++) // iterate through initial table
    {
        free_memory(table[i]); // start freeing memory starting from table
        table[i] = NULL;       // ensure table is clean
    }
    return true; // succesfuly unloaded the dictionary from memory
}

int c2i(char c) // helper function returning the index of a character
{
    c = toupper(c);             // set to uppercase for case insensitive comparisons
    if (c >= 'A' && c <= 'Z')   // if a valid letter
        return c - 'A';         // return the index of that character [0-25]
    return c == '\'' ? 26 : -1; // otherwise if it's character \' return 26 or -1 if it's a
                                // character not allowed.
}

void free_memory(node *n) // frees the memory used by the nodes
{
    if (!n)                      // if the node is NULL, there is nothing to free (like last nodes)
        return;                  // if so returns, the task is completed
    for (int i = 0; i < N; i++)  // for each child node, starting from 0 and ending with 26
        free_memory(n->next[i]); // free the memory of the child based on current character/index
    free(n); // lastly free the memory of the parent node n after all its children have been freed
}
