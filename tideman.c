#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
bool cycle_verify(int pair_1, int pair_2);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Loopping over all the candidates and finding a matching name
    for (int i = 0; i < candidate_count; i++)
    {
        // If the name passed by the voter matches with one candidate, update the ranks array with the candidate's position at the position in the rank variable which represent his preference
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Variables to hold both candidates positions and values in the ranks array
    int candidate_row_position = 0, candidate_col_position = 0, candidate_value_row = 0, candidate_value_col = 0;
    // Variable to verify if a candidate col value was got
    bool candidate_col_verification = false;
    while (true)
    {
        // Loop over all the candidates to get the position of the current candidate
        for (int i = 0; i < candidate_count; i++)
        {
            if (ranks[i] == candidate_value_row)
            {
                candidate_row_position = i;
                break;
            }
        }
        // Loop over all the candidates, the number of the candidates times
        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                // If the value which represent the current ranks j candidate is equals to the candidate value that is being compared, get it's position in the array
                if (ranks[j] == candidate_value_col)
                {
                    candidate_col_position = j;
                    candidate_col_verification = true;
                }
                // If the candidate row position is smaller than the candidate column position, if it come first, add one to the preference array
                if (candidate_row_position < candidate_col_position)
                {
                    preferences[candidate_value_row][candidate_value_col] += 1;
                }
                // If the current candidate col value was already taken, break the loop
                if (candidate_col_verification)
                {
                    break;
                }
            }
            // Restart the position value for the next candidate
            candidate_col_position = 0;
            // Add one to the value to go to the next candidate
            candidate_value_col += 1;
            // Set bool variable verification to flase again
            candidate_col_verification = false;
        }
        // Restart variable to pass over each candidate column again
        candidate_value_col = 0;
        // Go to the next row candidate
        candidate_value_row += 1;
        // If the current value is the last one and it gets in here, break the loop
        if (candidate_value_row == (candidate_count))
        {
            break;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // Variable to hold the positions of each possible pair
    int pairs_position = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        // Looping over the candidates only after the candidate which is being compared
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pairs_position].winner = i;
                pairs[pairs_position].loser = j;
                pair_count += 1;
                pairs_position += 1;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pairs_position].winner = j;
                pairs[pairs_position].loser = i;
                pair_count += 1;
                pairs_position += 1;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Array to hold the difference between the winner and loser of each pair
    int strength_difference[pair_count];
    // Declaring variable to hold the stronger pair at the moment, the position of this pair in the pairs array and a counter to make the verifing loop begin after the pairs which had been selected already
    int stronger_pair = 0, position_stronger, c = 0;
    bool verification_stronger_change = true;
    // Loop over the pairs array and get the difference between each pair candidate as an int and put it in the strength difference array in the same order as pairs array
    for (int i = 0; i < pair_count; i++)
    {
        // Get the preference value of the winner position by his position and the loser position at the preferences array and the same with the loser and subtract them
        strength_difference[i] = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
    }
    while (true)
    {
        // Get the bigger number at the strength_difference array and its position
        for (int i = c; i < pair_count; i++)
        {
            if (strength_difference[i] > stronger_pair)
            {
                stronger_pair = strength_difference[i];
                position_stronger = i;
            }
        }
        // Get the current first position of the pairs array, which is not sorted, and put it in a temporary variable
        pair temp = pairs[c];
        // Put the stronger pair, which has the bigger difference in the strength_diference array, which has the same order as pairs array, in the first position
        pairs[c] = pairs[position_stronger];
        // And put the old first position pair at the old position of the stronger pair
        pairs[position_stronger] = temp;
        // The same process happen with strength_difference array, because it has the same order of pairs array
        int temp2 = strength_difference[c];
        strength_difference[c] = strength_difference[position_stronger];
        strength_difference[position_stronger] = temp2;
        // Add one to c, which is the counter of positions, moving foward and not analyzing the positions which have been already analized
        c += 1;
        // Set stronger pair to 0 to find the next stronger pair
        stronger_pair = 0;
        // If c is equals the number of pairs, break the loop
        if (c == pair_count)
        {
            break;
        }
    }
    return;
}
// Verify if there's a cycle
bool cycle_verify(int pair_1, int pair_2)
{
    // If the initial winner pair is equals to some loser pair, return true because it is a cycle
    if (pair_1 == pair_2)
    {
        return true;
    }
    // Loop through each candidate
    for (int i = 0; i < pair_count; i++)
    {
        // If the actual loser wins over someone, verify the same thing with this new loser
        if (locked[pair_2][i])
        {
            // Call the function inside the if conditional to return always true if there's a cycle
            if (cycle_verify(pair_1, i))
            {
                return true;
            }
        }
    }
    // If there isn't any cycle, return false
    return false;
}
// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    bool verification = true;
    // Loop through each candidate and call the cycle_verify function
    for (int i = 0; i < pair_count; i++)
    {
        verification = cycle_verify(pairs[i].winner, pairs[i].loser);
        // If the return value is false, add the pair
        if (!verification)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // Loop each candidate through each candidate to verify if the actual candidate loses to somebody
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count; j++)
        {
            if (locked[j][i])
            {
                break;
            }
            // If the loop reach this point, print the candidate because he hasn't losed to anybody
            if (j == pair_count - 1)
            {
                printf("%s\n", candidates[i]);
            }
        }
    }
    return;
}
