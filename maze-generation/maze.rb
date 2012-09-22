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
    # XXX shared edges
    @edges = (1 .. 4).map { Edge.new }
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
  end
end

class Maze
  def initialize(width, height)
    if width != height
      raise 'Non-square mazes are not yet supported'
    end

    @rows = build_rows width, height
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

  def each_cell(&block)
    each_row do |row|
      row.each &block
    end
  end

  def each_row(&block)
    @rows.each &block
  end
end
