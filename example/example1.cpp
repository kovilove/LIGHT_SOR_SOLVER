/**
 * @file example1.cpp
 * @brief 示例代码
 * @note  10 × 10 稀疏矩阵求解
 */

#include <iostream>
#include "../LSS/light_sor_solver.hpp"

using std::cout;
using std::endl;

constexpr size_t num = 10;

int main()
{
    using namespace LSS;
    // mtx - 构造稀疏矩阵
    SparseMatrix<float> mtx;
    size_t row = 0;
    // x - 待求解量
    std::vector<float> x(num, {});
    // rhs - 右端项
    std::vector<float> rhs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 设置矩阵尺寸
    mtx.Reset(num);
    // AddMtx - 添加稠密矩阵
    mtx.AddMtx({
                   {5, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 5, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 5, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 5, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 5, 0, 0, 0, 0, 1},
                   {1, 5, 0, 0, 0, 5, 0, 0, 0, 1},
                   {1, 0, 5, 0, 0, 0, 5, 0, 0, 1},
                   {1, 0, 0, 5, 0, 0, 0, 5, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 5, 1},
                   {1, 5, 0, 0, 0, 0, 0, 0, 0, 5},
               },
               row);
    // SetRhs - 添加右端项
    mtx.SetRhs(rhs);
    // SOR_Solve -  SOR求解
    if (mtx.SOR_Solve(x, 1000, 0.5) != LSS_OK)
    {
        cout << "calculate err";
        return 1;
    }
    // 打印机求解量
    for (auto &ele : x)
    {
        cout << ele << "\n";
    }
    /* result:
        -0.2
        0.04
        0.24
        0.44
        0.64
        0.8
        0.8
        0.8
        1.44
        2
     */
    return 0;
}