class Edge
  def initialize
    @closed = true
  end

  def open?
    not closed?
  end

  def closed?
    @closed
  end

  def open
    @closed = false
  end
end

class Cell
  attr_reader :x
  attr_reader :y

  def initialize(maze, x, y)
    @maze  = maze
    @x     = x
    @y     = y
    @edges = []
  end

  def open_outer_edge
    # XXX should the RNG be used to select which edge for corners?
    if @maze.top_row.include? self
      top_edge.open
      return
    end

    if @maze.bottom_row.include? self
      bottom_edge.open
      return
    end

    if @maze.left_column.include? self
      left_edge.open
      return
    end

    if @maze.right_column.include? self
      right_edge.open
      return
    end
  end

  %w(top bottom left right).each_with_index do |name, index|
    define_method (name + '_edge').to_sym do
      @edges[index]
    end

    define_method (name + '_edge=').to_sym do |value|
      @edges[index] = value
    end
  end
end

class Maze
  attr_accessor :start
  attr_accessor :finish

  def initialize(width, height)
    if width != height
      raise 'Non-square mazes are not yet supported'
    end

    @rows = build_rows width, height
    build_edges
  end

  def each_cell(&block)
    each_row do |row|
      row.each &block
    end
  end

  def each_row(&block)
    @rows.each &block
  end

  def each_column
    first_row = @rows.first

    (0 .. first_row.length - 1).each do |index|
      column = @rows.map { |row| row[index] }
      yield column
    end
  end

  def edge_cells
    [ top_row, bottom_row, left_column, right_column ].flatten.uniq
  end

  def start=(cell)
    @start = cell
    cell.open_outer_edge
  end

  def finish=(cell)
    @finish = cell
    cell.open_outer_edge
  end

  def top_row
    @rows[0]
  end

  def bottom_row
    @rows[-1]
  end

  def left_column
    columns[0]
  end

  def right_column
    columns[-1]
  end

  def find_neighbors(cell)
    x         = cell.x
    y         = cell.y
    neighbors = []

    if x != 0
      neighbors.push @rows[y][x - 1]
    end

    if y != 0
      neighbors.push @rows[y - 1][x]
    end

    if x != @rows.length - 1
      neighbors.push @rows[y][x + 1]
    end

    if y != columns.length - 1 # XXX inefficient
      neighbors.push @rows[y + 1][x]
    end

    neighbors
  end

  def open_edge(cell1, cell2)
    if cell1.x < cell2.x
      cell1.right_edge.open
    elsif cell1.x > cell2.x
      cell1.left_edge.open
    elsif cell1.y < cell2.y
      cell1.bottom_edge.open
    else
      cell1.top_edge.open
    end
  end

  def width
    @rows[0].size
  end

  def height
    @rows.size
  end

  private

  def columns
    columns = []

    each_column do |column|
      columns.push column
    end

    columns
  end

  def build_rows(width, height)
    (1 .. height).map do |y|
      (1 .. width).map do |x|
        Cell.new self, x - 1, y - 1
      end
    end
  end

  def build_edges
    each_row do |row|
      row.inject do |left, right|
        left.right_edge = right.left_edge = Edge.new
        right
      end
    end

    index             = 0
    last_column_index = @rows.first.length - 1

    each_column do |column|
      column.inject do |top, bottom|
        top.bottom_edge = bottom.top_edge = Edge.new
        bottom
      end

      if index == 0
        column.each do |cell|
          cell.left_edge = Edge.new
        end
      elsif index == last_column_index
        column.each do |cell|
          cell.right_edge = Edge.new
        end
      end

      index += 1
    end

    @rows.first.each do |cell|
      cell.top_edge = Edge.new
    end

    @rows.last.each do |cell|
      cell.bottom_edge = Edge.new
    end
  end
end
