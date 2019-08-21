Aluno:Carlos Gabriel Vilas Novas Soares

Spa|c|e             
  S|c|ape 
    Jogo baseado em "River Raid"

    Versao GCC: gcc (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609 
    Parametros : gcc -Wall -ansi -o 180056298_01 180056298_01.c
    
    Obs: Existem warnings quanto as cores e quanto a funcao usleep().

    Extras adicionados ao jogo:
    1-Cores, incluindo a cor do hud e dos personagens;
    2-Tiro tipo laser;
    3-Diferentes 'finais' de jogo;
    4-Funcao para pausar o jogo;
    5- Animacao de entrada.
    
    Mecanicas de jogo:

    1-Alem das cores no menu e dos personagens, a cor do elemento "combustivel" e a "municao laser" na HUD eh alterada levando em consideração a quantidade disponivel.
	-Combustivel se torna vermelho, se menor ou igual que 75, amarelo entre 76 e 200, e verde caso esteja acima.
	-Laser tambem aparece de cores diferentes, e avisa quando está no maximo.

    Deve-se coletar os 'F' para adquirir mais combustivel, nao deixando que ele chegue a zero, caso isso ocorra, o jogo se encerra.

    Deve-se evitar os inimigos 'X', pois caso haja contato com eles, o jogo acaba.

    2-Os objetos 'L' fornecem municao para a arma tipo laser, que funciona do seguinte modo:
	-Possui um limite de 5 municoes.
        -Nao consome combustivel.
        -Ao se utilizar o laser, ocorre um "flash" na linha em que o jogador se encontra, que limpa tudo em seu caminho.
        -Se ganha 35 pontos para cada 'X' eliminado, e nao se perdem pontos caso sem atingidos o elemento'F'.

    Tiros tipo foguete '>':
        -Sao consumidos 3 combustiveis consumidos para cada tiro, eles se movem na horizontal.
        -Caso atinja um 'X', 50 pontos sao ganhos.
        -Caso atinja um 'F', 20 pontos sao perdidos.
        -Caso atinja um 'L', nao se perdem pontos.
    

    Caso um input invalido ocorra, o jogador sofrera uma punicao de 100 pontos.

    3-Existem cerca de 20 diferentes finais que levam em consideracao 3 fatores, sendo eles:
         - O modo em que o usuario morreu, seja ele por falta de combustivel ou por colisao com inimigo.
         - Quantos inimigos o jogador eliminou durante sua vida.
         - Qual a quantidade total de combustivel que o coletou.
        Alem disso, sera mostrando um rank ao final da partida.
	
    4- Ao pressionar "p", o jogo é pausado.
    
    5- Foi criada uma animacao no inicio do jogo.
    
