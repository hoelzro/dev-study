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
    pairs   = []
    visited = Set.new

    neighbors = @maze.find_neighbors(@maze.start).shuffle random: @rng
    neighbors.each do |neighbor|
      pairs.push [ @maze.start, neighbor ]
    end

    visited.add @maze.start

    Enumerator.new do |y|
      while not pairs.empty?
        node, neighbor = pairs.pop

        next if visited.include? neighbor

        visited.add neighbor

        @maze.open_edge node, neighbor

        neighbors = @maze.find_neighbors(neighbor).shuffle random: @rng

        neighbors.each do |n|
          pairs.push [ neighbor, n ]
        end

        y.yield
      end
    end
  end
end
