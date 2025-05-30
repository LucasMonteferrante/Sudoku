#include <vector>
#include <array>
#include <bitset>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <random>
#include <locale>
#include <iostream>

// Função para obter a região 3x3 correspondente a uma determinada linha e coluna
constexpr std::size_t obter_regiao(std::size_t linha, std::size_t coluna) noexcept
{
    return (linha / 3) * 3 + coluna / 3;
}

// Função para obter a próxima linha a partir da posição atual
constexpr std::size_t proxima_linha(std::size_t linha, std::size_t coluna) noexcept
{
    return linha + (coluna + 1) / 9;
}

// Função para obter a próxima coluna a partir da posição atual
constexpr std::size_t proxima_coluna(std::size_t coluna) noexcept
{
    return (coluna + 1) % 9;
}

class Solucao
{
public:
    // Função principal para resolver o Sudoku
    void resolverSudoku(std::vector<std::vector<char>> &tabuleiro) const noexcept
    {
        // Arrays para acompanhar os dígitos presentes em cada linha, coluna e região 3x3
        std::array<std::bitset<9>, 9> linha_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::array<std::bitset<9>, 9> coluna_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::array<std::bitset<9>, 9> regiao_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        // Preencher os arrays com os valores iniciais do tabuleiro
        for (std::size_t linha = 0; linha < 9; ++linha)
        {
            for (std::size_t coluna = 0; coluna < 9; ++coluna)
            {
                char digito;
                if ((digito = tabuleiro[linha][coluna]) != '.')
                {
                    std::size_t idx_digito = digito - '1';
                    linha_contem[linha].set(idx_digito);
                    coluna_contem[coluna].set(idx_digito);
                    std::size_t regiao = obter_regiao(linha, coluna);
                    regiao_contem[regiao].set(idx_digito);
                }
            }
        }
        // Chamar a função recursiva para resolver o tabuleiro
        resolver(tabuleiro, 0, 0, linha_contem, coluna_contem, regiao_contem);
    }

private:
    // Função para encontrar a próxima posição vazia no tabuleiro
    static constexpr std::pair<std::size_t, std::size_t> proxima_posicao_vazia(std::vector<std::vector<char>> &tabuleiro, std::size_t linha, std::size_t coluna) noexcept
    {
        while (linha != 9)
        {
            if (tabuleiro[linha][coluna] == '.')
            {
                return {linha, coluna};
            }
            linha = proxima_linha(linha, coluna);
            coluna = proxima_coluna(coluna);
        }
        return {9, 0};
    }

    // Função recursiva para resolver o Sudoku
    static bool resolver(std::vector<std::vector<char>> &tabuleiro, std::size_t linha_inicio, std::size_t coluna_inicio,
                         std::array<std::bitset<9>, 9> &linha_contem,
                         std::array<std::bitset<9>, 9> &coluna_contem,
                         std::array<std::bitset<9>, 9> &regiao_contem) noexcept
    {
        // Encontrar a próxima posição vazia
        auto [linha, coluna] = proxima_posicao_vazia(tabuleiro, linha_inicio, coluna_inicio);

        // Se não houver mais posições vazias, o tabuleiro está resolvido
        if (linha == 9)
        {
            return true;
        }

        // Obter a região 3x3 correspondente
        std::size_t regiao = obter_regiao(linha, coluna);
        // Verificar quais dígitos já estão presentes na linha, coluna e região
        std::bitset<9> contem = linha_contem[linha] | coluna_contem[coluna] | regiao_contem[regiao];
        if (contem.all())
        {
            return false;
        }

        // Embaralhar os dígitos de 0 a 8 para tentar diferentes possibilidades
        std::vector<std::size_t> digitos{0, 1, 2, 3, 4, 5, 6, 7, 8};
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(digitos.begin(), digitos.end(), g);

        // Tentar preencher a posição com cada dígito possível
        for (std::size_t idx_digito : digitos)
        {
            if (!contem[idx_digito])
            {
                tabuleiro[linha][coluna] = static_cast<char>(idx_digito + '1');
                linha_contem[linha].set(idx_digito);
                coluna_contem[coluna].set(idx_digito);
                regiao_contem[regiao].set(idx_digito);
                if (resolver(tabuleiro, linha, coluna, linha_contem, coluna_contem, regiao_contem))
                {
                    return true;
                }
                // Desfazer a tentativa se não resolver o Sudoku
                linha_contem[linha].reset(idx_digito);
                coluna_contem[coluna].reset(idx_digito);
                regiao_contem[regiao].reset(idx_digito);
            }
        }
        // Restaurar a posição vazia se nenhuma tentativa funcionar
        tabuleiro[linha][coluna] = '.';
        return false;
    }
};

// Função para imprimir o tabuleiro do Sudoku
void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
    for (std::size_t linha = 0; linha < 9; ++linha)
    {
        for (std::size_t coluna = 0; coluna < 9; ++coluna)
        {
            std::cout << tabuleiro[linha][coluna] << " ";
        }
        std::cout << "\n";
    }
}

// Função para gerar um tabuleiro de Sudoku aleatório com um número especificado de buracos
std::vector<std::vector<char>> gerar_tabuleiro_aleatorio(int num_vazios)
{
    std::vector<std::vector<char>> tabuleiro(9, std::vector<char>(9, '.'));

    // Gerar um tabuleiro completo e resolvido
    Solucao().resolverSudoku(tabuleiro);

    // Remover alguns números para criar um quebra-cabeça
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
    return tabuleiro;
}

// Função para salvar o tabuleiro em um arquivo
void salvar_tabuleiro(const std::vector<std::vector<char>> &tabuleiro, const std::string &nome_arquivo)
{
    std::ofstream arquivo(nome_arquivo);
    for (const auto &linha : tabuleiro)
    {
        for (const auto &celula : linha)
        {
            arquivo << celula << " ";
        }
        arquivo << "\n";
    }
}

int main()
{
    // Acentos especiais
    std::setlocale(LC_ALL, "pt_BR.UTF-8");
    int dificuldade;
    std::cout << "Escolha o nível de dificuldade (1 - Fácil, 2 - Médio, 3 - Difícil): ";
    std::cin >> dificuldade;

    int num_vazios;
    switch (dificuldade)
    {
    case 1:
        num_vazios = 20;
        break;
    case 2:
        num_vazios = 40;
        break;
    case 3:
        num_vazios = 60;
        break;
    default:
        std::cout << "Escolha inválida. Usando dificuldade Fácil.\n";
        num_vazios = 30;
        break;
    }

    // Gerar um tabuleiro de Sudoku com a dificuldade especificada
    std::vector<std::vector<char>> tabuleiro = gerar_tabuleiro_aleatorio(num_vazios);

    std::cout << "Quebra-cabeça de Sudoku gerado:\n";
    imprimir_tabuleiro(tabuleiro);

    // Salvar o tabuleiro em um arquivo
    salvar_tabuleiro(tabuleiro, "sudoku_gerado.txt");

    return 0;
}