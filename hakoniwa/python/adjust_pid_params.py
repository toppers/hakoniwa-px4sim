import sys
import random

# パラメータの範囲
Kp_range = (0, 10)
Ki_range = (0, 2)
Kd_range = (0, 10)

# 初期集団サイズ
population_size = 10

def read_evaluation_results(filename):
    results = {
        'steady_state': 0.0,
        'steady_state_ok': False,
        'rise_time': 0.0,
        'rise_time_ok': False,
        'delay_time': 0.0,
        'delay_time_ok': False,
        'overshoot': 0.0,
        'overshoot_ok': False,
        'settling_time': 0.0,
        'settling_time_ok': False
    }

    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
        for line in lines:
            if 'Steady state value' in line:
                results['steady_state'] = float(line.split(':')[1].split(' ')[1])
                results['steady_state_ok'] = 'OK' in line
            elif 'Rise time' in line:
                results['rise_time'] = float(line.split(':')[1].split(' ')[1])
                results['rise_time_ok'] = 'OK' in line
            elif 'Delay time' in line:
                results['delay_time'] = float(line.split(':')[1].split(' ')[1])
                results['delay_time_ok'] = 'OK' in line
            elif 'Maximum overshoot' in line:
                results['overshoot'] = float(line.split(':')[1].split(' ')[1])
                results['overshoot_ok'] = 'OK' in line
            elif 'settling time' in line:
                results['settling_time'] = float(line.split(':')[1].split(' ')[1])
                results['settling_time_ok'] = 'OK' in line
    except Exception as e:
        print(f"Error reading evaluation results: {e}")
    
    return results

def read_pid_params(filename):
    params = {
        'Kp': 0.0,
        'Ki': 0.0,
        'Kd': 0.0
    }

    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
        for line in lines:
            if 'DroneAvator_Kp' in line:
                params['Kp'] = float(line.split('=')[1])
            elif 'DroneAvator_Ki' in line:
                params['Ki'] = float(line.split('=')[1])
            elif 'DroneAvator_Kd' in line:
                params['Kd'] = float(line.split('=')[1])
    except Exception as e:
        print(f"Error reading PID params: {e}")

    return params

def fitness(results):
    # フィットネス関数を定義して定量化
    fitness_value = 0.0
    if results['steady_state_ok']:
        fitness_value += 1.0
    else:
        # ステディステートの誤差に基づいてスコアを減算
        target = 10.0
        tolerance = 0.1
        error = abs(results['steady_state'] - target)
        fitness_value -= min(error / tolerance, 1.0)

    if results['rise_time_ok']:
        fitness_value += 1.0
    else:
        # ライズタイムのスコア減算（小さい方が良い）
        target = 10.0
        tolerance = 10.0
        error = max(0, target - results['rise_time'])  # 小さい方が良いのでtargetから引く
        fitness_value += min(error / tolerance, 1.0)  # プラスに加算

    if results['delay_time_ok']:
        fitness_value += 1.0
    else:
        # ディレイタイムのスコア減算（小さい方が良い）
        target = 5.0
        tolerance = 5.0
        error = max(0, target - results['delay_time'])  # 小さい方が良いのでtargetから引く
        fitness_value += min(error / tolerance, 1.0)  # プラスに加算

    if results['overshoot_ok']:
        fitness_value += 1.0
    else:
        # オーバーシュートのスコア減算
        target = 1.0
        tolerance = 1.0
        error = results['overshoot'] - target
        fitness_value -= min(error / tolerance, 1.0)

    if results['settling_time_ok']:
        fitness_value += 1.0
    else:
        # セトリングタイムのスコア減算（小さい方が良い）
        target = 20.0
        tolerance = 20.0
        error = max(0, target - results['settling_time'])  # 小さい方が良いのでtargetから引く
        fitness_value += min(error / tolerance, 1.0)  # プラスに加算

    return fitness_value

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

def select_parents(population, fitnesses):
    total_fitness = sum(fitnesses)
    if total_fitness == 0:
        # フィットネス値がすべてゼロの場合、ランダムに親を選択
        parents = random.sample(population, 2)
    else:
        parents = random.choices(population, weights=fitnesses, k=2)
    return parents

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
        individual['Ki'] += random.uniform(-0.05, 0.05)
        individual['Kd'] += random.uniform(-0.1, 0.1)

    individual['Kp'] = max(min(individual['Kp'], Kp_range[1]), Kp_range[0])
    individual['Ki'] = max(min(individual['Ki'], Ki_range[1]), Ki_range[0])
    individual['Kd'] = max(min(individual['Kd'], Kd_range[1]), Kd_range[0])
    return individual

def adjust_pid_params(results, params, population):
    fitnesses = [fitness(results) for _ in population]
    new_population = []
    for _ in range(len(population)):
        parent1, parent2 = select_parents(population, fitnesses)
        child = crossover(parent1, parent2)
        child = mutate(child)
        new_population.append(child)
    return new_population

if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1] == '--generate_initial_population':
        initial_population = generate_initial_population(population_size)
        for individual in initial_population:
            print(individual['Kp'], individual['Ki'], individual['Kd'])
    elif len(sys.argv) == 3:
        evaluation_file = sys.argv[1]
        pid_param_file = sys.argv[2]

        results = read_evaluation_results(evaluation_file)
        params = read_pid_params(pid_param_file)
        initial_population = generate_initial_population(population_size)
        new_population = adjust_pid_params(results, params, initial_population)
        
        for individual in new_population:
            print(individual['Kp'], individual['Ki'], individual['Kd'])
    else:
        print("Usage: python adjust_pid_params.py <evaluation_result_file> <pid_param_file>")
        print("       python adjust_pid_params.py --generate_initial_population")
        sys.exit(1)
