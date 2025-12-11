# Guia Completo de Testes

## Compilação

```
make clean
make
```

### Executáveis esperados

- matrixmult_serial, matrixmult_paralelo  
- countsort_serial, countsort_paralelo  
- quicksort_serial, quicksort_paralelo  
- triangulacao_serial, triangulacao_paralelo  
- gerar_matrizes_mult, gerar_vetor, gerar_matriz_tri  

**Obs.:** ajuste a variável `CC` no Makefile (ex.: gcc no Linux. Eu (Lucas) usei `/opt/homebrew/bin/gcc-15` no macOS).

---

# 1. Multiplicação Matricial (matrixmult)

## 1.1. Escolha dos tamanhos
Sugestão do enunciado:

- **TAMANHO 1:** 500 × 500  
- **TAMANHO 2:** 1000 × 1000  
- **TAMANHO 3:** 2000 × 2000 (ou maior até o serial ficar ~20–30s)

Assumindo: **N ∈ {500, 1000, 2000}**

---

## 1.2. Gerar as matrizes de entrada
Para cada tamanho N:

```
./gerar_matrizes_mult N
```

Exemplo:

```
./gerar_matrizes_mult 500
# Gera: matriz1.in e matriz2.in
```

---

## 1.3. Rodar versão serial

Formato:

```
./matrixmult_serial <N> matriz1.in matriz2.in matriz_mult_serial.out
```

Exemplo:

```
./matrixmult_serial 500 matriz1.in matriz2.in matriz_mult_serial_500.out
```

Registrar:

```
Tempo serial (s): <valor>
```

Repetir 3 vezes → calcular média.

---

## 1.4. Rodar versão paralela (2 e 4 threads)

Formato:

```
./matrixmult_paralelo <N> matriz1.in matriz2.in matriz_mult_paralelo.out <num_threads>
```

Exemplos:

```
# 2 threads
./matrixmult_paralelo 500 matriz1.in matriz2.in matriz_mult_paralelo_500_t2.out 2

# 4 threads
./matrixmult_paralelo 500 matriz1.in matriz2.in matriz_mult_paralelo_500_t4.out 4
```

Registrar tempos:

```
Threads: 2
Tempo paralelo (s): <valor>

Threads: 4
Tempo paralelo (s): <valor>
```

Repetir 3 vezes → calcular média.

---

## 1.5. Verificar correção

```
diff matriz_mult_serial_500.out matriz_mult_paralelo_500_t2.out
diff matriz_mult_serial_500.out matriz_mult_paralelo_500_t4.out
```

Nenhuma saída → arquivos idênticos.

---

# 2. Algoritmos de Ordenação (CountSort e QuickSort)

O professor exige:

- Entrada: N e arquivo de vetor  
- Saída: vetor_ordenado.out  

Você deve:

- Usar **mesmos tamanhos** para ambos algoritmos  
- Usar **o mesmo vetor de entrada** para CountSort e QuickSort  
- Isso permite comparar desempenho e cache

---

## 2.1. Escolha dos tamanhos

- **100.000**  
- **200.000**  
- **400.000** (ou maior até o serial ~20–30s)

---

## 2.2. Gerar vetores de entrada

```
./gerar_vetor <N> 0 100000
```

Exemplo (100k):

```
./gerar_vetor 100000 0 100000
cp vetor.in vetor_100k.in
```

Repetir para 200k e 400k.

---

## 2.3. CountSort — Serial

Formato:

```
./countsort_serial <N> <arquivo_entrada> <arquivo_saida>
```

Exemplo:

```
./countsort_serial 100000 vetor_100k.in vetor_count_serial_100k.out
```

Registrar:

```
Tempo serial (s): <valor>
```

Repetir 3 vezes → média.

---

## 2.4. CountSort — Paralelo

```
./countsort_paralelo <N> <arquivo_entrada> <arquivo_saida> <num_threads>
```

Exemplos:

```
./countsort_paralelo 100000 vetor_100k.in vetor_count_paralelo_100k_t2.out 2
./countsort_paralelo 100000 vetor_100k.in vetor_count_paralelo_100k_t4.out 4
```

Registrar tempos → média.

---

## 2.5. QuickSort — Serial

```
./quicksort_serial <N> <arquivo_entrada> <arquivo_saida>
```

Exemplo:

```
./quicksort_serial 100000 vetor_100k.in vetor_quick_serial_100k.out
```

---

## 2.6. QuickSort — Paralelo

```
./quicksort_paralelo <N> <arquivo_entrada> <arquivo_saida> <num_threads>
```

---

## 2.7. Verificar correção

```
diff vetor_count_serial_100k.out vetor_count_paralelo_100k_t2.out
diff vetor_quick_serial_100k.out vetor_quick_paralelo_100k_t2.out
```

---

# 3. Triangulação / Eliminação Gaussiana

Entrada: matriz aumentada **N × (N+1)**

## 3.1. Tamanhos

- **250 × 251**  
- **500 × 501**  
- **1000 × 1001**

---

## 3.2. Gerar matrizes aumentadas

```
./gerar_matriz_tri 250
cp matriz3.in matriz_250.in
```

Repetir para 500 e 1000.

---

## 3.3. Rodar versão serial

```
./triangulacao_serial <N> <entrada> <saida>
```

Exemplo:

```
./triangulacao_serial 250 matriz_250.in triangulacao_serial_250.out
```

---

## 3.4. Rodar versão paralela

```
./triangulacao_paralelo <N> <entrada> <saida> <num_threads>
```

Exemplo:

```
./triangulacao_paralelo 250 matriz_250.in triangulacao_paralelo_250_t2.out 2
```

---

## 3.5. Verificar correção

```
diff triangulacao_serial_250.out triangulacao_paralelo_250_t2.out
```

---

# 4. Montagem das Tabelas (Tempo, Speedup, Eficiência)

Para cada algoritmo e tamanho:

Calcular:

```
Speedup_2 = T_serial / T_paralelo_2threads
Speedup_4 = T_serial / T_paralelo_4threads
Eficiência_2 = Speedup_2 / 2
Eficiência_4 = Speedup_4 / 4
```

### Modelo de tabela

| N | Threads | Tempo (s) | Speedup | Eficiência |
|---|---------|-----------|---------|------------|
| 500 | 1 | ... | 1.00 | 1.00 |
| 500 | 2 | ... | ... | ... |
| 500 | 4 | ... | ... | ... |

Gerar gráficos:

- Speedup × tamanho  
- Eficiência × tamanho  

---

# 5. Comparação de Cache (CountSort vs QuickSort)

1. Escolher tamanho único (ex.: 400k)  
2. Gerar vetor:

```
./gerar_vetor 400000 0 100000
cp vetor.in vetor_cache.in
```

3. Rodar ambos algoritmos com **mesmo arquivo**  
4. Medir cache com:

```
valgrind --tool=cachegrind ./exec...
perf stat ./exec...
```

5. Comparar:

- Qual tem mais cache-misses?  
- Qual roda mais rápido?  
- Relacionar estrutura do algoritmo vs comportamento de memória.  
