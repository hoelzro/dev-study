$:.push '.'

require 'maze'
require 'generator'
require 'renderer'
require 'gtk2'

class MazeGenerator < Gtk::Window
  def initialize(width, height)
    super()

    set_title 'Maze Generator'

    signal_connect 'destroy' do
      Gtk.main_quit
    end

    @canvas   = Gtk::DrawingArea.new
    @maze     = Maze.new width, height
    @renderer = Renderer.new @canvas
    generator = Generator.new @maze
    generator.generate

    window_width, window_height = @renderer.calculate_size width, height

    set_default_size window_width, window_height

    add @canvas

    @canvas.signal_connect 'expose-event' do
      draw
    end

    show_all
  end

  private

  def draw
    @renderer.render @maze
  end
end

if ARGV.length < 2
  puts "usage: #{$0} [width] [height]"
  exit 1
end

width  = ARGV[0].to_i
height = ARGV[0].to_i

Gtk.init
window = MazeGenerator.new width, height
Gtk.main
