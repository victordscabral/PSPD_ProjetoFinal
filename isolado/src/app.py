from collections import defaultdict
import re

def clean_word(word):
    """Remove caracteres não alfanuméricos e converte para minúsculas."""
    return re.sub(r'[^a-zA-Z0-9]', '', word).lower()

def count_words(filename):
    """Conta a ocorrência de cada palavra única em um arquivo."""
    word_count = defaultdict(int)  # Dicionário para armazenar a contagem de palavras

    try:
        with open(filename, "r") as file:
            for line in file:
                # Divide a linha em palavras e processa cada uma
                words = line.split()
                for word in words:
                    cleaned_word = clean_word(word)
                    if cleaned_word:  # Ignora palavras vazias após a limpeza
                        word_count[cleaned_word] += 1
    except FileNotFoundError:
        print(f"Erro: O arquivo '{filename}' não foi encontrado.")
        return None

    return word_count

def print_word_counts(word_count):
    """Exibe a contagem de palavras."""
    if not word_count:
        print("Nenhuma palavra foi contada.")
        return

    print("Contagem de palavras:")
    for word, count in sorted(word_count.items()):
        print(f"{word}: {count}")

def main():
    # Solicita o nome do arquivo ao usuário
    filename = "../../palavras.txt"

    # Conta as palavras no arquivo
    word_count = count_words(filename)

    # Exibe os resultados
    if word_count:
        print_word_counts(word_count)

if __name__ == "__main__":
    main()