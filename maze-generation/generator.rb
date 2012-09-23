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
  end

  def generate
    loop do
      step
    end
  end

  def step
    raise StopIteration
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
end
