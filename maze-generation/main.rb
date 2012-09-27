$:.push '.'

require 'maze'
require 'generator'
require 'renderer'

def do_file_mode(width, height, filename)
  maze      = Maze.new width, height
  renderer  = Renderer.new filename
  generator = Generator.new maze
  generator.generate
  renderer.render maze
end

def do_graphics_mode(width, height)
  require 'gtk2'

  maze_generator = Class.new(Gtk::Window) do
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

  Gtk.init
  window = maze_generator.new width, height
  Gtk.main
end

if ARGV.length < 2
  puts "usage: #{$0} width height [filename.png]"
  exit 1
end

width  = ARGV[0].to_i
height = ARGV[1].to_i

if ARGV.length > 2
  do_file_mode width, height, ARGV[2]
else
  do_graphics_mode width, height
end
