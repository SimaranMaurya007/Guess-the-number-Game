#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const int LOWER_BOUND = 1;
const int UPPER_BOUND = 100;
const int NUM_GAMES = 10;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int ai_guess(int target) {
    int low = LOWER_BOUND, high = UPPER_BOUND;
    int guesses = 0;
    while (1) {
        int guess = (low + high) / 2;
        guesses++;
        if (guess == target) {
            return guesses;
        } else if (guess < target) {
            low = guess + 1;
        } else {
            high = guess - 1;
        }
    }
}

void *player_thread(void *arg) {
    int *data = (int *)arg;
    data[1] = ai_guess(data[0]);
    return NULL;
}

int main() {
    srand((unsigned int)time(NULL));
    FILE *logFile = fopen("log.txt", "w");
    
    if (!logFile) {
        perror("Failed to open log.txt for writing");
        return 1;
    }

    int total_guesses_player1 = 0;
    int total_guesses_player2 = 0;

    for (int i = 0; i < NUM_GAMES; i++) {
        int player1_number = rand() % (UPPER_BOUND - LOWER_BOUND + 1) + LOWER_BOUND;
        int player2_number = rand() % (UPPER_BOUND - LOWER_BOUND + 1) + LOWER_BOUND;

        int player1_data[2] = { player2_number, 0 };
        int player2_data[2] = { player1_number, 0 };

        pthread_t t1, t2;

        pthread_create(&t1, NULL, player_thread, &player1_data);
        pthread_join(t1, NULL);

        pthread_create(&t2, NULL, player_thread, &player2_data);
        pthread_join(t2, NULL);

        total_guesses_player1 += player1_data[1];
        total_guesses_player2 += player2_data[1];

        pthread_mutex_lock(&mtx);
        fprintf(logFile, "Game %d:\n", i + 1);
        fprintf(logFile, "Player 1 thought of %d and took %d guesses.\n", player1_number, player2_data[1]);
        fprintf(logFile, "Player 2 thought of %d and took %d guesses.\n", player2_number, player1_data[1]);
        fprintf(logFile, "--------------------------\n");
        pthread_mutex_unlock(&mtx);
    }

    double avg_guesses_player1 = (double)total_guesses_player1 / NUM_GAMES;
    double avg_guesses_player2 = (double)total_guesses_player2 / NUM_GAMES;

    fprintf(logFile, "Average guesses after %d games:\n", NUM_GAMES);
    fprintf(logFile, "Player 1: %lf\n", avg_guesses_player1);
    fprintf(logFile, "Player 2: %lf\n", avg_guesses_player2);

    if (avg_guesses_player1 < avg_guesses_player2) {
        fprintf(logFile, "Player 1 wins!\n");
    } else if (avg_guesses_player2 < avg_guesses_player1) {
        fprintf(logFile, "Player 2 wins!\n");
    } else {
        fprintf(logFile, "It's a tie!\n");
    }

    fclose(logFile);
    return 0;
}
