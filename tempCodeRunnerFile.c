int main() {
  int dia = 1, hora = 8;
  struct Entrada entradas[100];
  struct Paciente pacientes[100];
  struct Medico medicos[100];
  char especialidades[100][50];

  FILE *file = fopen("data.txt", "r");
  if (!file) {
    perror("Erro ao abrir o arquivo");
    return 1;
  }
  FILE *output_file = fopen("output.txt", "w");
  if (!output_file) {
    perror("Erro ao abrir o arquivo de saída");
    return 1;
  }

  int num_pacientes, num_medicos, num_entradas, num_especialidades;
  lerArquivo(pacientes, medicos, entradas, especialidades, &num_pacientes, &num_medicos, &num_entradas, &num_especialidades, file);

  int salas[100][2];

  for (int i = 0; i < 100; i++) {
    salas[i][0] = i + 1; 
    salas[i][1] = 0; 
  }

  struct Saida saidas[100];
  int num_saidas;

  struct Heap heaps[100];
  int num_heaps = 0;

  for (int i = 0; i < num_especialidades; i++) {
    heaps[i].tamanho = 0;
    heaps[i].capacidade = 100;
    strcpy(heaps[i].especialidade, especialidades[i]);
    num_heaps++;
  }

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
  int especialidades_vazias = 0;
  while (!especialidades_vazias) { 
    while (hora <= 16) {
      
      int especialidade_atual = 0;
      int salas_ocupadas = 0;
      int medicos_ocupados = 0;
      
      while (salas_ocupadas < 100 && medicos_ocupados < num_medicos) {

        especialidades_vazias = 1;
        for (int i = 0; i < num_heaps; i++) {
          if (heaps[i].tamanho > 0) {
            especialidades_vazias = 0;
            break;
          }
        }

        int all_specialties_returned_minus_one = 1;
        for (int i = 0; i < num_heaps; i++) {
          if (removerHeap(&heaps[i], saidas, &num_saidas, medicos, num_medicos, salas, entradas, &medicos_ocupados, output_file) != -1) {

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
        if (all_specialties_returned_minus_one) {
          break;
        }
        especialidade_atual = (especialidade_atual + 1) % num_heaps;
        
      }


      for (int i = 0; i < 100; i++) {
        salas[i][1] = 0;
      }
      for (int i = 0; i < num_medicos; i++) {
        medicos[i].esta_ocupado = 0;
      }

      hora++;
    }
    dia++;
    hora = 8;
  

  }
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


  for (int i = 0; i < num_medicos; i++) {
    fprintf(output_file, "Médico: %s, Horas Trabalhadas: %d\n", medicos[i].nome, medicos[i].horas_trabalhadas);
  }
return 0;
}