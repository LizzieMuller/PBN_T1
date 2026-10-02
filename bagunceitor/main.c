/*

Para compilar de dentro do VS Code:

Windows/Linux: CTRL+SHIFT+B (Terminal -> Run Build Task)
macOS: COMMAND+SHIFT+B

Para executar de dentro do VS Code:

Executar normalmente: CTRL+F5 (Run -> Run Without Debugging)
Debugar: F5 (Run -> Start Debugging)

Pelo terminal:

Windows: mingw32-make
Linux/macOS: make

Para executar:

./bagunceitor [arquivo com a imagem de entrada]

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Para usar strings
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "include/stb_image.h"
#include "include/stb_image_write.h"

// Um pixel Pixel (24 bits)
typedef struct {
  unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct {
  int width, height;  // largura, altura
  int channels;       // qtd de canais (geralmente 3, RGB)
  Pixel* pixels;
} Img;

// As 2 imagens
Img in, out;

// Protótipos
void load(char* name, Img* pic);


// PROPOSTA DA LIZ:
// para R = R original x 3 + i * 1
// para G = G original x 5 + i * 2
// para B = B original x 7 + i * 3
// assim giramos R > G, G > B, B > R
// minha ideia era usar modulo dessas operacoes, mas como
// estamos usando unsigned char, ja faz automaticamente o modulo

void confusao(int height, int width, Pixel (*p)[width]) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      unsigned char r_orig = (*(*(p + i) + j)).r;
      unsigned char g_orig = (*(*(p + i) + j)).g;
      unsigned char b_orig = (*(*(p + i) + j)).b;

      (*(*(p + i) + j)).g = (unsigned char)((r_orig * 3) + (i + j) * 1); // R(1) vira G
      (*(*(p + i) + j)).b = (unsigned char)((g_orig * 5) + (i + j) * 2); // G(2) vira B
      (*(*(p + i) + j)).r = (unsigned char)((b_orig * 7) + (i + j) * 3); // B(3) vira R
    }
  }
}

// PARA INVERTER
// subtrai-se o indice i e desfaz-se a multiplicacao modular, multiplicando pelos inversos
void confusao_inversa(int height, int width, Pixel (*p)[width]) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      unsigned char r_atual = (*(*(p + i) + j)).r;
      unsigned char g_atual = (*(*(p + i) + j)).g;
      unsigned char b_atual = (*(*(p + i) + j)).b;

      unsigned char r_sem_caos = (unsigned char)(g_atual - (i + j) * 1); // R estava em G
      unsigned char g_sem_caos = (unsigned char)(b_atual - (i + j) * 2); // G estava em B
      unsigned char b_sem_caos = (unsigned char)(r_atual - (i + j) * 3); // B estava em R

      (*(*(p + i) + j)).r = (unsigned char)(r_sem_caos * 171);
      (*(*(p + i) + j)).g = (unsigned char)(g_sem_caos * 205);
      (*(*(p + i) + j)).b = (unsigned char)(b_sem_caos * 183);
    }
  }
}

int main(int argc, char* argv[]) {
  // adaptei essa parte pois estou usando CLion
  char* arquivo_entrada = (argc < 2) ? "predio32.jpg" : argv[1];

  // Carrega a imagem original
  load(arquivo_entrada, &in);

  // Exibe as dimensões na tela, para conferência
  printf("Origem   : %s %d x %d\n", argv[1], in.width, in.height);

  printf("Processando...\n");

  // Cria imagem de saída e "zera" ela
  int tam = in.width * in.height;
  out = in;
  out.pixels = malloc(tam * sizeof(Pixel));
  memset(out.pixels, 0, tam * sizeof(Pixel));

  Pixel(*pin)[in.width] = (Pixel(*)[in.width])in.pixels;
  Pixel(*pout)[in.width] = (Pixel(*)[in.width])out.pixels;

  // 1. Liga 'pin' e 'pout' copiando a matriz com aritmética de ponteiros
  for (int i = 0; i < in.height; i++) {
    for (int j = 0; j < in.width; j++) {
      *(*(pout + i) + j) = *(*(pin + i) + j);
    }
  }

  // aplica a confusao na matriz pout (e salva para ver o efeito)
  confusao(in.height, in.width, pout);
  stbi_write_png("confusao.png", out.width, out.height, 3, pout, 0);

  // desaplica a confusao na matriz pout (recupera a imagem original)
  confusao_inversa(in.height, in.width, pout);

  // grava a imagem final recuperada como PNG para registro
  stbi_write_png("saida.png", out.width, out.height, 3, pout, 0);

  free(in.pixels);
  free(out.pixels);
  return 0;
}

void load(char* name, Img* pic) {
  pic->pixels =
      (Pixel*)stbi_load(name, &pic->width, &pic->height, &pic->channels, 0);
  if (!pic->pixels) {
    printf("Erro de leitura: %s\n", stbi_failure_reason());
    exit(1);
  }
  printf("Load: %d x %d x %d\n", pic->width, pic->height, pic->channels);
  // Exibe um bloco de 8 x 8 pixels em hexadecimal (teste)
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("[%02X %02X %02X] ", pic->pixels[i].r, pic->pixels[i].g,
             pic->pixels[i].b);
    }
    printf("\n");
  }
}
