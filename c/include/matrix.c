#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Matrix {
    double *values;
    int rows;
    int cols;
    int *independent_cols;
    int independent_cols_len;
} Matrix;

Matrix matrix_new(int rows, int cols)
{
    return (Matrix){
        .rows   = rows,
        .cols   = cols,
        .values = calloc((size_t)(rows * cols), sizeof(double)),
    };
}

void matrix_free(Matrix *m)
{
    free(m->values);
    free(m->independent_cols);
    m->values = NULL;
    m->rows = 0;
    m->cols = 0;
}

double matrix_get(Matrix m, int row, int col)
{
    return m.values[row * m.cols + col];
}

void matrix_set(Matrix *m, int row, int col, double value)
{
    m->values[row * m->cols + col] = value;
}

void matrix_swap_rows(Matrix *m, int row1, int row2)
{
    for (int col = 0; col < m->cols; col++) {
        double temp = matrix_get(*m, row1, col);
        matrix_set(m, row1, col, matrix_get(*m, row2, col));
        matrix_set(m, row2, col, temp);
    }
}

void matrix_add_scaled_row(Matrix *m, int mut_row, int other_row, double scalar)
{
    for (int col = 0; col < m->cols; col++) {
        double value = matrix_get(*m, mut_row, col) + scalar * matrix_get(*m, other_row, col);
        matrix_set(m, mut_row, col, value);
    }
}

int matrix_max_abs_in_column(Matrix m, int col, int start_row)
{
    int max_row = start_row;
    double max_value = fabs(matrix_get(m, start_row, col));
    for (int row = start_row + 1; row < m.rows; row++) {
        double value = fabs(matrix_get(m, row, col));
        if (value > max_value) {
            max_value = value;
            max_row = row;
        }
    }
    return max_row;
}

void gaussian_elimination(Matrix *m)
{
    // Pseudo-code from Wikipedia:
    //
    // h := 1 /* Initialization of the pivot row */
    // k := 1 /* Initialization of the pivot column */
    //
    // while h ≤ m and k ≤ n:
    //     /* Find the k-th pivot: */
    //     i_max := argmax (i = h ... m, abs(A[i, k]))
    //     if A[i_max, k] = 0:
    //         /* No pivot in this column, pass to next column */
    //         k := k + 1
    //     else:
    //         swap rows(h, i_max)
    //         /* Do for all rows below pivot: */
    //         for i = h + 1 ... m:
    //             f := A[i, k] / A[h, k]
    //             /* Fill with zeros the lower part of pivot column: */
    //             A[i, k] := 0
    //             /* Do for all remaining elements in current row: */
    //             for j = k + 1 ... n:
    //                 A[i, j] := A[i, j] - A[h, j] * f
    //         /* Increase pivot row and column */
    //         h := h + 1
    //         k := k + 1

    int h = 0;
    int k = 0;
    m->independent_cols = calloc((size_t)m->cols, sizeof(int));
    m->independent_cols_len = 0;

    while (h < m->rows && k < m->cols) {
        int i_max = matrix_max_abs_in_column(*m, k, h);
        if (fabs(matrix_get(*m, i_max, k)) < 1e-6) {
            // No pivot in this column, pass to next column
            // fprintf(stderr, "No pivot found at row %d, column %d (max at row %d)\n", h, k, i_max);
            if (k < m->cols - 1) {
                m->independent_cols[m->independent_cols_len] = k;
                m->independent_cols_len++;
            }
            k++;
        } else {
            // fprintf(stderr, "Pivot found at row %d, column %d (max at row %d)\n", h, k, i_max);
            matrix_swap_rows(m, h, i_max);
            // Do for all rows below pivot
            for (int i = h + 1; i < m->rows; i++) {
                double f = matrix_get(*m, i, k) / matrix_get(*m, h, k);
                // Fill with zeros the lower part of pivot column:
                matrix_set(m, i, k, 0.0f);
                // Do for all remaining elements in current row:
                for (int j = k + 1; j < m->cols; j++) {
                    matrix_set(m, i, j, matrix_get(*m, i, j) - matrix_get(*m, h, j) * f);
                }
            }
            // Increase pivot row and column
            h++;
            k++;
        }
    }

    // add remaining independent variables
    while (k < m->cols - 1) {
        m->independent_cols[m->independent_cols_len] = k;
        m->independent_cols_len++;
        k++;
    }
}

void back_substitution(Matrix m, double *solutions)
{
    int starting_row = m.rows - 1;
    while (starting_row >= 0 && fabs(matrix_get(m, starting_row, starting_row)) < 1e-6) {
        starting_row--;
    }

    solutions[starting_row] = matrix_get(m, starting_row, m.cols - 1) / matrix_get(m, starting_row, m.cols - 2);
    for (int i = starting_row; i >= 0; i--) {
        double sum = 0.0f;
        for (int j = i + 1; j < m.cols - 1; j++) {
            sum += matrix_get(m, i, j) * solutions[j];
        }
        solutions[i] = (matrix_get(m, i, m.cols - 1) - sum) / matrix_get(m, i, i);
    }
}

Matrix copy_set_variable(Matrix m, int col, double value)
{
    Matrix copy = matrix_new(m.rows + 1, m.cols);
    memcpy(copy.values, m.values, (size_t)(m.rows * m.cols) * sizeof(double));
    matrix_set(&copy, m.rows, col, 1.0f);
    matrix_set(&copy, m.rows, m.cols - 1, value);
    return copy;
}

Matrix matrix_extend_rows(Matrix m, int add_rows)
{
    Matrix copy = matrix_new(m.rows + add_rows, m.cols);
    memcpy(copy.values, m.values, (size_t)(m.rows * m.cols) * sizeof(double));
    return copy;
}

void matrix_copy_into(Matrix src, Matrix dest)
{
    assert(src.rows == dest.rows);
    assert(src.cols == dest.cols);
    memcpy(dest.values, src.values, (size_t)(src.rows * src.cols) * sizeof(double));
}

Matrix matrix_clone(Matrix src)
{
    Matrix dest = matrix_new(src.rows, src.cols);
    matrix_copy_into(src, dest);
    return dest;
}

void matrix_print(Matrix m)
{
    for (int row = 0; row < m.rows; row++) {
        for (int col = 0; col < m.cols; col++) {
            fprintf(stderr, "%5.1f ", matrix_get(m, row, col));
        }
        fprintf(stderr, "\n");
    }
    if (m.independent_cols_len > 0) {
        int independent_col_idx = 0;
        for (int i = 0; i < m.cols; i++) {
            if (m.independent_cols[independent_col_idx] == i) {
                fprintf(stderr, "    I ");
                independent_col_idx++;
            } else {
                fprintf(stderr, "      ");
            }
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "Independent columns: ");
        for (int i = 0; i < m.independent_cols_len; i++) {
            fprintf(stderr, "%d ", m.independent_cols[i]);
        }
        fprintf(stderr, "\n");
    }
}
