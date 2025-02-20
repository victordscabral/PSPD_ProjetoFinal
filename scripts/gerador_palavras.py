import random

# Lista de palavras em português
palavras = [
    "afeição", "companheirismo", "lar", "contentamento", "ambiente", "astro-rei", "garoa",
    "pétala", "serra", "correnteza", "existência", "bem-estar", "júbilo", "confiança",
    "riso", "ocupação", "parentesco", "serenidade", "claridade", "astro", "pspd"
]

# Gerar 250 milhões de palavras aleatórias
with open("palavras.txt", "w") as arquivo:
    for _ in range(250000000):
        arquivo.write(random.choice(palavras) + "\n")

print("Arquivo 'palavras.txt' gerado com sucesso!")