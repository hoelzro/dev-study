require 'cairo'

class Renderer
  X_OFFSET    = 5
  Y_OFFSET    = 5
  CELL_WIDTH  = 20
  CELL_HEIGHT = 20

  def initialize(canvas_or_filename)
    if canvas_or_filename.is_a? String
      @mode     = :file
      @filename = canvas_or_filename
    else
      @mode   = :gtk
      @canvas = canvas_or_filename
    end
  end

  def render(maze)
    init_drawing maze

    maze.each_cell do |cell|
      if cell.top_edge.closed?
        draw_line X_OFFSET + CELL_WIDTH  * cell.x, Y_OFFSET + CELL_HEIGHT * cell.y, CELL_WIDTH, 0
      end

      if cell.left_edge.closed?
        draw_line X_OFFSET + CELL_WIDTH  * cell.x, Y_OFFSET + CELL_HEIGHT * cell.y, 0, CELL_HEIGHT
      end

      if cell.bottom_edge.closed?
        draw_line X_OFFSET + CELL_WIDTH  * cell.x, Y_OFFSET + CELL_HEIGHT * (cell.y + 1), CELL_WIDTH, 0
      end

      if cell.right_edge.closed?
        draw_line X_OFFSET + CELL_WIDTH * (cell.x + 1), Y_OFFSET + CELL_HEIGHT * cell.y, 0, CELL_HEIGHT
      end
    end

    if @mode == :file
      @surface.write_to_png @filename
    end
  end

  def calculate_size(width, height)
    return [
      width  * CELL_WIDTH  + X_OFFSET * 2,
      height * CELL_HEIGHT + Y_OFFSET * 2,
    ]
  end

  private

  def draw_line(startx, starty, width, height)
    @ctx.new_path
    @ctx.move_to startx, starty
    @ctx.line_to startx + width, starty + height
    @ctx.stroke
  end

  def init_drawing(maze)
    if @mode == :gtk
      @ctx   = @canvas.window.create_cairo_context
      width  = @canvas.allocation.width
      height = @canvas.allocation.height
    else
      width, height = calculate_size maze.width, maze.height
      @surface = Cairo::ImageSurface.new width, height
      @ctx     = Cairo::Context.new @surface
    end

    @ctx.rectangle 0, 0, width, height
    @ctx.set_source_rgba 1, 1, 1, 1
    @ctx.fill

    @ctx.set_source_rgba 0, 0, 0, 1
    @ctx.set_line_width 1
  end
end
