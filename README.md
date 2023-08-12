# Trabalho Final - Fundamentos de Computação Gráfica (INF01047)

## Integrantes:
- Arthur Alves Ferreira Melo (00333985)
- Sofia Maciel Dávila ()

## Conceito de jogo
Shooter de ondas inspirado em Binding of Isaac/Journey of the Prairie King (minigame de Stardew Valley).

O jogador nascerá no meio de um mapa quadrangular onde inimigos virão dos 4 pontos cardeais em direção a ele. 
Ele está equipado com uma pistola e um bumerange, acessáveis atráves de hotkeys. 
Ao vencer uma certa quantidade de inimigos, o mapa é alterado para uma próxima fase, em um total de três fases, na qual os inimigos estão mais resistentes e rápidos. 
Ao vencer as três fases, o jogador vence o jogo.

## Requisitos
- [ ] Possibilitar interação com o usuário via mouse/teclado.
- [ ] Objetos virtuais representados em malhas complexas.
      No mínimo, um modelo geométrico de tamanho igual ou superior a 750KB.
      Para carregar modelos geométricos no formato OBJ, pode-se utilizar bibliotecas existentes. Ex.: tinyobjloader
      Quanto maior a variedade dos modelos geométricos, melhor.
- [ ] Transformações geométricas de objetos virtuais.
      Através da interação com o teclado e/ou mouse, o usuário deve poder controlar transformações geométricas aplicadas a objetos virtuais, não somente controle da câmera.
- [ ] Controle de câmeras virtuais.
      No mínimo, sua aplicação deve implementar uma câmera look-at e uma câmera livre.
- [ ] No mínimo, um objeto virtual deve ser copiado com duas ou mais instâncias. 
      Isto é, utilizando duas ou mais Model matrix aplicadas ao mesmo conjunto de vértices.
- [ ] Testes de colisão entre objetos virtuais.
      No mínimo, sua aplicação deve utilizar três tipos de teste de colisão.
      Estes testes devem ter algum propósito dentro da lógica da aplicação.
      Os testesde colisão devem ser implementados em um arquivo à parte, nomeado "collisions.cpp"
- [ ] Modelos de iluminação de objetos geométricos.
      No mínimo, sua aplicação deve incluir objetos com os modelos de iluminação: difusa (Lambert) e Blinn-Phong.
      No mínimo, um objeto com modelo de Gouraud.
      No mínimo, um objeto com modelo de Phong.
- [ ] Mapeamento de texturas.
      Todos objetos virtuais de sua aplicação devem ter suas cores definidas através de texturas representadas por imagens, com no mínimo, três imagens distintas.
- [ ] Curvas de Bézier.
      No mínimo um objeto virtual de sua aplicação deve ter sua movimentação definida através de uma curva de Bézier cúbica. O objeto deve se movimentar de forma suave ao longo do espaço em um caminho curvo (não reto).
- [ ] Animação de Movimento baseada no tempo.
      Todas as movimentações de objetos, incluindo da câmera, devem ser computadas baseado no tempo.
      

## TODO
- [ ] Câmera em 1ª pessoa - REQ. 4
- [ ] Câmera em 3ª pessoa - REQ. 4
- [ ] Geração do cenário - REQ. 7, REQ. 8
- [ ] Geração do personagem - REQ. 2, REQ. 7, REQ. 8
- [ ] Movimentação do personagem - REQ. 1, REQ. 3, REQ. 10
- [ ] Implementação do tiro comum - REQ. 5, REQ. 6, REQ. 7, REQ. 10
- [ ] Implementação do bumerange - REQ. 5, REQ. 6, REQ. 7, REQ. 9
- [ ] Geração dos inimigos - REQ. 2, REQ. 5, REQ. 7, REQ. 8
      
