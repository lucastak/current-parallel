# README — Trabalho de Paralelização com OpenMP

## 1. Compilação

Compile todos os programas (serial e paralelo) usando GCC com suporte a OpenMP:

altere o make, a variavel CC a depender de cada sistema operacional

---

## 2. Arquivos de entrada

Todos os arquivos .in devem ficar no mesmo diretório dos executáveis, por exemplo:

matriz1.in
matriz2.in
matriz3.in
vetor.in

---

## 3. Execução dos Programas

### Multiplicação Matricial

Serial:
./matrixmult_serial 2 matriz1.in matriz2.in matriz_serial.out

Paralelo (exemplo com 4 threads):
./matrixmult_paralelo 2 matriz1.in matriz2.in matriz_paralelo.out 4

---

### CountSort

Serial:
./countsort_serial 10 vetor.in vetor_count_serial.out

Paralelo:
./countsort_paralelo 10 vetor.in vetor_count_paralelo.out 4

---

### QuickSort

Serial:
./quicksort_serial 10 vetor.in vetor_quick_serial.out

Paralelo:
./quicksort_paralelo 10 vetor.in vetor_quick_paralelo.out 4

---

### Triangulação (Eliminação Gaussiana)
Entrada: matriz aumentada N × (N+1)

Serial:
./triangulacao_serial 2 matriz3.in triangulacao_serial.out

Paralelo:
./triangulacao_paralelo 2 matriz3.in triangulacao_paralelo.out

---

## 4. Comparar serial vs paralelo

diff matriz_serial.out matriz_paralelo.out
diff vetor_count_serial.out vetor_count_paralelo.out
diff vetor_quick_serial.out vetor_quick_paralelo.out
diff triangulacao_serial.out triangulacao_paralelo.out

Se não houver saída → arquivos idênticos.

---

## 5. Limpar Binários

make clean

---

## 6. Coleta de resultados para o relatório

1. Execute cada algoritmo com três tamanhos diferentes (TAMANHO 1, TAMANHO 2, TAMANHO 3).
2. Meça os tempos impressos no terminal para:
   - versão serial
   - versão paralela com 2 threads
   - versão paralela com 4 threads
3. Calcule:
   - Speedup = tempo_serial / tempo_paralelo
   - Eficiência = speedup / quantidade_de_threads
4. Preencha tabelas e gráficos no relatório.


# Matrizes para multiplicação (ex: N = 2, 500, 1000, etc.)
./gerar_matrizes_mult 2     # gera matriz1.in e matriz2.in

# Vetor para ordenação
./gerar_vetor 10 0 100      # gera vetor.in

# Matriz aumentada para triangulação
./gerar_matriz_tri 2        # gera matriz3.in