
  #include <stdio.h>
  #include <string.h>
  
    struct HeapNode {
    int prioridade;
    int id_paciente;
    int id_medico;
    int id_sala;
    int is_retorno;
    int ja_faltou;
  };
  
    struct Entrada {
    int prioridade;
    int id_paciente;
    int especialidade;
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
    char especialidade[10][50];
    int id;
    int horas_trabalhadas;
  };

  void lerPacientes(struct Paciente pacientes[], int *num_pacientes, FILE *file) {
    char line[256];
    *num_pacientes = 0;
    while (fgets(line, sizeof(line), file)) {
      if (strncmp(line, "Paciente", 8) == 0) {
        break;
      }
    }
    while (fgets(line, sizeof(line), file) && strncmp(line, "Medico", 6) != 0) {
      sscanf(line, "%[^,],%f,%f,%d,%[^,],%d", pacientes[*num_pacientes].nome, &pacientes[*num_pacientes].peso, &pacientes[*num_pacientes].altura, &pacientes[*num_pacientes].idade, pacientes[*num_pacientes].telefone, &pacientes[*num_pacientes].id);
      (*num_pacientes)++;
    }
  }

  void lerMedicos(struct Medico medicos[], int *num_medicos, FILE *file) {
    char line[256];
    *num_medicos = 0;
    while (fgets(line, sizeof(line), file)) {
      if (strncmp(line, "Medico", 6) == 0) {
        break;
      }
    }
    while (fgets(line, sizeof(line), file) && strncmp(line, "Entrada", 7) != 0) {
      sscanf(line, "%[^,],%[^,],%d", medicos[*num_medicos].nome, medicos[*num_medicos].especialidade[0], &medicos[*num_medicos].id);
      medicos[*num_medicos].horas_trabalhadas = 0;
      (*num_medicos)++;
    }
  }

  void lerEntradas(struct Entrada entradas[], int *num_entradas, FILE *file) {
    char line[256];
    *num_entradas = 0;
    while (fgets(line, sizeof(line), file)) {
      if (strncmp(line, "Entrada", 7) == 0) {
        break;
      }
    }
    while (fgets(line, sizeof(line), file)) {
      sscanf(line, "%d,%d,%d,%[^,],%[^,]", &entradas[*num_entradas].prioridade, &entradas[*num_entradas].id_paciente, &entradas[*num_entradas].especialidade, entradas[*num_entradas].sintomas, entradas[*num_entradas].medicacoes);
      (*num_entradas)++;
    }
  }


  struct Heap {
    struct HeapNode node[100];
    int tamanho;
    int capacidade;
  };

  int main() {

    struct Entrada entradas[100];
    struct Paciente pacientes[100];
    struct Medico medicos[100];
    struct Heap heap;

    FILE *file = fopen("data.txt", "r");
    if (!file) {
      perror("Erro ao abrir o arquivo");
      return 1;
    }

    int num_pacientes, num_medicos, num_entradas;
    lerPacientes(pacientes, &num_pacientes, file);
    lerMedicos(medicos, &num_medicos, file);
    lerEntradas(entradas,&num_entradas, file);

    fclose(file);

  char especialidades[100][50];
  int salas[100];

  heap.tamanho = 0;
  heap.capacidade = 100;

  heap.node[0].prioridade = 0;
  heap.node[0].id_paciente = 0;
  heap.node[0].id_medico = 0;
  heap.node[0].id_sala = 0;
  heap.tamanho++;

  printf("Pacientes:\n");
  for (int i = 0; i < num_pacientes; i++) {
    printf("Nome: %s\n", pacientes[i].nome);
    printf("Peso: %.2f\n", pacientes[i].peso);
    printf("Altura: %.2f\n", pacientes[i].altura);
    printf("Idade: %d\n", pacientes[i].idade);
    printf("Telefone: %s\n", pacientes[i].telefone);
    printf("ID: %d\n", pacientes[i].id);
    printf("\n");
  }

}