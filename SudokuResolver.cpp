// SudokuResolver.cpp

#include <vector>
#include <array>
#include <bitset>
#include <iostream>
#include <fstream>
#include <chrono>
#include <locale>
#include <string>
#include <set>

// Função para obter a região 3x3 correspondente a uma determinada linha e coluna
constexpr std::size_t obter_regiao(std::size_t linha, std::size_t coluna) noexcept
{
    return (linha / 3) * 3 + coluna / 3;
}

// Função para validar o tabuleiro antes de tentar resolver
std::string validar_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
    // Verificar linhas
    for (int i = 0; i < 9; ++i)
    {
        std::set<char> s;
        for (int j = 0; j < 9; ++j)
        {
            char c = tabuleiro[i][j];
            if (c != '.')
            {
                if (s.count(c))
                    return "Número repetido na linha " + std::to_string(i + 1) + ".";
                s.insert(c);
            }
        }
    }
    // Verificar colunas
    for (int j = 0; j < 9; ++j)
    {
        std::set<char> s;
        for (int i = 0; i < 9; ++i)
        {
            char c = tabuleiro[i][j];
            if (c != '.')
            {
                if (s.count(c))
                    return "Número repetido na coluna " + std::to_string(j + 1) + ".";
                s.insert(c);
            }
        }
    }
    // Verificar blocos 3x3
    for (int bi = 0; bi < 3; ++bi)
    {
        for (int bj = 0; bj < 3; ++bj)
        {
            std::set<char> s;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    char c = tabuleiro[3 * bi + i][3 * bj + j];
                    if (c != '.')
                    {
                        if (s.count(c))
                            return "Número repetido no bloco 3x3 começando em linha " + std::to_string(3 * bi + 1) + ", coluna " + std::to_string(3 * bj + 1) + ".";
                        s.insert(c);
                    }
                }
            }
        }
    }
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (tabuleiro[i][j] != '.' && (tabuleiro[i][j] < '1' || tabuleiro[i][j] > '9'))
                return "Caractere inválido na posição linha " + std::to_string(i + 1) +
                       ", coluna " + std::to_string(j + 1) + ".";
    return "";
}

class Solucao
{
public:
    // Agora retorna bool: true se conseguiu resolver, false se não tem solução
    bool resolverSudoku(std::vector<std::vector<char>> &tabuleiro) const noexcept
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
        // Retornar o resultado da recursão
        return resolver(tabuleiro, 0, 0, linha_contem, coluna_contem, regiao_contem);
    }

private:
    static constexpr std::pair<std::size_t, std::size_t> proxima_posicao_vazia(const std::vector<std::vector<char>> &tabuleiro, std::size_t linha, std::size_t coluna) noexcept
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

        for (std::size_t idx_digito = 0; idx_digito < 9; ++idx_digito)
        {
            if (!contem[idx_digito])
            {
                tabuleiro[linha][coluna] = static_cast<char>(idx_digito + '1');
                linha_contem[linha].set(idx_digito);
                coluna_contem[coluna].set(idx_digito);
                regiao_contem[regiao].set(idx_digito);

                // Saída de depuração
                std::cout << "Colocando " << (char)(idx_digito + '1') << " em (" << linha << ", " << coluna << ")\n";
                imprimir_tabuleiro(tabuleiro);

                if (resolver(tabuleiro, linha, coluna, linha_contem, coluna_contem, regiao_contem))
                {
                    return true;
                }

                // Saída de depuração para backtracking
                std::cout << "Retrocedendo em (" << linha << ", " << coluna << ")\n";
                tabuleiro[linha][coluna] = '.';
                linha_contem[linha].reset(idx_digito);
                coluna_contem[coluna].reset(idx_digito);
                regiao_contem[regiao].reset(idx_digito);
            }
        }
        return false;
    }

    static constexpr std::size_t proxima_linha(std::size_t linha, std::size_t coluna) noexcept
    {
        return linha + (coluna + 1) / 9;
    }

    static constexpr std::size_t proxima_coluna(std::size_t coluna) noexcept
    {
        return (coluna + 1) % 9;
    }

    // Função de depuração interna (não utilizada no main, pode deixar se quiser)
    static void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
    {
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
    }
};

// Função global para imprimir o tabuleiro com grades
void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
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
}

std::vector<std::vector<char>> carregar_tabuleiro(const std::string &nome_arquivo)
{
    std::vector<std::vector<char>> tabuleiro(9, std::vector<char>(9, '.'));
    std::ifstream arquivo(nome_arquivo);
    if (arquivo.is_open())
    {
        for (std::size_t linha = 0; linha < 9; ++linha)
        {
            for (std::size_t coluna = 0; coluna < 9; ++coluna)
            {
                arquivo >> tabuleiro[linha][coluna];
            }
        }
        arquivo.close();
    }
    else
    {
        std::cout << "Não foi possível abrir o arquivo: " << nome_arquivo << "\n";
    }
    return tabuleiro;
}

int main()
{
    std::setlocale(LC_ALL, "pt_BR.UTF-8");

    // Perguntar o nome do arquivo a ser carregado
    std::string nome_arquivo;
    std::cout << "Digite o nome do arquivo com o Sudoku a ser resolvido (exemplo: facil.txt): ";
    std::cin >> nome_arquivo;

    std::vector<std::vector<char>> tabuleiro = carregar_tabuleiro(nome_arquivo);

    std::cout << "Quebra-cabeça de Sudoku carregado:\n";
    imprimir_tabuleiro(tabuleiro);

    // Validação inicial
    std::string motivo = validar_tabuleiro(tabuleiro);
    if (!motivo.empty())
    {
        std::cout << "Jogo não tem solução por conta do motivo: " << motivo << std::endl;
        return 1;
    }

    // Iniciar cronômetro
    auto inicio = std::chrono::high_resolution_clock::now();

    // Resolver o Sudoku
    Solucao solucao;
    bool resolveu = solucao.resolverSudoku(tabuleiro);

    // Parar cronômetro
    auto fim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tempo = fim - inicio;

    if (resolveu)
    {
        std::cout << "Sudoku resolvido:\n";
        imprimir_tabuleiro(tabuleiro);
    }
    else
    {
        std::cout << "Não existe solução para o tabuleiro de Sudoku informado!\n";
    }

    std::cout << "Tempo gasto: " << tempo.count() << " ms" << std::endl;

    return 0;
}