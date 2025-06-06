// SudokuCriar.cpp

#include <vector>    // Para usar listas (vetores) dinâmicos
#include <array>     // Para usar arrays fixos, mais rápidos
#include <bitset>    // Para representar conjuntos de bits (usado para controle rápido de valores)
#include <iostream>  // Para entrada e saída (cin/cout)
#include <cstdlib>   // Para funções como srand/rand
#include <ctime>     // Para obter o horário atual (usado em srand)
#include <fstream>   // Para salvar arquivos
#include <algorithm> // Para usar funções de embaralhar (shuffle)
#include <random>    // Para gerar números aleatórios modernos
#include <locale>    // Para configurar a linguagem/acentuação do terminal
#include <string>    // Para usar strings

// Função que retorna o índice da região 3x3 baseado na linha e coluna da célula
constexpr std::size_t obter_regiao(std::size_t linha, std::size_t coluna) noexcept
{
    return (linha / 3) * 3 + coluna / 3;
}

// Função que retorna a próxima linha ao percorrer o tabuleiro em ordem linha/coluna
constexpr std::size_t proxima_linha(std::size_t linha, std::size_t coluna) noexcept
{
    return linha + (coluna + 1) / 9;
}

// Função que retorna a próxima coluna ao percorrer o tabuleiro em ordem linha/coluna
constexpr std::size_t proxima_coluna(std::size_t coluna) noexcept
{
    return (coluna + 1) % 9;
}

// Função para imprimir o tabuleiro de Sudoku no terminal, com linhas e colunas separadoras
void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
    std::cout << '\n'; // Linha em branco antes do tabuleiro
    for (std::size_t linha = 0; linha < 9; ++linha)
    {
        // Imprime uma linha separadora a cada 3 linhas
        if (linha % 3 == 0 && linha != 0)
        {
            std::cout << "------+-------+------\n";
        }
        for (std::size_t coluna = 0; coluna < 9; ++coluna)
        {
            // Imprime um separador vertical a cada 3 colunas
            if (coluna % 3 == 0 && coluna != 0)
            {
                std::cout << "| ";
            }
            char c = tabuleiro[linha][coluna];
            // Mostra ponto para célula vazia, ou o valor do dígito
            if (c == '.')
                std::cout << ". ";
            else
                std::cout << c << ' ';
        }
        std::cout << '\n'; // Nova linha após cada linha do tabuleiro
    }
    std::cout << '\n'; // Linha em branco após o tabuleiro
}

// Classe responsável por resolver um Sudoku usando backtracking
class Solucao
{
public:
    // Função principal que resolve o Sudoku
    void resolverSudoku(std::vector<std::vector<char>> &tabuleiro) const noexcept
    {
        // Arrays de bits para marcar quais números já existem em cada linha, coluna e região
        std::array<std::bitset<9>, 9> linha_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::array<std::bitset<9>, 9> coluna_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::array<std::bitset<9>, 9> regiao_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        // Inicializa os bitsets com os valores já presentes no tabuleiro
        for (std::size_t linha = 0; linha < 9; ++linha)
        {
            for (std::size_t coluna = 0; coluna < 9; ++coluna)
            {
                char digito;
                if ((digito = tabuleiro[linha][coluna]) != '.')
                {
                    std::size_t idx_digito = digito - '1'; // Converte caractere '1'-'9' para 0-8
                    linha_contem[linha].set(idx_digito);
                    coluna_contem[coluna].set(idx_digito);
                    std::size_t regiao = obter_regiao(linha, coluna);
                    regiao_contem[regiao].set(idx_digito);
                }
            }
        }
        // Chama o solucionador recursivo a partir da posição (0,0)
        resolver(tabuleiro, 0, 0, linha_contem, coluna_contem, regiao_contem);
    }

private:
    // Função que encontra a próxima célula vazia a partir de uma posição
    static constexpr std::pair<std::size_t, std::size_t> proxima_posicao_vazia(std::vector<std::vector<char>> &tabuleiro, std::size_t linha, std::size_t coluna) noexcept
    {
        while (linha != 9) // Percorre até o final do tabuleiro
        {
            if (tabuleiro[linha][coluna] == '.')
            {
                return {linha, coluna}; // Retorna a posição da célula vazia
            }
            linha = proxima_linha(linha, coluna);
            coluna = proxima_coluna(coluna);
        }
        return {9, 0}; // Não encontrou, retorna posição "final"
    }

    // Função recursiva de backtracking que preenche o tabuleiro
    static bool resolver(std::vector<std::vector<char>> &tabuleiro, std::size_t linha_inicio, std::size_t coluna_inicio,
                         std::array<std::bitset<9>, 9> &linha_contem,
                         std::array<std::bitset<9>, 9> &coluna_contem,
                         std::array<std::bitset<9>, 9> &regiao_contem) noexcept
    {
        // Encontra a próxima célula vazia
        auto [linha, coluna] = proxima_posicao_vazia(tabuleiro, linha_inicio, coluna_inicio);

        // Se percorreu todo o tabuleiro, a solução está completa
        if (linha == 9)
        {
            return true;
        }

        // Calcula o índice da região 3x3
        std::size_t regiao = obter_regiao(linha, coluna);
        // Gera um bitset que indica quais dígitos já estão presentes na linha, coluna ou região
        std::bitset<9> contem = linha_contem[linha] | coluna_contem[coluna] | regiao_contem[regiao];
        // Se todos os números já estão presentes, não é possível preencher essa célula
        if (contem.all())
        {
            return false;
        }

        // Vetor com os índices dos dígitos possíveis (0-8, que representam '1'-'9')
        std::vector<std::size_t> digitos{0, 1, 2, 3, 4, 5, 6, 7, 8};
        // Embaralha a ordem dos dígitos para variar a geração do tabuleiro
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(digitos.begin(), digitos.end(), g);

        // Tenta todos os dígitos possíveis na célula
        for (std::size_t idx_digito : digitos)
        {
            if (!contem[idx_digito]) // Se esse dígito ainda não foi usado
            {
                tabuleiro[linha][coluna] = static_cast<char>(idx_digito + '1'); // Preenche a célula
                linha_contem[linha].set(idx_digito);    // Marca na linha
                coluna_contem[coluna].set(idx_digito);  // Marca na coluna
                regiao_contem[regiao].set(idx_digito);  // Marca na região
                // Chama recursivamente para próxima célula
                if (resolver(tabuleiro, linha, coluna, linha_contem, coluna_contem, regiao_contem))
                {
                    return true; // Se resolveu, retorna true
                }
                // Se não resolveu, desfaz (backtracking)
                linha_contem[linha].reset(idx_digito);
                coluna_contem[coluna].reset(idx_digito);
                regiao_contem[regiao].reset(idx_digito);
            }
        }
        // Nenhum dígito funcionou, desfaz o preenchimento e retorna false
        tabuleiro[linha][coluna] = '.';
        return false;
    }
};

// Função para gerar um tabuleiro de Sudoku completo e depois remover células
std::vector<std::vector<char>> gerar_tabuleiro_aleatorio(int num_vazios)
{
    // Inicializa o tabuleiro vazio
    std::vector<std::vector<char>> tabuleiro(9, std::vector<char>(9, '.'));

    // Preenche todo o tabuleiro com uma solução válida
    Solucao().resolverSudoku(tabuleiro);

    // Remove aleatoriamente 'num_vazios' células do tabuleiro, tornando-as vazias
    std::srand(std::time(nullptr));
    while (num_vazios > 0)
    {
        int linha = std::rand() % 9;
        int coluna = std::rand() % 9;
        if (tabuleiro[linha][coluna] != '.')
        {
            tabuleiro[linha][coluna] = '.';
            --num_vazios;
        }
    }
    // Retorna o tabuleiro com células removidas
    return tabuleiro;
}

// Função para salvar o tabuleiro de Sudoku em um arquivo texto
void salvar_tabuleiro(const std::vector<std::vector<char>> &tabuleiro, const std::string &nome_arquivo)
{
    std::ofstream arquivo(nome_arquivo);
    if (!arquivo.is_open())
    {
        std::cout << "Não foi possível salvar o arquivo: " << nome_arquivo << "\n";
        return;
    }
    // Escreve o tabuleiro no arquivo, linha por linha
    for (const auto &linha : tabuleiro)
    {
        for (const auto &celula : linha)
        {
            arquivo << celula;
        }
        arquivo << "\n";
    }
    arquivo.close();
}

// Função principal do programa
int main()
{
    std::setlocale(LC_ALL, "pt_BR.UTF-8"); // Configura acentuação no terminal

    int dificuldade;
    std::cout << "Escolha o nível de dificuldade (1 - Fácil, 2 - Médio, 3 - Difícil): ";
    std::cin >> dificuldade; // Lê a dificuldade desejada

    int num_vazios;
    // Define quantas células vazias de acordo com a dificuldade escolhida
    switch (dificuldade)
    {
    case 1:
        num_vazios = 20; // Fácil
        break;
    case 2:
        num_vazios = 40; // Médio
        break;
    case 3:
        num_vazios = 60; // Difícil
        break;
    default:
        std::cout << "Escolha inválida. Usando dificuldade Fácil.\n";
        num_vazios = 30; // Padrão caso digite errado
        break;
    }

    // Gera o tabuleiro de Sudoku aleatório com o número de células vazias escolhido
    std::vector<std::vector<char>> tabuleiro = gerar_tabuleiro_aleatorio(num_vazios);

    std::cout << "Sudoku gerado:\n";
    imprimir_tabuleiro(tabuleiro); // Exibe o tabuleiro no terminal

    std::string nome_arquivo;
    std::cout << "Digite o nome do arquivo para salvar o Sudoku (exemplo: facil.txt): ";
    std::cin >> nome_arquivo; // Lê o nome do arquivo

    salvar_tabuleiro(tabuleiro, nome_arquivo); // Salva no arquivo

    std::cout << "Tabuleiro salvo em " << nome_arquivo << "\n";

    return 0; // Fim do programa
}