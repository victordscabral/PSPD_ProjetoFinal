#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include <omp.h>

#define MAX_WORD_LENGTH 100

// Função para limpar a palavra: remove caracteres não alfanuméricos e converte para minúsculas
void clean_word(char *word) {
    int i = 0, j = 0;
    while (word[i]) {
        if (isalnum(word[i])) {
            word[j++] = tolower(word[i]);
        }
        i++;
    }
    word[j] = '\0';
}

// Função para contar palavras em um arquivo
void count_words(const char *filename, int *word_count, int *unique_words, int rank, int size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro: O arquivo '%s' não foi encontrado.\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char line[1024];
    char word[MAX_WORD_LENGTH];
    int local_word_count[1000] = {0};  // Supondo um máximo de 1000 palavras únicas por processo
    char local_unique_words[1000][MAX_WORD_LENGTH] = {0};
    int local_unique_count = 0;

    while (fgets(line, sizeof(line), file)) {
        #pragma omp parallel for private(word)
        for (int i = 0; i < strlen(line); i++) {
            if (isspace(line[i]) || line[i] == '\0') {
                word[0] = '\0';
                clean_word(word);
                if (strlen(word) > 0) {
                    int found = 0;
                    #pragma omp critical
                    {
                        for (int j = 0; j < local_unique_count; j++) {
                            if (strcmp(local_unique_words[j], word) == 0) {
                                local_word_count[j]++;
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            strcpy(local_unique_words[local_unique_count], word);
                            local_word_count[local_unique_count]++;
                            local_unique_count++;
                        }
                    }
                }
            } else {
                strncat(word, &line[i], 1);
            }
        }
    }

    fclose(file);

    // Combinar resultados de todos os processos
    MPI_Allreduce(local_word_count, word_count, 1000, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allgather(local_unique_words, 1000 * MAX_WORD_LENGTH, MPI_CHAR, unique_words, 1000 * MAX_WORD_LENGTH, MPI_CHAR, MPI_COMM_WORLD);
}

// Função principal
int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int word_count[1000] = {0};
    char unique_words[1000][MAX_WORD_LENGTH] = {0};

    count_words(argv[1], word_count, (int *)unique_words, rank, size);

    if (rank == 0) {
        printf("Contagem de palavras:\n");
        for (int i = 0; i < 1000; i++) {
            if (word_count[i] > 0) {
                printf("%s: %d\n", unique_words[i], word_count[i]);
            }
        }
    }

    MPI_Finalize();
    return 0;
}