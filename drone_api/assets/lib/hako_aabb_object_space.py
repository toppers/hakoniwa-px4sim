from ihako_object_space import IHakoObjectSpace

class HakoAABBObjectSpace(IHakoObjectSpace):
    def __init__(self, position, size):
        self.position = position  # (x, y, z)
        self.size = size  # (width, height, depth)
        self.bounds = self.calculate_bounds()

    def calculate_bounds(self):
        half_size = (self.size[0] / 2, self.size[1] / 2, self.size[2] / 2)
        min_bound = (
            self.position[0] - half_size[0],
            self.position[1] - half_size[1],
            self.position[2] - half_size[2]
        )
        max_bound = (
            self.position[0] + half_size[0],
            self.position[1] + half_size[1],
            self.position[2] + half_size[2]
        )
        return {"min": min_bound, "max": max_bound}

    def get_position(self):
        return self.position

    def get_bounds(self):
        return self.bounds
