# TheGameOfWar
**Game Description**

TheGameOfWar is a strategic game played on a 16x16 grid. Each cell can be either dead or belong to one of two players (blue or green). Players take turns making moves, where they can change a dead cell to their color, eliminate one of their own cells, or press the spacebar.

When a player presses the spacebar, the entire grid changes according to simple rules. A cell comes to life if it has exactly three living neighbors, survives if it has two or three neighbors, and dies if it has fewer than two or more than three living neighbors. The neighborhood is defined as the 3x3 area surrounding the cell in question. Importantly, each opponent's cell within this area counts as -1, reducing the number of neighbors for your cell. If the neighbor count becomes negative, the cell immediately changes to the opponent's color.

The game ends when all living cells on the grid belong to one player, and that player is declared the winner. The objective of the game is strategic planning and tactical maneuvering to gain control of the grid and defeat the opponent.
