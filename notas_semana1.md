#Notas Da Primeira Semana - Observando o Código

Grupo - Felipe Leivas, Gustavo Bordin, Maria Eduarda Trevisan, Maria Elisa Faria e Sarah Mendes Ferraz

**O que é o toytrace?
    - O toytrace é uma ferramenta de monitoramento insipirada no strace. Ela é responsável por "espiar" um programa específico durante sua execução, interceptando todas as chamadas do sistema (syscalls) feitas por ele, como: abertura de arquivos, escrita na tela e execução de outros programas.

**Como funciona?
    - O toytrace utiliza a API ptrace do Linux. Sendo passo a passo:

        1. O Toytrace cria um processo filho utilizando fork ()
        2. O Filho avisa o Kernel que será monitorado (PTRACE_TRACEME)
        3. O filho para com raise (SIGSTOP) para dar tempo ao pai configurar a monitoração
        4. O pai configura as opções do ptrace
        5. O filho executa o programa alvo via execvp ()
        6. A cada syscall, o filho para automaticamente e o pai lê os registradores.

**Onde estão os TODOs?

    -Semana 2: launch_tracee() e wait_for_initial_stop() em src/trace_runtime.c
    -Semana 3: configure_trace_options(), resume_until_next_syscall() e wait_for_syscall_stop()
    -Semana 4: fill_event_from_regs() e leitura dos registradores com PTRACE_GETREGS

**O que são registradores?

    - Registradores são espaços de memória dentro do processador.

    No toytrace, os que mais importam são: 
        -  orig_rax: número da syscall
        - rax: valor de retorno
        - rdi, rsi, rdx: argumentos da syscall

**Testes Realizados

    - make: compilou com sucesso (apenas warnings esperados)
    - ./toytrace --help: exibiu a ajuda corretamente
    - ./tests/targets/hello_write: imprimiu "ola toytrace"
    - ./toytrace trace -- ./tests/targets/hello_write: exibiu "erro: TODO Semana 2: implementar launch_tracee()" conforme esperado
    