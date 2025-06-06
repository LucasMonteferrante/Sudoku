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

// Função global para imprimir o tabuleiro com grade e espaços antes e depois
void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
    std::cout << '\n';
    for (std::size_t linha = 0; linha < 9; ++linha)
    {
        if (linha % 3 == 0 && linha != 0)
        {
            std::cout << "------+-------+------\n";
        }
        for (std::size_t coluna = 0; coluna < 9; ++coluna)
        {
            if (coluna % 3 == 0 && coluna != 0)
            {
                std::cout << "| ";
            }
            char c = tabuleiro[linha][coluna];
            if (c == '.')
                std::cout << ". ";
            else
                std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

class Solucao
{
public:
    // Função principal para resolver o Sudoku
    void resolverSudoku(std::vector<std::vector<char>> &tabuleiro) const noexcept
    {
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
        resolver(tabuleiro, 0, 0, linha_contem, coluna_contem, regiao_contem);
    }

private:
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

    static bool resolver(std::vector<std::vector<char>> &tabuleiro, std::size_t linha_inicio, std::size_t coluna_inicio,
                         std::array<std::bitset<9>, 9> &linha_contem,
                         std::array<std::bitset<9>, 9> &coluna_contem,
                         std::array<std::bitset<9>, 9> &regiao_contem) noexcept
    {
        auto [linha, coluna] = proxima_posicao_vazia(tabuleiro, linha_inicio, coluna_inicio);

        if (linha == 9)
        {
            return true;
        }

        std::size_t regiao = obter_regiao(linha, coluna);
        std::bitset<9> contem = linha_contem[linha] | coluna_contem[coluna] | regiao_contem[regiao];
        if (contem.all())
        {
            return false;
        }

        std::vector<std::size_t> digitos{0, 1, 2, 3, 4, 5, 6, 7, 8};
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(digitos.begin(), digitos.end(), g);

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
                linha_contem[linha].reset(idx_digito);
                coluna_contem[coluna].reset(idx_digito);
                regiao_contem[regiao].reset(idx_digito);
            }
        }
        tabuleiro[linha][coluna] = '.';
        return false;
    }
};

std::vector<std::vector<char>> gerar_tabuleiro_aleatorio(int num_vazios)
{
    std::vector<std::vector<char>> tabuleiro(9, std::vector<char>(9, '.'));

    Solucao().resolverSudoku(tabuleiro);

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

void salvar_tabuleiro(const std::vector<std::vector<char>> &tabuleiro, const std::string &nome_arquivo)
{
    std::ofstream arquivo(nome_arquivo);
    if (!arquivo.is_open())
    {
        std::cout << "Não foi possível salvar o arquivo: " << nome_arquivo << "\n";
        return;
    }
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

int main()
{
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

    std::vector<std::vector<char>> tabuleiro = gerar_tabuleiro_aleatorio(num_vazios);

    std::cout << "Sudoku gerado:\n";
    imprimir_tabuleiro(tabuleiro);

    std::string nome_arquivo;
    std::cout << "Digite o nome do arquivo para salvar o Sudoku (exemplo: facil.txt): ";
    std::cin >> nome_arquivo;

    salvar_tabuleiro(tabuleiro, nome_arquivo);

    std::cout << "Tabuleiro salvo em " << nome_arquivo << "\n";

    return 0;
}