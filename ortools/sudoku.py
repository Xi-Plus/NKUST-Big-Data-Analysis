from __future__ import division
from __future__ import print_function
from ortools.sat.python import cp_model


class VarArraySolutionPrinter(cp_model.CpSolverSolutionCallback):
    """Print intermediate solutions."""

    def __init__(self, variables):
        cp_model.CpSolverSolutionCallback.__init__(self)
        self.__variables = variables
        self.__solution_count = 0

    def on_solution_callback(self):
        self.__solution_count += 1
        print('Solution %d' % self.__solution_count)
        for x in range(9):
            for y in range(9):
                print(self.Value(self.__variables[(x, y)]), end=' ')
            print()
        print()

    def solution_count(self):
        return self.__solution_count


def main(preload):
    model = cp_model.CpModel()

    board = {}
    for x in range(9):
        for y in range(9):
            board[(x, y)] = model.NewIntVar(1, 9, 'board%d%d' % (x, y))

    for x in range(9):
        model.AddAllDifferent([board[(x, y)] for y in range(9)])

    for y in range(9):
        model.AddAllDifferent([board[(x, y)] for x in range(9)])

    for x in range(3):
        for y in range(3):
            model.AddAllDifferent([board[(x * 3 + i, y * 3 + j)] for i in range(3) for j in range(3)])

    for x in range(9):
        for y in range(9):
            if preload[x][y] != 0:
                model.Add(board[(x, y)] == preload[x][y])

    solver = cp_model.CpSolver()
    solution_printer = VarArraySolutionPrinter(board)
    status = solver.SearchForAllSolutions(model, solution_printer)

    print('Status = %s' % solver.StatusName(status))
    print('Number of solutions found: %i' % solution_printer.solution_count())


if __name__ == '__main__':
    preload = [
        [0, 0, 0, 0, 0, 5, 0, 0, 0],
        [2, 0, 0, 9, 6, 0, 8, 1, 0],
        [0, 0, 0, 0, 0, 0, 3, 0, 4],
        [0, 0, 0, 0, 0, 7, 0, 0, 5],
        [8, 0, 4, 6, 0, 1, 0, 0, 0],
        [6, 0, 0, 0, 0, 0, 0, 0, 0],
        [9, 0, 0, 0, 1, 3, 0, 7, 2],
        [4, 0, 7, 0, 0, 8, 0, 9, 0],
        [0, 0, 1, 0, 0, 0, 0, 0, 0],
    ]
    main(preload)
