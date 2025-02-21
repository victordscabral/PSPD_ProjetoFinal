#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>

#define MAX_WORD_LENGTH 100

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

void add_word(WordCount *words, int *size, char *word) {
    for (int i = 0; i < *size; i++) {
        if (strcmp(words[i].word, word) == 0) {
            words[i].count++;
            return;
        }
    }
    strcpy(words[*size].word, word);
    words[*size].count = 1;
    (*size)++;
}

int compare_words(const void *a, const void *b) {
    return strcmp(((WordCount *)a)->word, ((WordCount *)b)->word);
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = MPI_Wtime();

    FILE *file;
    char word[MAX_WORD_LENGTH];
    WordCount *local_words = malloc(1000 * sizeof(WordCount));
    int local_word_count = 0;

    if (rank == 0) {
        printf("Processo 0: Tentando abrir o arquivo palavras.txt...\n");
        fflush(stdout);
        file = fopen("/app/palavras.txt", "r");
        if (file == NULL) {
            perror("Erro ao abrir o arquivo");
            MPI_Abort(MPI_COMM_WORLD, 1);
        } else {
            printf("Arquivo palavras.txt aberto com sucesso!\n");
            fflush(stdout);
        }
    }

    while (1) {
        if (rank == 0) {
            if (fscanf(file, "%s", word) != EOF) {
                for (int i = 1; i < size; i++) {
                    MPI_Send(word, MAX_WORD_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }
            } else {
                for (int i = 1; i < size; i++) {
                    MPI_Send("", MAX_WORD_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }
                break;
            }
        } else {
            MPI_Recv(word, MAX_WORD_LENGTH, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (strlen(word) == 0) break;
        }

        #pragma omp parallel for
        for (int i = 0; i < 1; i++) {
            add_word(local_words, &local_word_count, word);
        }
    }

    if (rank == 0) {
        fclose(file);
    }

    WordCount *global_words = NULL;
    int global_word_count = 0;

    if (rank == 0) {
        global_words = malloc(1000 * size * sizeof(WordCount));
    }

    MPI_Gather(local_words, local_word_count * sizeof(WordCount), MPI_BYTE,
               global_words, local_word_count * sizeof(WordCount), MPI_BYTE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < local_word_count; j++) {
                add_word(global_words, &global_word_count, global_words[i * local_word_count + j].word);
            }
        }

        qsort(global_words, global_word_count, sizeof(WordCount), compare_words);

        printf("Contagem de palavras:\n");
        fflush(stdout);
        for (int i = 0; i < global_word_count; i++) {
            printf("%s: %d\n", global_words[i].word, global_words[i].count);
            fflush(stdout);
        }

        double end_time = MPI_Wtime();
        printf("Tempo de execução: %f segundos\n", end_time - start_time);
        fflush(stdout);

        free(global_words);
    }

    free(local_words);
    MPI_Finalize();
    return 0;
}