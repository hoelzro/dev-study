require 'set'

class Generator
  def initialize(maze, rng = nil)
    if rng.nil?
      rng = Random.new
    elsif not rng.respond_to? :rand
      rng = Random.new rng
    end

    @maze = maze
    @rng  = rng

    create_endpoints
    @stepper = build_stepper
  end

  def generate
    loop do
      step
    end
  end

  def step
    @stepper.next
  end

  private

  def create_endpoints
    cells  = @maze.edge_cells
    cells  = cells.shuffle random: @rng
    start  = cells[0]
    finish = cells[1]

    @maze.start  = start
    @maze.finish = finish
  end

  def build_stepper
    nodes   = [ @maze.start ]
    visited = Set.new

    Enumerator.new do |y|
      while not nodes.empty?
        node = nodes.last

        visited.add node

        neighbors = @maze.find_neighbors node

        if neighbors.all? { |n| visited.include? n }
          nodes.pop
        else
          neighbors = neighbors.shuffle random: @rng
          @maze.open_edge node, neighbors.first
          nodes.push neighbors.first
        end

        y.yield
      end
    end
  end
end
