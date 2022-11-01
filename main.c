#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <omp.h>
#include "gconio.h"

#ifndef _POSIX_THREAD_PRIO_INHERIT
#error "This system does not support priority inherit protection in mutex"
#endif

pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t  mutexattr_prioinherit;

int vidroM = 0, vidroP, temperatura = 0, eletronica = 0, absDianteiro = 0, airbag = 0, cintoSegurancaM = 0;
int cintoSegurancaP = 0, farol = 0, trava = 0;
double tempoVP = 0;
double tempoVM = 0;
double tempoTem = 0;
double tempoAbs = 0;
double tempoAir = 0;
double tempoCP = 0;
double tempoCM = 0;
double tempoF = 0;


time_t fimVP, inicioVP;
time_t fimVM, inicioVM;
time_t fimTem, inicioTem;
time_t fimAbs, inicioAbs;
time_t fimAir, inicioAir;
time_t fimCP, inicioCP;
time_t fimCM, inicioCM;
time_t fimF, inicioF;

struct carro
{
    int temp1;
    int abrindo;
    int ativandoA;
    int ativandoAbs;
    int ativandoCM;
    int ativandoCP;
    int ativandoT;
    int ativandoF;
    int abrindoP;
    int abrindoM;
    int injecaoE;
    int velocidade;
    int velocidadeM;
    int cont;
};

struct carro c; //struct global para compartilhar valores entre as threads

void inicializa_carro()
{
    c.temp1 = 0;
    c.abrindo = 0;
    c.ativandoA = 0;
    c.ativandoAbs = 0;
    c.ativandoCM = 0;
    c.ativandoCP = 0;
    c.ativandoT = 0;
    c.ativandoF = 0;
    c.abrindoM = 0;
    c.abrindoP = 0;
    c.injecaoE = 0;
    c.velocidade = 0;
    c.velocidadeM = 0;
    c.cont = 0;
}

//THREAD QUE AGUARDA O USUARIO APERTAR UMA TECLA
void monitoramentoTeclado()
{
    char tecla; //variavel para armazenar a telha digitada

    while(1)
    {

        tecla = getch();

        if(tecla == 'v' || tecla == 'V') //se for v esta acionando o vidro
        {
            inicioVM = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            vidroM = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'c' || tecla == 'C') //Se for c esta acionando o vidro
        {
            inicioVP = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            vidroP = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'e' || tecla == 'E') //Injeção eletronica
        {
            pthread_mutex_lock(&em);
            delay(0.001);
            eletronica = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'a' || tecla == 'A') //abs dianteiro foi acionado
        {
            inicioAbs = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            absDianteiro = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'b' || tecla == 'B') //airbag acionado
        {
            inicioAir = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            airbag = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'm' || tecla == 'M') //Mexeu no cinto de segurança
        {
            inicioCM = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            cintoSegurancaM = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'p' || tecla == 'P') //Passageiro mexeu no cinto de segurança
        {
            inicioCP = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            cintoSegurancaP = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }
        else if(tecla == 'f' || tecla == 'F') //Farol
        {
           inicioF = time(NULL);
            pthread_mutex_lock(&em);
            delay(0.001);
            farol = 1;
            delay(0.005);
            pthread_mutex_unlock(&em);
        }


    }
}

//SENSOR DA INJEÇÃO ELETRONICA
void InjecaoEletonica()
{
    int velocidade[200];
    int vel;
    int soma = 0, media = 0;

    while(1)
    {
        c.injecaoE = 1;
        pthread_mutex_lock(&em);
        delay(0.5);
        vel = rand() % 200;
        c.velocidade = vel;
        c.cont++;

        media = (vel)/2;
        c.velocidadeM = media;
        pthread_mutex_unlock(&em);
    }

}

//SENSOR DA TEMPERATURA INTERNA DO MOTOR
void TemperaturaInterna()
{
    int temperatura1;
    while(1)
    {
        temperatura1 = rand() % 200;
        pthread_mutex_lock(&em);
        delay(20);
        c.temp1 = 1;
        pthread_mutex_unlock(&em);

        delay(7000);
        c.temp1 = 0;
        delay(15000);

    }
}

//SENSOR DO ABS DIANTEIRO
void absDia()
{
    while(1)
    {
        if(absDianteiro == 1)
        {
            delay(100);
            pthread_mutex_lock(&em);
            if(c.ativandoAbs == 0)
            {
                c.ativandoAbs = 1;
                delay(3000);
                c.ativandoAbs = 0;
            }
            else
            {
                c.ativandoAbs = 0;
            }
            absDianteiro = 0;
            pthread_mutex_unlock(&em);
            fimAbs = time(NULL);
            tempoAbs = fimAbs - inicioAbs;
        }

    }
}

//SENSOR DO AIRBAG
void Airbag()
{

    while(1)
    {

        if(airbag == 1)
        {
            delay(100); //Deadline relativo para uma ação de controle
            pthread_mutex_lock(&em);
            if(c.ativandoA == 0)
            {
                c.ativandoA = 1;
            }
            else
            {
                c.ativandoA = 0;
            }
            airbag = 0;
            pthread_mutex_unlock(&em);
            fimAir = time(NULL);
            tempoAbs = fimAir - inicioAir;
        }

    }
}

//SENSOR DO CINTO DE SEGURANÇA
void CintoSeguranca()
{
    sleep(1); //Deadline relativo para uma ação de controle
    while(1)
    {

        if(cintoSegurancaM == 1)
        {
            delay(1000);
            pthread_mutex_lock(&em);
            if(c.ativandoCM == 0)
            {
                c.ativandoCM = 1;
            }
            else
            {
                c.ativandoCM = 0;
            }

            cintoSegurancaM = 0;
            pthread_mutex_unlock(&em);
            fimCM = time(NULL);
            tempoCM = fimCM - inicioCM;
        }

        if(cintoSegurancaP == 1)
        {
            delay(1000);
            pthread_mutex_lock(&em);

            if(c.ativandoCP == 0)
            {
                c.ativandoCP = 1;
            }
            else
            {
                c.ativandoCP = 0;
            }

            cintoSegurancaP= 0;
            pthread_mutex_unlock(&em);
            fimCP = time(NULL);
            tempoCP = fimCP - inicioCP;
        }

    }
}

//SENSOR DO FAROL
void Farol()
{
    while(1)
    {

        if(farol == 1)
        {
            delay(1000);
            pthread_mutex_lock(&em);
            if(c.ativandoF == 0)
            {
                c.ativandoF = 1;
            }
            else
            {
                c.ativandoF = 0;
            }
            farol = 0;
            pthread_mutex_unlock(&em);
            fimF = time(NULL);
            tempoF = fimF - inicioF;
        }

    }
}

//SENSOR DO VIDRO
void Vidro()
{
    while(1)
    {

        if(vidroP == 1)
        {
            delay(1000);
            pthread_mutex_lock(&em);
            if(c.abrindoP == 0)
            {
                c.abrindoP = 1;
            }
            else
            {
                c.abrindoP = 0;
            }
            vidroP = 0;
            pthread_mutex_unlock(&em);
            fimVP = time(NULL);
            tempoVP = (double)(fimVP - inicioVP);
        }

        if(vidroM == 1)
        {
            delay(1000);
            pthread_mutex_lock(&em);
            if(c.abrindoM == 0)
            {
                c.abrindoM = 1;
            }
            else
            {
                c.abrindoM = 0;
            }
            vidroM = 0;
            pthread_mutex_unlock(&em);
            fimVM = time(NULL);
            tempoVM = (double)(fimVM - inicioVM);
        }





    }

}

void TravaPorta()
{
    while(1)
    {
        pthread_mutex_lock(&em);
        delay(1000);
        c.ativandoT = 1;

        pthread_mutex_unlock(&em);
    }

}

void display()
{


    while(1)
    {
    delay(1000);
        system("clear");
        printf("\t\t------\tMONITORAMENT DO VEICULO\t-----\n\n");

        if(c.injecaoE == 0)
        {
            printf("\nInjeção eletronica                  |\t Desativada ");
        }
        else
        {
            printf("\nInjeção eletronica                  |\t Ativada");
        }
        printf("\n--------------------------------------------------------------------------------");
        printf("\nVelocidade                          |\t %d", c.velocidade);
        printf("\n--------------------------------------------------------------------------------");
        if(c.temp1 == 1)
        {
            printf("\nTemperatura motor                   |\t Alta ");
            printf("\n--------------------------------------------------------------------------------");
        }
        else
        {
            printf("\nTemperatura motor                   |\t Normal");
            printf("\n--------------------------------------------------------------------------------");
        }
        if(c.ativandoAbs == 0)
        {
            printf("\nAbs Dianteiro                       |\t Desativado");
        }
        else
        {
            printf("\nAbs Dianteiro                       |\t Ativado  \t                        | Tempo de execução: %f", tempoAbs);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.ativandoA == 0)
        {
            printf("\nAirbag                              |\t Desativado");
        }
        else
        {
            printf("\nAirbag                              |\t Ativado  \t                        | Tempo de execução: %f", tempoAir);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.ativandoCP == 0 )
        {
            printf("\nCinto de segurança do passageiro    |\t Sem Cinto de Segurança");
        }
        else
        {
            printf("\nCinto de segurança do passageiro    |\t Com Cinto de Segurança \t          | Tempo de execução: %f", tempoCP);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.ativandoCM == 0 )
        {
            printf("\nCinto de segurança do motorista     |\t Sem Cinto de Segurança");
        }
        else
        {
            printf("\nCinto de segurança do motorista     |\t Com Cinto de Segurança  \t         | Tempo de execução: %f", tempoCM);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.ativandoF == 0)
        {
            printf("\nFarol dianteiro                     |\t Desligado");
        }
        else
        {
            printf("\nFarol dianteiro                     |\t Ligado  \t                         | Tempo de execução: %f", tempoF);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.abrindoP == 0)
        {
            printf("\nVidro do passageiro                 |\t Fechado");
        }
        else
        {
            printf("\nVidro do passageiro                 |\t Aberto  \t                         | Tempo de execução: %f", tempoVP);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.abrindoM == 0)
        {
            printf("\nVidro do motorista                  |\t Fechado");
        }
        else
        {
            printf("\nVidro do motorista                  |\t Aberto  \t                         | Tempo de execução: %f", tempoVM);
        }
        printf("\n--------------------------------------------------------------------------------");
        if(c.ativandoT == 0)
        {
            printf("\nTrava das Portas                    |\t Desativadas");
        }
        else
        {
            printf("\nTrava das Portas                    |\t Ativadas");
        }
        printf("\n--------------------------------------------------------------------------------\n");

    }
}

void init_mutex_inh();

int main()
{
    pthread_t t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
    pthread_attr_t tattr;
    int newprio = 20;
    struct sched_param param;

    /*chama a funÃ§Ã£o para inicializar o mutex com a heranÃ§a de prioridade*/
    init_mutex_inh();

    /* inicializado com atributos padrÃ£o */
    pthread_attr_init (&tattr);

    /* obter o parÃ¢metro de programaÃ§Ã£o existente */
    pthread_attr_getschedparam (&tattr, &param);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 25;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para a esteira 1*/
    pthread_create(&t1, &tattr, (void *)monitoramentoTeclado, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 22;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para a esteira 2*/
    pthread_create(&t2, NULL, (void *) InjecaoEletonica, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 23;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para a esteira 3*/
    pthread_create(&t3, NULL, (void *) TemperaturaInterna, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 2;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t4, NULL, (void *) display, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 20;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t5, NULL, (void *) Vidro, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 22;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t6, NULL, (void *) absDia, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 22;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t7, NULL, (void *) Airbag, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 23;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t8, NULL, (void *) CintoSeguranca, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 15;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t9, NULL, (void *) TravaPorta, NULL);

    /* definir a prioridade; outros parÃ¢metros nÃ£o mudaram */
    param.sched_priority = 17;

    /* definindo o novo parÃ¢metro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /*criando a thread para atualizar o display*/
    pthread_create(&t10, NULL, (void *) Farol, NULL);

    /*thread principal espera as threas criadas retornarem para encerrar o processo*/
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    pthread_join(t6, NULL);
    pthread_join(t7, NULL);
    pthread_join(t8, NULL);
    pthread_join(t9, NULL);
    pthread_join(t10, NULL);


    return 0;
}

/* Iniciando o mutex com o protocolo de prioridade */
void init_mutex_inh()
{
    int rtn;
    int mutex_protocol;

    /* Qual Ã© o protocolo padrÃ£o no host? - retirado de exemplo*/
    if ((rtn = pthread_mutexattr_init(&mutexattr_prioinherit)) != 0)
        fprintf(stderr,"pthread_mutexattr_init: %s",strerror(rtn)); //mensagem em caso de haver erro
    if ((rtn = pthread_mutexattr_getprotocol(&mutexattr_prioinherit, &mutex_protocol)) != 0)
        fprintf(stderr,"pthread_mutexattr_getprotocol: %s",strerror(rtn)); //mensagem em caso de haver erro

#ifdef DEBUG
    if (mutex_protocol == PTHREAD_PRIO_PROTECT)
        printf("Default mutex protocol is PTHREAD_PRIO_PROTECT\n");
    else if (mutex_protocol == PTHREAD_PRIO_INHERIT)
        printf("Default mutex protocol is PTHREAD_PRIO_INHERIT\n");
    else if (mutex_protocol == PTHREAD_PRIO_NONE)
        printf("Default mutex protocol is PTHREAD_PRIO_NONE\n");
    else
        printf("Default mutex protocol is unrecognized: %d\n");
#endif

    /* Define o protocolo do mutex como INHERIT - heranÃ§a de prioridade*/
    if ((rtn = pthread_mutexattr_setprotocol(&mutexattr_prioinherit, PTHREAD_PRIO_INHERIT)) != 0)
        fprintf(stderr,"pthread_mutexattr_setprotocol: %s",strerror(rtn)); //mensagem em caso de haver erro

    /* Inicialize mutex com o objeto de atributo */
    if ((rtn = pthread_mutex_init(&em,&mutexattr_prioinherit)) != 0)
        fprintf(stderr,"pthread_mutexattr_init: %s",strerror(rtn)); //mensagem em caso de haver erro

#ifdef DEBUG
    /*verificaÃ§Ã£o para identificar se o protocolo foi realmente atribuÃ­do */
    if ((rtn = pthread_mutexattr_getprotocol(&mutexattr_prioinherit, &mutex_protocol)) != 0)
        fprintf(stderr,"pthread_mutexattr_getprotocol: %s",strerror(rtn)); //mensagem em caso de haver erro

    if (mutex_protocol == PTHREAD_PRIO_PROTECT)
        printf("Default mutex protocol is PTHREAD_PRIO_PROTECT\n");
    else if (mutex_protocol == PTHREAD_PRIO_INHERIT)
        printf("Default mutex protocol is PTHREAD_PRIO_INHERIT\n");
    else if (mutex_protocol == PTHREAD_PRIO_NONE)
        printf("Default mutex protocol is PTHREAD_PRIO_NONE\n");
    else
        printf("Default mutex protocol is unrecognized: %d\n");
#endif
}


