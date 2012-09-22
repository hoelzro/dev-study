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
  X_OFFSET    = 5
  Y_OFFSET    = 5
  CELL_WIDTH  = 20
  CELL_HEIGHT = 20

  attr_reader :x
  attr_reader :y

  def initialize(x, y)
    @x     = x
    @y     = y
    @edges = []
  end

  def render_to(renderer)
    if top_edge.closed?
      renderer.draw_line X_OFFSET + CELL_WIDTH  * @x, Y_OFFSET + CELL_HEIGHT * @y, CELL_WIDTH, 0
    end

    if left_edge.closed?
      renderer.draw_line X_OFFSET + CELL_WIDTH  * @x, Y_OFFSET + CELL_HEIGHT * @y, 0, CELL_HEIGHT
    end

    if bottom_edge.closed?
      renderer.draw_line X_OFFSET + CELL_WIDTH  * @x, Y_OFFSET + CELL_HEIGHT * (@y + 1), CELL_WIDTH, 0
    end

    if right_edge.closed?
      renderer.draw_line X_OFFSET + CELL_WIDTH * (@x + 1), Y_OFFSET + CELL_HEIGHT * @y, 0, CELL_HEIGHT
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
  def initialize(width, height)
    if width != height
      raise 'Non-square mazes are not yet supported'
    end

    @rows = build_rows width, height
    build_edges
  end

  def render_to(renderer)
    each_cell do |cell|
      cell.render_to renderer
    end
  end

  private

  def build_rows(width, height)
    (1 .. height).map do |y|
      (1 .. width).map do |x|
        Cell.new x - 1, y - 1
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
end