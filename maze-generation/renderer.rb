require 'cairo'

class Renderer
  def initialize(width, height)
    @surface = Cairo::ImageSurface.new 'argb32', width, height
    @ctx     = Cairo::Context.new @surface

    @ctx.rectangle 0, 0, width, height
    @ctx.set_source_rgba 1, 1, 1, 1
    @ctx.fill

    @ctx.set_source_rgba 0, 0, 0, 1
    @ctx.set_line_width 1
  end

  def draw_line(startx, starty, width, height)
    @ctx.new_path
    @ctx.move_to startx, starty
    @ctx.line_to startx + width, starty + height
    @ctx.stroke
  end

  def write(filename)
    @surface.write_to_png filename
  end
end
