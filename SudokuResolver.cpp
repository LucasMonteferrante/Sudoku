#include <vector>
#include <array>
#include <bitset>
#include <iostream>
#include <fstream>
#include <chrono>
#include <locale>
#include <iostream>

// Função para obter a região 3x3 correspondente a uma determinada linha e coluna
constexpr std::size_t obter_regiao(std::size_t linha, std::size_t coluna) noexcept
{
    return (linha / 3) * 3 + coluna / 3;
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
        if (!resolver(tabuleiro, 0, 0, linha_contem, coluna_contem, regiao_contem))
        {
            std::cout << "Não existe solução para o quebra-cabeça de Sudoku informado.\n";
        }
    }

private:
    // Função para encontrar a próxima posição vazia no tabuleiro
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

        // Tentar preencher a posição com cada dígito possível
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

                // Chamar a função recursiva para continuar resolvendo o tabuleiro
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

    // Função para obter a próxima linha a partir da posição atual
    static constexpr std::size_t proxima_linha(std::size_t linha, std::size_t coluna) noexcept
    {
        return linha + (coluna + 1) / 9;
    }

    // Função para obter a próxima coluna a partir da posição atual
    static constexpr std::size_t proxima_coluna(std::size_t coluna) noexcept
    {
        return (coluna + 1) % 9;
    }

    // Função para imprimir o tabuleiro do Sudoku
    static void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
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
};

// Função global para imprimir o tabuleiro do Sudoku
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

// Função para carregar o tabuleiro de um arquivo
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
    return tabuleiro;
}

int main()
{
    // Acentos especiais
    std::setlocale(LC_ALL, "pt_BR.UTF-8");

    // Carregar o tabuleiro do arquivo
    std::vector<std::vector<char>> tabuleiro = carregar_tabuleiro("sudoku_gerado.txt");

    std::cout << "Quebra-cabeça de Sudoku carregado:\n";
    imprimir_tabuleiro(tabuleiro);

    // Iniciar cronômetro
    auto inicio = std::chrono::high_resolution_clock::now();

    // Resolver o Sudoku
    Solucao solucao;
    solucao.resolverSudoku(tabuleiro);

    // Parar cronômetro
    auto fim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tempo = fim - inicio;

    std::cout << "Sudoku resolvido:\n";
    imprimir_tabuleiro(tabuleiro);

    // Mostrar o tempo gasto
    std::cout << "Tempo gasto: " << tempo.count() << " ms" << std::endl;

    return 0;
}