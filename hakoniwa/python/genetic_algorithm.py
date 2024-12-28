import sys
import random

# パラメータの範囲
Kp_range = (0, 5)
Ki_range = (0, 1)
Kd_range = (0, 10)

def read_pid_params(filename):
    params = []
    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
        for i in range(0, len(lines), 3):
            Kp = float(lines[i].split('=')[1])
            Ki = float(lines[i+1].split('=')[1])
            Kd = float(lines[i+2].split('=')[1])
            params.append({'Kp': Kp, 'Ki': Ki, 'Kd': Kd})
    except Exception as e:
        print(f"Error reading PID params: {e}", file=sys.stderr)
    return params

def read_fitnesses(filename):
    fitnesses = []
    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
        for line in lines:
            fitnesses.append(float(line.strip()))
    except Exception as e:
        print(f"Error reading fitness values: {e}", file=sys.stderr)
    return fitnesses

def generate_initial_population(size):
    population = []
    for _ in range(size):
        individual = {
            'Kp': random.uniform(*Kp_range),
            'Ki': random.uniform(*Ki_range),
            'Kd': random.uniform(*Kd_range)
        }
        population.append(individual)
    return population



def crossover(parent1, parent2):
    child = {
        'Kp': (parent1['Kp'] + parent2['Kp']) / 2,
        'Ki': (parent1['Ki'] + parent2['Ki']) / 2,
        'Kd': (parent1['Kd'] + parent2['Kd']) / 2
    }
    return child

def mutate(individual):
    mutation_rate = 0.1
    if random.random() < mutation_rate:
        individual['Kp'] += random.uniform(-0.1, 0.1)
        individual['Ki'] += random.uniform(-0.01, 0.01)
        individual['Kd'] += random.uniform(-0.1, 0.1)

    individual['Kp'] = max(min(individual['Kp'], Kp_range[1]), Kp_range[0])
    individual['Ki'] = max(min(individual['Ki'], Ki_range[1]), Ki_range[0])
    individual['Kd'] = max(min(individual['Kd'], Kd_range[1]), Kd_range[0])
    return individual


def select_parent_from_elites(elites):
    return random.choice(elites)


def adjust_pid_params(fitnesses, population):
    new_population = []
    elites = []
    non_elites = []
    
    # エリート選択: 最高のフィットネス値を持つ個体をそのまま次世代に追加
    elite_size = max(2, int(0.1 * len(population)))  # エリート個体の数を調整
    elite_indices = sorted(range(len(fitnesses)), key=lambda i: fitnesses[i], reverse=True)[:elite_size]
    
    print(f"Elite size: {elite_size}", file=sys.stderr)
    print(f"Elite indices: {elite_indices}", file=sys.stderr)
    for idx in elite_indices:
        print(f"Elite: {idx} : fitness={fitnesses[idx]}", file=sys.stderr)
        elites.append(population[idx])
        new_population.append(population[idx])

    # エリートでない個体をリスト管理
    non_elite_indices = [i for i in range(len(population)) if i not in elite_indices]
    for idx in non_elite_indices:
        non_elites.append((idx, population[idx]))
    
    # 残りの個体を交叉と突然変異で生成（非エリート + エリート）
    for idx, non_elite in non_elites:
        print(f"NonElite: {idx} : fitness={fitnesses[idx]}", file=sys.stderr)
        elite_parent = select_parent_from_elites(elites)
        child = crossover(non_elite, elite_parent)
        child = mutate(child)
        new_population.append(child)
    
    return new_population  # 人口サイズを維持

if __name__ == "__main__":
    if len(sys.argv) == 3 and sys.argv[1] == '--generate_initial_population':
        population_size = int(sys.argv[2])  # ここを修正
        initial_population = generate_initial_population(population_size)
        for individual in initial_population:
            print(individual['Kp'], individual['Ki'], individual['Kd'])
    elif len(sys.argv) == 3:
        fitness_file = sys.argv[1]
        pid_param_file = sys.argv[2]

        fitnesses = read_fitnesses(fitness_file)
        population = read_pid_params(pid_param_file)

        print("BEFORE", file=sys.stderr)
        idx=0
        for individual in population:
            print(idx, individual['Kp'], individual['Ki'], individual['Kd'], file=sys.stderr)
            idx=idx+1
        new_population = adjust_pid_params(fitnesses, population)
        
        print("AFTER", file=sys.stderr)
        idx=0
        for individual in new_population:
            print(idx, individual['Kp'], individual['Ki'], individual['Kd'], file=sys.stderr)
            idx=idx+1
        for individual in new_population:
            print(individual['Kp'], individual['Ki'], individual['Kd'])
   
    else:
        print("Usage: python genetic_algorithm.py <fitness_values_file> <pid_param_file>")
        print("       python genetic_algorithm.py --generate_initial_population <population_size>")
        sys.exit(1)
