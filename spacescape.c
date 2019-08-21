/*
Universidade de Brasilia
Instituto de Ciencias Exatas
Departamento de Ciencia da Computacao

Algoritmos e Programacao de Computadores – 1/2018

Aluno(a): Carlos Gabriel Vilas Novas Soares
Matricula: 180056298
Turma: A
Versao do compilador: gcc (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609

Descricao: Voce e uma nave e deve conseguir o maior numero de pontos possiveis destruindo inimigos e pegando mantimentos pelo caminho.Mais informacoes sobre as mecanicas estao disponiveis no readme.
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/*Definicao do grid e do combustivel maximo*/
#define LINHAS 10
#define COLUNAS 135
#define FUELTOTAL 400



/*Limpar a tela tanto no windows quanto linux*/
#ifdef _WIN32
    #define CLEAR system("cls")
#else
    #define CLEAR system("clear")
#endif

/*definindo kbhit() e getchar()*/
#ifndef _WIN32

	#include <termios.h>
	#include <fcntl.h>

    int kbhit(){
        struct termios oldt, newt;
        int ch, oldf;
        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF){
            ungetc(ch,stdin);
            return 1;
        }
        return 0;
    }
    int getch(void) {
        int ch;
        struct termios oldt;
        struct termios newt;
        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#else
    #include <conio.h>
#endif

/*
Definindo a matriz, e a vida do personagem(utilizado tambem para demarcar o fim de jogo), alem do spawn do personagem;
x e y se referem a posicao do personagem na matriz;
Variaveis como "kill_count, combustivelacumulado e o motivomorte foram declaradas globalmente,pois podem ser alteradas de varias locais e serao utilizadas para o final do jogo
Comando_tiro eh uma flag para caso o tiro seja disparado;
Tiro eh utilizado para definir '>';
x e y se referem a posicao do seu personagem;
mun_laser eh a municao laser;
*/

char mapa[LINHAS][COLUNAS], personagem, tiro, comando_tiro;
int vida, spawn_inicio , x, y, fuel, s/*sair*/, kill_count, pontos, combustivelacumulado, motivomorte, mun_laser;

/*
Preenchimento do mapa, ou seja, da matriz
Paredes/limites- do jogo e espacos
Alem de setar algumas configuracoes padroes
*/
void config_mapa(){
    srand(time(0));
    char parede,vazio;
    int i, j;
    i = 0;
    kill_count = 0;
    mun_laser = 0;
    parede = '#';
    vazio = ' ';

    for(i=0;i<LINHAS;i++){
        if((i==0) || (i==9)){
            for(j=0;j<COLUNAS;j++){
                mapa[i][j] = parede;
            }
        }
        else {
            for(j=0;j<COLUNAS;j++){
                mapa[i][j] = vazio;
            }
        }
    }
}

/*Definicoes do personagem, inimigos e fuels, como por exemplo ponto de spawn e vida maxima*/
void config_personagem(){
    vida = 1;
    fuel = FUELTOTAL;
    personagem = '+';
    mapa[4][0] = personagem;
    spawn_inicio = 0;
    tiro = '>';
}

/*Outra verificacao de colisao para evitar que o tiro passe pelo 'F' e pelo 'L'*/
void colisao_f(){
    int k,l;
    for(k=1;k<9;k++){
        for(l=0;l<=134;l++){
            if((mapa[x][1] == '>') && ((mapa[k][1] == 'F') || (mapa[k][1] == 'F'))){
                mapa[k][1] = ' ';
                mapa[k][2] = ' ';
                pontos = pontos - 20;
            }
            if ((mapa[x][1] == '>') && ((mapa[k][1] == 'L') || (mapa[k][1] == 'L'))){
                mapa[k][1] = ' ';
                mapa[k][2] = ' ';
            }
            if((mapa[k][l] == 'F') && (mapa[k][l-1] == '>') && (mapa[k][l] != 'X')){
                mapa[k][l] = ' ';
                mapa[k][l-1] = ' ';
                pontos = pontos - 20;
                combustivelacumulado = combustivelacumulado - 10;
            }
            if ((mapa[x][1] == '>') && ((mapa[k][1] == 'X') || (mapa[k][1] == 'X'))){
                mapa[k][1] = ' ';
                mapa[k][2] = ' ';
                pontos = pontos + 50;
                kill_count++;
            }
            if((mapa[k][l] == 'X') && (mapa[k][l-1] == '>') && (mapa[k][l] != 'F')){
                mapa[k][l] = ' ';
                mapa[k][l-1] = ' ';
                pontos = pontos + 50;
             kill_count++;
            }
        }
    }
}
/*Movimentacao do 'X', 'F', e 'L'*/
void movimento_npc(){
    int k, l;
    for(k=1;k<9;k++){
        for(l=0;l<=134;l++){
            /*movimento do combustivel alem de uma das deteccoes de colisao*/
            if(mapa[k][l] == 'F' && (mapa[k][l] != 'X')){              
                if(mapa[k][l-1] == '>'){
                    mapa[k][l] = ' ';
                    mapa[k][l-1] = ' ';
                    pontos = pontos - 20;
                    
                }
                else if(mapa[k][l-1] == '+'){
                    mapa[k][l] = ' ';
                    mapa[k][l-1] = '+';
                    fuel = fuel + 40; 
                }
                else{
                    mapa[k][l-1] = 'F';
                    mapa[k][l] = ' ';
                }
            }
            /*movimento dos inimigios e deteccao de colisao*/
            if(mapa[k][l] == 'X'){              
                if(mapa[k][l-1] == '>'){
                    mapa[k][l] = ' ';
                    mapa[k][l-1] = ' ';
                    pontos = pontos + 50;
                    kill_count++;
                }
                else{
                    mapa[k][l-1] = 'X';
                    mapa[k][l] = ' ';
                }
            }
            /*Movimento da municao do laser e deteccao de colisao*/
            if(mapa[k][l] == 'L'){              
                if(mapa[k][l-1] == '>'){
                    mapa[k][l] = ' ';
                    mapa[k][l-1] = ' ';
                }
                else if(mapa[k][l-1] == '+'){
                    mapa[k][l] = ' ';
                    mapa[k][l-1] = '+';
                }
                else{
                    mapa[k][l-1] = 'L';
                    mapa[k][l] = ' ';
                }
            }
            if(mapa[k][l] == '.'){
                mapa[k][l] = ' ';
            }
        }
    }
}

/*Funcionamento do Tiro tipo Laser*/
void tiro_laser(){
int  l;
    for(l=0;l<133;l++){
        if((l >=1)&&(l<=133)){
            if(mapa[x][l] == 'X'){
                pontos = pontos + 35;
                kill_count++;
            }
            mapa[x][l] = '.';
        
        }
    }
}

void movimento_foguetes(){
    int k, l;
    for(k=0;k<9;k++){
        for(l=134;l>0;l--){
            /*Definicoes da colisao do foguete, e definicao de qual acao tomar dependendo de qual for o caractere  na frente do foguete*/
            /*Neste caso, deve ser analisada  a matriz da direita para a esquerda*/
            if(mapa[k][l] == '>'){
                if(mapa[k][l+1] == 'X'){
                    if(l>0){
                       mapa[k][l] = ' ';
                    }
                    else{
                        mapa[k][0] = '+';
                    }
                   mapa[k][l+1] = ' ';
                   pontos = pontos + 50;
                   kill_count++;  
                }
                if((mapa[k][l+1] == 'F')&& (mapa[k][l] == 'F')){
                   mapa[k][l] = ' ';
                   mapa[k][l-1] = ' ';
                   mapa[k][l+1] = ' ';
                   pontos = pontos - 20;
                } 
                else{
                    mapa[k][l+1] = '>';
                    mapa[k][l] = ' ';
                }
            }
            if(mapa[k][134] == '>'){
                mapa[k][134] = ' ';    
            }
        }
    }
}

/*Primeira etapa para verificar se esta acontecendo colisao e qual o tipo de npc*/
void colisao_personagem(){
        if(mapa[x][1] == 'X'){
            vida = 0;
            motivomorte = 1;
        }
        if(mapa[x][1] == 'F'){
            fuel = fuel + 1;
            combustivelacumulado = combustivelacumulado + 40;
        }
        else if(mapa[x][1] == 'L'){
            if(mun_laser < 5){
            mun_laser++;               
            }
        } 
}
/*Inputs do usuario*/
void movimento_personagem(){
    char dir_input, vazio;
    int flag;
    flag = 0;
     dir_input = getch();

    /*Detector de input valido alem da mensagem de inicio*/
    if (flag == 0) {
        if(spawn_inicio == 0){
        printf("Prepare-se, o jogo vai comecar, basta apertar qualquer tecla!\n");
        getch();
        flag++;
        }
        else if ((dir_input != 's') && (dir_input != 'S') && (dir_input != 'w') && (dir_input != 'W') && (dir_input != 'k') && (dir_input != 'K') && (dir_input != 'l') && (dir_input != 'L') && (dir_input != 'p') && (dir_input != 'P')){
            pontos = pontos - 100;        
            printf("EI, INPUT ERRADO! Menos 100 pontos para voce!\nAperte qualquer tecla para retornar ao jogo.\n");
            getch();
        }
    }
  
    /*Spawn  da nave na posicao default*/
    if(spawn_inicio == 0){
        x = 4;
        y = 0;
        spawn_inicio++;
        vazio = ' ';
    }
    
   
    if(dir_input == 'p' || dir_input == 'P'){
        printf("JOGO PAUSADO, aperte 'p' para retomar.\n");
        while(getch() != 'p'){
            printf("JOGO PAUSADO, aperte 'p' para retomar.\n");   
        }
    }
    
    /*L de laser e K de foguete*/

    if ((dir_input == 'k') || (dir_input == 'K')) {
        comando_tiro = 1;

    }
    else if((dir_input == 'l') || (dir_input == 'L')){
        comando_tiro = 2;
        
    }

    /*Mover para cima*/
    else if ((dir_input == 'w') || (dir_input == 'W')){
        if(x!=1){
            mapa[x-1][y] = personagem;
            mapa[x][y] = vazio;
            x = x - 1;
            
        }
    }

    /*Mover para baixo*/
    else if((dir_input == 's') || (dir_input == 'S')){
        if(x!=8){
            mapa[x+1][y] = mapa[x][y];
            mapa[x][y] = vazio;
            x = x + 1;
            
        }
    }    
    
        
}
/*Spawn do tiro diratamente a frente do '+' alem de um dos testes de colisao*/
void spawn_tiro(){
    if(comando_tiro == 1){
        fuel = fuel - 3;
        if(mapa[x][2] == 'F'){
            mapa[x][0] = '+';
            mapa[x][1] = ' ';
            mapa[x][2] = ' ';
            pontos = pontos - 20;
        }
        else if(mapa[x][2] == 'L'){
            mapa[x][0] = '+';
            mapa[x][1] = ' ';
            mapa[x][2] = ' ';
        }
        else if(mapa[x][2] == 'X'){
            mapa[x][0] = '+';
            mapa[x][1] = ' ';
            mapa[x][2] = ' ';
            pontos = pontos + 50;
            kill_count++;
        }
        else if(mapa[x][1] == 'X'){
            mapa[x][1] = ' ';
            mapa[x][0] = '+';
            kill_count++;
        }
        else if(mapa[x][1] == 'F'){
            mapa[x][1] = ' ';
            mapa[x][0] = '+';
            pontos = pontos - 20;
        }
        else{
            mapa[x][1] = '>';
            mapa[x][0] = '+';
        }
    }
    if((comando_tiro == 2) && (mun_laser > 0)){
        mun_laser--;
        tiro_laser();
    }
    comando_tiro = 0;
}


/*Definicao de qual npc spawnar, caso seja 1 ira dar spawn de "aliado"(combustivel), caso 0 spawn de inimigos, caso 2 spawn de lasers*/
int chance(){
    int chance;
    chance = rand()%15 + 1;
    if(chance > 13){
        return 2;
    }
    else if((chance > 8) && (chance <= 13)){
        return 1;
    }
    else{
        return 0;
    }
}
/*Mostrar o mapa na tela*/
void show_map(){
    int i, j;
    /*Indicador do combustivel e da municao do laser*/
    if(fuel <= 75){
        if(mun_laser == 0){
            printf("\e[33mPontos:\e[39m %d\n\e[91mCombustivel: %d\n\e[96mMunicao Laser: \e[39m-/5\e[39m\n", pontos, fuel);
        }
        else if((mun_laser > 0) && (mun_laser <=2)){
            printf("\e[33mPontos:\e[39m %d\n\e[91mCombustivel: %d\n\e[96mMunicao Laser:\e[36m%d\e[39m\n", pontos, fuel, mun_laser);
        }
        else if((mun_laser > 2) && (mun_laser <= 4)){
            printf("\e[33mPontos:\e[39m %d\n\e[91mCombustivel: %d\n\e[96mMunicao Laser:\e[96m%d\e[39m\n", pontos, fuel, mun_laser);
        }
        else{
            printf("\e[33mPontos:\e[39m %d\n\e[91mCombustivel: %d\n\e[96mMunicao Laser:\e[91m<!!MAX!!>\e[39m\n", pontos, fuel);
        }

    }
    else if((fuel > 75) && (fuel < 200)){
        if(mun_laser == 0){
            printf("\e[33mPontos:\e[39m %d\n\e[93mCombustivel: %d\n\e[96mMunicao Laser: \e[39m-/5\e[39m\n", pontos, fuel);
        }
        else if((mun_laser > 0) && (mun_laser <=2)){
            printf("\e[33mPontos:\e[39m %d\n\e[93mCombustivel: %d\n\e[96mMunicao Laser:\e[36m%d\e[39m\n", pontos, fuel, mun_laser);
        }
        else if((mun_laser > 2) && (mun_laser <= 4)){
            printf("\e[33mPontos:\e[39m %d\n\e[93mCombustivel: %d\n\e[96mMunicao Laser:\e[96m%d\e[39m\n", pontos, fuel, mun_laser);
        }
        else{
            printf("\e[33mPontos:\e[39m %d\n\e[93mCombustivel: %d\n\e[96mMunicao Laser:\e[91m<!!MAX!!>\e[39m\n", pontos, fuel);
        }
    }
    else{
        if(mun_laser == 0){
            printf("\e[33mPontos:\e[39m %d\n\e[92mCombustivel: %d\n\e[96mMunicao Laser: \e[39m-/5\e[39m\n", pontos, fuel);
        }
        else if((mun_laser > 0) && (mun_laser <=2)){
            printf("\e[33mPontos:\e[39m %d\n\e[92mCombustivel: %d\n\e[96mMunicao Laser:\e[36m%d\e[39m\n", pontos, fuel, mun_laser);
        }
        else if((mun_laser > 2) && (mun_laser <= 4)){
            printf("\e[33mPontos:\e[39m %d\n\e[92mCombustivel: %d\n\e[96mMunicao Laser:\e[96m%d\e[39m\n", pontos, fuel, mun_laser);
        }
        else{
            printf("\e[33mPontos:\e[39m %d\n\e[92mCombustivel: %d\n\e[96mMunicao Laser:\e[91m<!!MAX!!>\e[39m\n", pontos, fuel);
        }
    }
/*print da matriz*/
    for(i=0;i<LINHAS;i++){
        for(j=0;j<COLUNAS;j++){
            printf("\e[0m");
            if(j==134){
                printf("\n", mapa[i][j]);
            }
            /*definicao das cores do mapa*/
            else{
                if(mapa[i][j] == '+'){
                    printf("\e[97m", mapa[i][j]);
                }
                if(mapa[i][j] == '#'){
                    printf("\e[44;34m", mapa[i][j]);
                }
                if(mapa[i][j] == '>'){
                    printf("\e[39m", mapa[i][j]);
                }
                if(mapa[i][j] == 'X'){
                    printf("\e[91m", mapa[i][j]);
                }
                if(mapa[i][j] == 'F') {
                    printf("\e[92m", mapa[i][j]);
                    
                }
                if (mapa[i][j] == '.'){
                    printf("\e[46m \e[49m");
                }
                else{
                    printf("%c", mapa[i][j]);
                }
                
            }

        }
    }
    if(fuel <= 0){
        vida = 0;
        motivomorte = 0;
    }

}

/*Definicoes de spawn dos npcs*/
void spawn_npc(int prob){
    char inimigo, aliado;
    int q;
    inimigo = 'X';
    aliado = 'F';
    /*coordenadas randomicas de spawn do npc*/
    q = rand()%8 + 1;
    if(prob == 2){
        mapa[q][133] = 'L';
    }
    else if (prob == 1){
        mapa[q][133] = aliado;
    }
    else if(prob == 0){
        mapa[q][133] = inimigo;
    }
}


void gameover(){
    CLEAR;
    /*Preferi colocar 3x a tecla para evitar do usuario sair sem ver sua pontuacao
    O jogo possui "finais diferentes", de acordo com o motivo de sua morte, e quantos aliens matou e o quanto de combustivel que pegou
    Alem disso, cada ranking revela a percepcao que o seu povo teve e voce apos sua morte.
    */
    if(pontos < 0){
        printf("*Clap* *Clap* *Clap*...\nVoce realizou a proeza de negativar os seus pontos, parabens! Voce eh um em um milhao!\n ");
        printf("Seu rank: Voce nao merece um rank!\n");
    }
    else if((kill_count >= 0) && kill_count <= 20){
        if(combustivelacumulado <= 250){
            if(motivomorte == 0){
                printf("Voce nao contribuiu para a defesa de sua especie e ainda morreu congelado no espaco!\nQuando pensarem em fracasso, pensarao em voce.\n");
            }
            else{
                printf("Mal comecou sua jornada e ja teve sua nave invadida.\nAlem de perder uma nave em perfeito estado, serviu de alimento para os seus inimigos!\n");
            }
        }
        if(combustivelacumulado > 250){
            if(motivomorte == 0){
                printf("Isso que acontece quando nao se le o manual de como usar os lasers!\n");
            }
            else{
                printf("Sabe aqueles caras que se recusam a pegar em armas?\nEsse cara eh voce, e hoje sua escolha veio cobrar o preco!\n");
            }
        }
        printf("Vamos apenas dizer que, apos sua morte, as pessoas se recusam a nomear os bebes com o mesmo nome que o seu.\n");
        printf("Seu rank: \e[35mD\n\e[39m");
    }

    else if((kill_count > 20) && (kill_count <= 60)){
        if(combustivelacumulado <= 500){
            if(motivomorte == 0){
                printf("Voce nao chegou muito longe, mas pode se consolar dizendo que pelo menos matou alguns gatos pingados.\n");
            }
            else{
                printf("Eles podem ter te matado, mas pelo menos voce deixou seu K/D positivo!\n");
            }
        }
        if(combustivelacumulado > 500){
            if(motivomorte == 0){
                printf("Voce eh o que chamam de um soldado com potencial, pena que esta morto\n");
            }
            if(motivomorte == 1){
                printf("Resolveu comemorar a marca de 500 combustiveis e eles invadiram a sua nave enquanto estava desprevinido.\nQuem sabe voce nao deveria ter esperado um planeta seguro?\n");
            }
        }
        printf("Sua morte, junto com seus companheiros, ficou conhecida como a morte dos 5000, seu nome agora esta em um memorial.\n");
        printf("Seu rank:  \e[95mC\n\e[39m");
    }

    else if(kill_count > 60 && kill_count < 100){
        if(combustivelacumulado < 1000){
            if(motivomorte == 0){
                printf("Matar nao eh tudo na vida! Tente reabastecer as vezes\n");
            }
            if(motivomorte == 1){
                printf("Apertar o gatilho eh importante, mas ja pensou em recarregar?\n");
            }
        }
        if(combustivelacumulado >= 1000){
            if(motivomorte == 0){
                printf("Voce desmaiou enquanto pilotava!\nIsso que acontece quando nao se bebe agua!\n");   
            }
            if(motivomorte == 1){
                printf("Huh, quem diria que aquele novo tripulante estaria infestado de borboletas-mutantes-espacias-devoradoras-de-carne?\n");
            }
        }
        printf("Parabens!Voce foi honrado na contra-capa do jornal!Agora 0.1%% do seu povo te conhece!\n");
        printf("Seu rank: \e[33mB\n\e[39m");
    }

    else if(kill_count >= 100){
        if(combustivelacumulado < 1500){
            if(motivomorte == 0){
                printf("Toda a energia da nave foi para os lasers, e seus propulsores ficaram sem energia!\n");
            }
            if(motivomorte == 1){
                printf("Live fast die young!\n");
            }
        }
        if(combustivelacumulado >= 1500){
            if(motivomorte == 0){
                printf("Voce eh a definicao viva de 'nadou, nadou e morreu na praia'.\n");
            }
            if(motivomorte == 1){
                printf("Ei, aqui nao existem upgrades, nao importa quantos voce mate, basta um para te matar!\n");
            }
        }
        printf("Seu povo eh extremamente grato pelos seus feitos!Gracas a voce eles estao mais perto de um novo lar!\nCorrem ainda boatos que um filme sera feito sobre a sua vida!\n");
        printf("Seu rank: \e[93mA\n\e[39m");
    }
    printf("\a\n\e[91mFIM DE JOGO!\e[39m\nVoce fez: \e[93m %d \e[39mpontos.\nVoce eliminou \e[93m%d \e[39minimigo(s).\nAperte qualquer tecla tres vezes para continuar.\n", pontos, kill_count);

    getch();
    getch();
    getch();
}

/*Comeco do jogo*/
void start_game() {
    int prob;
    CLEAR;
    pontos = 0;
    /*Cria o mapa "default"*/
    config_mapa();
    config_personagem();

    while(vida == 1){
        /*define qual tipo de npc vai ser spawnado*/
         
        if(rand()%10 > 7){
            prob = chance();
            spawn_npc(prob);
        }

        if(kbhit() == 1){
            show_map();
            movimento_personagem();
            usleep(40000);
            fuel = fuel - 2;
            CLEAR;
        }

        else  {
            show_map();
            fuel= fuel - 1;
            usleep(40000);
            CLEAR;
        }
        /*movimentos que ocorrem independentemente do input*/
        colisao_personagem();    
        movimento_foguetes();
        movimento_npc();
        colisao_f();
        spawn_tiro();        
        
    }

    gameover();
}

/*A partir desse ponto, as funcoes sao majoritariamente do menu, alem da animacao de introducao*/
void sair(){
    CLEAR;
    printf("Espero que tenha se divertido!Obrigado por jogar!\n");
    s = 1;
}
void creditos(){
    CLEAR;
    getch();    
    printf("Esse trabalho foi criado por mim, Carlos Gabriel Vilas Novas Soares(matricula 18/0056298), aluno do primeiro semestre de 2018\nNa materia de APC ministrada pela professora Carla Castanho.\nAperte qualquer tecla para retornar ao menu.\n");
    getch();
}

void instrucoes(){
    CLEAR;
    getch();
    printf("\e[93mComo jogar:\n\n\e[39m");
    printf("\e[35mVoce esta em busca de um novo lar para seu povo, em sua jornada voce ira enfrentar varios perigos!\n\e[39mUtilize \e[36m 'w' e 's' \e[39mpara se mover para cima e para baixo.\nMas tome cuidado para seu combustivel nao acabar!\nPegue os 'F' que encontrar pelo caminho para abastecer. E os 'L' para abastecer seu laser, mas lembre-se que voce pode ter apenas 5 municoes!\nPara se defender dos aliens que querem te devorar, use\e[36m 'k' \e[39mpara soltar foguetes e\e[36m 'l'\e[39m para ativar seus lasers!\n");
    printf("Caso um input invalido seja pressionado, voce perdera 100 pontos!\n");
    printf("Aperte qualquer tecla para retornar ao menu.\n\e[39m");
    getch();
}


/*Definindo elementos do menu principal*/
void Menuprincipal(){
    s = 0;
    CLEAR;
    char menu_x;
    printf("\e[92m|1| - Start\e[39m\n");
    printf("\e[95m|2| - Configuracoes \e[90m- Em desenvolvimento\n\e[39m");
    printf("\e[97m|3| - Como jogar\n\e[39m");
    printf("\e[93m|4| - Ranking \e[90m- Em desenvolvimento\n\e[39m");
    printf("\e[396m|5| - Creditos\n\e[39m");
    printf("\e[91m|6| - Sair\n\e[39m");
    printf("Escolha uma opcao:");
    scanf("%c", &menu_x);

    if((menu_x == '1') || (menu_x == '2') || (menu_x == '3') || (menu_x == '4') || (menu_x == '5') || (menu_x == '6')){
        switch (menu_x){
            case '1': start_game(); break;
            /*case '2': configuracoes(); break;*/
            case '3': instrucoes(); break;
            /*case '4': ranking(); break;*/
            case '5': creditos(); break;
            case '6': sair(); break;
            }
    }
    else{
        CLEAR;
        getch();
        printf("Escolha uma opcao valida!\nPeessione qualquer tecla retornar ao menu.\n");
        getch();
    }

}



int bem_vindo(){
  void nome(void);
    CLEAR;
    nome();
    getch();
    return 0;
}

/*Funcao principal*/
int main(){

  /*Declaracao da variavel para evitar erros, tendo em vista que elas esta apos a main*/
  void loading();
  void nome();

  loading();
  s = bem_vindo();
  
  while(s==0){
    Menuprincipal();
  }
    return 0;
}
/*Animacao de transicao do jogo, optei por fazer desse modo por ser mais pratico*/
void loading(){
  CLEAR;
  int i;
    for(i=0;i<1;i++){
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        printf("          ##########\n");
        usleep(125000);
        CLEAR;
      }
    for(i=0;i<1;i++){
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      printf("                                   ##########\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      printf("                         ##########\n");
      usleep(125000);
      CLEAR;
    }
    for(i=0;i<1;i++){
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      printf("               ##########\n");
      usleep(125000);
      CLEAR;
    }

    for(i=0;i<1;i++){
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      printf("          ##########\n");
      usleep(125000);
      CLEAR;
    }

    for(i=0;i<1;i++){
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      printf("##########\n");
      usleep(125000);
      CLEAR;
    }

    for(i=0;i<1;i++){
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      printf("#############################################\n");
      usleep(125000);
      CLEAR;
    }


    for(i=0;i<1;i++){printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    usleep(125000);
    CLEAR;
  }

    usleep(100000);

}
/*funcao da tela incial*/
void nome(){

        printf(" __                _        _        \n");
        printf("/ _\\\\ _ __   __ _ | |  ___ | |  ___\n");
        printf("\\ \\ | '_ \\  / _` || | / __|| | / _ \\ \n");
        printf("_\\ \\| |_) || (_| || || (__ | ||  __/ \n");
        printf("\\__/| .__/  \\__,_|| | \\___|| | \\___| \n");
        printf("    |_|           |_|      |_|       \n");
        printf("               __  _        _                     \n");
        printf("              / _\\| |  ___ | |  __ _  _ __    ___ \n");
        printf("              \\ \\ | | / __|| | / _` || '_ \\  / _ \\\n");
        printf("              _\\ \\| || (__ | || (_| || |_) ||  __/\n");
        printf("              \\__/| | \\___|| | \\__,_|| .__/  \\___|\n");
        printf("                  |_|      |_|       |_|          \n");

        printf("\n\n\n\nAperte qualquer tecla para iniciar!\n\n\n");
        printf("Versao: 1.0\nCriado por: Carlos Gabriel V.N. Soares - 2018\n");
     
}

