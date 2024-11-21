#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>

struct HeapNode {
  int id;
  int id_entrada;
  int prioridade;
  int id_paciente;
  int id_medico;
  int id_sala;
  int is_retorno;
  int ja_faltou;
};

struct Heap {
  struct HeapNode node[100];
  char especialidade[50];
  int tamanho;
  int capacidade;
};

struct Entrada {
  int id;
  int prioridade;
  int id_paciente;
  char especialidade[50];
  char sintomas[256];
  char medicacoes[256];
};

struct Paciente {
  char nome[100];
  float peso;
  float altura;
  int idade;
  char telefone[15];
  int id;
};

struct Medico {
  char nome[100];
  char especialidade[50]; // Single specialty
  int id;
  int horas_trabalhadas;
  int esta_ocupado;
};

struct Saida {
  int id;
  int id_entrada;
  int prioridade;
  int id_paciente;
  int id_medico;
  int id_sala;
  int is_retorno;
};


#include "leitor.h"

void inserirHeap(struct Heap *heap, struct HeapNode node) {
  if (heap->tamanho >= heap->capacidade) {
    printf("Heap cheio\n");
    return;
  }
  heap->node[heap->tamanho] = node;
  int i = heap->tamanho;
  heap->tamanho++;
  while (i != 0 && heap->node[(i - 1) / 2].prioridade < heap->node[i].prioridade) {
    struct HeapNode temp = heap->node[i];
    heap->node[i] = heap->node[(i - 1) / 2];
    heap->node[(i - 1) / 2] = temp;
    i = (i - 1) / 2;
  }
}

void construirHeap(struct Heap *heap, struct Entrada entradas[], int num_entradas, struct Medico medicos[], int num_medicos) {
  for (int i = 0; i < num_entradas; i++) {
    struct HeapNode node;
    node.id = i;
    node.id_entrada = entradas[i].id;
    node.prioridade = entradas[i].prioridade;
    node.id_paciente = entradas[i].id_paciente;
    node.id_medico = -1; // Inicialmente sem médico
    node.id_sala = -1; // Inicialmente sem sala
    node.is_retorno = 0; // Inicialmente não é retorno
    node.ja_faltou = 0; // Inicialmente não faltou
    inserirHeap(heap, node);
  }
}

int removerHeap(struct Heap *heap, struct Saida saidas[], int *num_saidas, struct Medico medicos[], int num_medicos, int salas[][2], struct Entrada entradas[], int *medicos_ocupados, FILE *output_file) {
  if (heap->tamanho <= 0) {
    return -1;
  }

  struct HeapNode root = heap->node[0];
  struct Saida saida;
  saida.id = root.id;
  saida.id_entrada = root.id_entrada;
  saida.prioridade = root.prioridade;
  saida.id_paciente = root.id_paciente;
  saida.id_medico = root.id_medico;
  saida.id_sala = -1;
  saida.is_retorno = root.is_retorno;

    int medidx = -1;
    int salaidx = -1;

    if(root.is_retorno == 1) {
      medidx = root.id_medico;
    }else{
      for (int j = 0; j < num_medicos; j++) {
      if (strcmp(medicos[j].especialidade, heap->especialidade) == 0 && medicos[j].esta_ocupado == 0) {
      saida.id_medico = medicos[j].id;
      medidx = j;
      break;
    }
  }
}

  for (int i = 0; i < 100; i++) {
    if (salas[i][1] == 0) {
      saida.id_sala = salas[i][0];
      salaidx = i;
      break;
    }
  }

  if (medidx == -1 || salaidx == -1) {
    return -1;
  }
    
  heap->node[0] = heap->node[heap->tamanho - 1];
  heap->tamanho--;

  int i = 0;
  while (i * 2 + 1 < heap->tamanho) {
    int maxChild = i * 2 + 1;
    if (i * 2 + 2 < heap->tamanho && heap->node[i * 2 + 2].prioridade > heap->node[i * 2 + 1].prioridade) {
      maxChild = i * 2 + 2;
    }
    if (heap->node[i].prioridade >= heap->node[maxChild].prioridade) {
      break;
    }
    struct HeapNode temp = heap->node[i];
    heap->node[i] = heap->node[maxChild];
    heap->node[maxChild] = temp;
    i = maxChild;
  }

  if ((rand() % 100) < 5) {
   
    if(root.ja_faltou == 0) {
      fprintf(output_file, "Primeira falta de um paciente, substituindo vaga...\n\n");
      root.ja_faltou = 1;
      root.prioridade = heap->node[heap->tamanho].prioridade -1;
      inserirHeap(heap, root);
    }else {
      fprintf(output_file, "Segunda falta de um paciente, paciente removido da fila...\n\n");
    }
    return -1;
  }



  if (root.is_retorno == 0) {
    root.is_retorno = 1;
    root.id_medico = medidx;
    if (heap->tamanho > 80) {
      root.prioridade = heap->node[80].prioridade;
    } else {
      root.prioridade = heap->node[heap->tamanho].prioridade + 1;
    }
    inserirHeap(heap, root);
  }

  salas[salaidx][1] = 1;
  printf("hora adicionada para medico: %d\n", medidx);
  medicos[medidx].esta_ocupado = 1;
  medicos[medidx].horas_trabalhadas++;
  saidas[*num_saidas] = saida;
  (*num_saidas)++;
  (*medicos_ocupados)++;


  return 1;
}

int main() {
  // Inicializa variáveis de dia e hora
  int dia = 1, hora = 8;

  // Declara arrays de estruturas para entradas, pacientes, médicos e especialidades
  struct Entrada entradas[100];
  struct Paciente pacientes[100];
  struct Medico medicos[100];
  char especialidades[100][50];

  // Abre o arquivo de entrada "data.txt" para leitura
  FILE *file = fopen("data.txt", "r");
  if (!file) {
    perror("Erro ao abrir o arquivo");
    return 1;
  }

  // Abre o arquivo de saída "output.txt" para escrita
  FILE *output_file = fopen("output.txt", "w");
  if (!output_file) {
    perror("Erro ao abrir o arquivo de saída");
    return 1;
  }

  // Declara variáveis para armazenar o número de pacientes, médicos, entradas e especialidades
  int num_pacientes, num_medicos, num_entradas, num_especialidades;

  // Lê os dados do arquivo e preenche as estruturas
  lerArquivo(pacientes, medicos, entradas, especialidades, &num_pacientes, &num_medicos, &num_entradas, &num_especialidades, file);

  // Inicializa a matriz de salas
  int salas[100][2];
  for (int i = 0; i < 100; i++) {
    salas[i][0] = i + 1; // Número da sala
    salas[i][1] = 0; // Estado da sala (0 = livre, 1 = ocupada)
  }

  // Declara array de estruturas de saída e variável para contar o número de saídas
  struct Saida saidas[100];
  int num_saidas;

  // Declara array de heaps e variável para contar o número de heaps
  struct Heap heaps[100];
  int num_heaps = 0;

  // Inicializa os heaps para cada especialidade
  for (int i = 0; i < num_especialidades; i++) {
    heaps[i].tamanho = 0;
    heaps[i].capacidade = 100;
    strcpy(heaps[i].especialidade, especialidades[i]);
    num_heaps++;
  }

  // Insere as entradas nos heaps correspondentes às suas especialidades
  for (int i = 0; i < num_entradas; i++) {
    for (int j = 0; j < num_especialidades; j++) {
      if (strcmp(entradas[i].especialidade, especialidades[j]) == 0) {
        struct HeapNode node;
        node.id = i;
        node.id_entrada = entradas[i].id;
        node.prioridade = entradas[i].prioridade;
        node.id_paciente = entradas[i].id_paciente;
        node.id_medico = -1;
        node.id_sala = -1;
        node.is_retorno = 0;
        node.ja_faltou = 0;
        inserirHeap(&heaps[j], node);
        break;
      }
    }
  }

  // Variável para verificar se todas as especialidades estão vazias
  int especialidades_vazias = 0;

  // Loop principal que simula os dias e horas de atendimento
  while (!especialidades_vazias) {
    while (hora <= 16) {
      int especialidade_atual = 0;
      int salas_ocupadas = 0;
      int medicos_ocupados = 0;

      // Verifica se há especialidades com pacientes a serem atendidos
      especialidades_vazias = 1;
      for (int i = 0; i < num_heaps; i++) {
        if (heaps[i].tamanho > 0) {
          especialidades_vazias = 0;
          break;
        }
      }

      // Processa as entradas nos heaps
      int all_specialties_returned_minus_one = 1;
      for (int i = 0; i < num_heaps; i++) {
        if (removerHeap(&heaps[i], saidas, &num_saidas, medicos, num_medicos, salas, entradas, &medicos_ocupados, output_file) == 1) {
          // Escreve os detalhes da saída no arquivo de saída
          fprintf(output_file, "Dia: %d, Hora: %d\n", dia, hora);
          fprintf(output_file, "ID: %d\n", saidas[num_saidas-1].id);
          fprintf(output_file, "ID Entrada: %d\n", saidas[num_saidas-1].id_entrada);
          fprintf(output_file, "Prioridade: %d\n", saidas[num_saidas-1].prioridade);
          fprintf(output_file, "ID Paciente: %d\n", saidas[num_saidas-1].id_paciente);
          fprintf(output_file, "ID Médico: %d\n", saidas[num_saidas-1].id_medico);
          fprintf(output_file, "ID Sala: %d\n", saidas[num_saidas-1].id_sala);
          fprintf(output_file, "É retorno: %d\n", saidas[num_saidas-1].is_retorno);
          fprintf(output_file, "\n");
          all_specialties_returned_minus_one = 0;
        }
      }

      // Se todas as especialidades retornaram -1, sai do loop
      if (all_specialties_returned_minus_one) {
        break;
      }

      // Atualiza a especialidade atual
      especialidade_atual = (especialidade_atual + 1) % num_heaps;
    }

    // Libera todas as salas e médicos para o próximo dia
    for (int i = 0; i < 100; i++) {
      salas[i][1] = 0;
    }
    for (int i = 0; i < num_medicos; i++) {
      medicos[i].esta_ocupado = 0;
    }

    // Incrementa a hora e o dia
    hora++;
    if (hora > 16) {
      dia++;
      hora = 8;
    }
  }

  // Escreve as horas trabalhadas por cada médico em ordem não-crescente no arquivo de saída
  fprintf(output_file, "Horas trabalhadas por cada médico em ordem não-crescente:\n");
  for (int i = 0; i < num_medicos - 1; i++) {
    for (int j = i + 1; j < num_medicos; j++) {
      if (medicos[i].horas_trabalhadas < medicos[j].horas_trabalhadas) {
        struct Medico temp = medicos[i];
        medicos[i] = medicos[j];
        medicos[j] = temp;
      }
    }
  }

  // Escreve os detalhes das horas trabalhadas por cada médico no arquivo de saída
  for (int i = 0; i < num_medicos; i++) {
    fprintf(output_file, "Médico: %s, Horas Trabalhadas: %d\n", medicos[i].nome, medicos[i].horas_trabalhadas);
  }

  // Fecha os arquivos de entrada e saída
  fclose(file);
  fclose(output_file);
  return 0;
}