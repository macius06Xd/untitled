#include <dim/dimension3D.hpp>
#include <set>
#include <functional>
#include <queue>
#include <chrono>
namespace std {
	template<>
	struct hash<dim::Vector3>
	{
		
		size_t operator()(const dim::Vector3& v) const
		{
			return ((size_t)v.x ^ (size_t)v.y) ^ (size_t)v.z;
		}
	};

	template<>
	struct equal_to<dim::Vector3>
	{
		bool operator()(const dim::Vector3& v1, const dim::Vector3& v2) const
		{
			return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
		}
	};

		
}
std::vector<dim::Vector3> generate_pyramid_vertices(dim::Vector3 start, int size, float square_size)
{
	
	std::vector<dim::Vector3> Points;
	int level_size = size;
	dim::Vector3 point = start;
	while (level_size - 1)
	{
		auto starting_point = point;
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ 0,square_size,0 };
			Points.push_back(point);
		}
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ square_size,0,0 };
			Points.push_back(point);
		}
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ 0,-square_size,0 };
			Points.push_back(point);
		}
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ -square_size,0,0 };
			Points.push_back(point);
		}
		point = starting_point + dim::Vector3(square_size / 2, square_size / 2 , square_size);
		level_size--;
	}
	Points.push_back(point);
	return Points;


}

std::string vectortotext(std::vector<dim::Vector3> wektors)
{
	std::string returnvalue;
	for (auto wektor : wektors)
	{
		returnvalue += "" + std::to_string(wektor.x) + " " + std::to_string(wektor.y) + " " + std::to_string(wektor.z) + "\n";
	}
	return returnvalue;
}

bool collides(dim::Object* robot, dim::Vector3 newPosition, dim::Object* object)
{
	const float buffer = -0.1f; // Buffer zone around each object
	dim::Vector3 robotSize = robot->get_size();
	dim::Vector3 objectSize = object->get_size() + dim::Vector3(buffer, buffer, buffer); // Expand the object's size
	dim::Vector3 robotPositionCenter = newPosition;
	dim::Vector3 objectPositionCenter = object->get_position();
	// Calculate the minimum and maximum extents of both the cubes along each axis
	dim::Vector3 robotMin = robotPositionCenter - (robotSize / 2.0f);
	dim::Vector3 robotMax = robotPositionCenter + (robotSize / 2.0f);
	dim::Vector3 objectMin = objectPositionCenter - (objectSize / 2.0f);
	dim::Vector3 objectMax = objectPositionCenter + (objectSize / 2.0f);

	// Check if the extents overlap along each axis
	bool xOverlap = robotMin.x <= objectMax.x && robotMax.x >= objectMin.x;
	bool yOverlap = robotMin.y <= objectMax.y && robotMax.y >= objectMin.y;
	bool zOverlap = robotMin.z <= objectMax.z && robotMax.z >= objectMin.z;

	// If the extents overlap along all axes, the cubes intersect
	return xOverlap && yOverlap && zOverlap;
}
std::vector<dim::Vector3> possibleMoves(dim::Object *robot,dim::Vector3 position, std::vector<dim::Object*> gameObjects)
{

	std::vector<dim::Vector3> possibleMoves = {
		dim::Vector3(1, 0, 0),
		dim::Vector3(-1, 0, 0),
		dim::Vector3(0, 1, 0),
		dim::Vector3(0, -1, 0)
	};

	for (auto move : possibleMoves)
	{
		dim::Vector3 newRobotPosition = position + move;

		bool collidesWithObject = false;
		for (auto object : gameObjects)
		{
			if (object != robot)
			{
				if (collides(robot, newRobotPosition, object))
				{
					//collidesWithObject = true;
					break;
				}
			}
		}

		if (!collidesWithObject)
		{
			possibleMoves.push_back(dim::Vector3(0, 0, 1)); // Add new move
			break;
		}
	}
	return possibleMoves;
}
bool is_collision(dim::Vector3 pos, dim::Object* robot, std::vector<dim::Object*> objects) {


// Check for collision with other objects
for (dim::Object* obj : objects) {
	if (obj != robot)
	{
		if (collides(robot, pos, obj)) {
			return true;
		}
	}
}

// No collision
return false;
}

#include <queue>
#include <unordered_map>
#include <vector>
#include <functional>
#include <cmath>

struct Node {
	dim::Vector3 pos;
	double g, h;
	Node* parent;

	Node(dim::Vector3 pos, double g, double h, Node* parent = nullptr) : pos(pos), g(g), h(h), parent(parent) {}

	double f() const {
		return g + h;
	}
};

struct NodeHash {
	std::size_t operator()(const Node* node) const {
		return std::hash<int>()(std::round(node->pos.x)) ^
			std::hash<int>()(std::round(node->pos.y)) ^
			std::hash<int>()(std::round(node->pos.z));
	}
};

struct NodeEqual {
	bool operator()(const Node* lhs, const Node* rhs) const {
		return lhs->pos == rhs->pos;
	}
};

std::vector<dim::Vector3> findpath(dim::Vector3 start, dim::Vector3 goal, dim::Object* robot, std::vector<dim::Object*> objects) {
	// Define a lambda function to calculate the heuristic distance between two points
	auto heuristic = [](dim::Vector3 a, dim::Vector3 b) {
		return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
	};

	// Initialize the open and closed sets
	std::priority_queue<Node*, std::vector<Node*>, std::function<bool(Node*, Node*)>> open(
		[](Node* a, Node* b) {
			return a->f() > b->f();
		});
	std::unordered_map<Node*, Node*, NodeHash, NodeEqual> closed;

	// Create the start node and add it to the open set
	Node* start_node = new Node(start, 0, heuristic(start, goal));
	open.push(start_node);

	while (!open.empty()) {
		// Get the node with the lowest f score from the open set
		Node* current_node = open.top();
		open.pop();

		// Check if we have reached the goal
		if (current_node->pos == goal) {
			// Build the path by following the parent pointers
			std::vector<dim::Vector3> path;
			while (current_node) {
				path.push_back(current_node->pos);
				current_node = current_node->parent;
			}
			std::reverse(path.begin(), path.end());

			closed.clear();
			delete start_node;

			return path;
		}

		// Generate the successors of the current node
		const double move_dist = 1.0;
		std::vector<dim::Vector3> moves = {
			dim::Vector3(move_dist, 0, 0),
			dim::Vector3(-move_dist, 0, 0),
			dim::Vector3(0, move_dist, 0),
			dim::Vector3(0, -move_dist, 0),
		};
		std::vector<dim::Vector3> Amoves;

		auto was_Collision = false;
		for (auto& move : moves)
		{
			if (is_collision(current_node->pos + move, robot, objects))
			{
				was_Collision = true;
			}
			else if (is_collision(current_node->pos + move + dim::Vector3(0,0,-1), robot, objects) || current_node->pos.z < 0.1)
			{
				Amoves.push_back(move);
				}
			}
		if (was_Collision)
		{
			Amoves.push_back(dim::Vector3(0, 0, 1));
		}

		
		for (auto& move : Amoves) {
			dim::Vector3 next_pos = current_node->pos + move;

			// Check if the next position is valid (i.e., within bounds and not colliding with any object)
			if (next_pos.x >= -40 && next_pos.x <= 40 &&
				next_pos.y >= -40 && next_pos.y <= 40 &&
				next_pos.z >= 0 && next_pos.z <= 40 )
				 {

				// Calculate the cost of moving to the next position
				double cost = current_node->g + heuristic(current_node->pos, next_pos);

				// Check if we have already visited the next position
				auto next_node_it = closed.find(new Node(next_pos, 0, 0));
				if (next_node_it != closed.end()) {
					Node* next_node = next_node_it->first;
					if (cost < next_node->g) {
						// Update the next node's parent and g score
						next_node->parent = current_node;
						next_node->g = cost;
					}
				}
				else {
					// Create a new node for the next position and add it to the open set
					Node* next_node = new Node(next_pos, cost, heuristic(next_pos, goal), current_node);
					open.push(next_node);
					closed[next_node] = next_node;
				}
			}
		}

		// Add the current node to the closed set
		closed[current_node] = current_node;
	}

	// Clean up memory allocated for the nodes
	for (auto& pair : closed) {
		delete pair.first;
		delete pair.second;
	}
	closed.clear();
	delete start_node;

	// Return an empty path if no path was found
	return std::vector<dim::Vector3>();
}



int main()
{
	// Open the main window and initialize the libraries
	dim::Window::open("App", 0.75f);

	// Create the scenes
	dim::Scene scene("Scene");

	// Cameras and controllers
	dim::PerspectiveCamera cam;
	cam.set_position(dim::Vector3(30.f, -30.f, 0.f));
	scene.set_camera(cam);

	scene.set_controller(dim::OrbitController());

	// Add lights
	scene.add_light(dim::DirectionalLight(dim::Vector3(-1.f, -1.f, -1.f)));

	// Create objects

	std::vector<dim::Object*> pyramid_objects;
	
	auto pyramid_points = generate_pyramid_vertices(dim::Vector3{ 0,0,0 }, 5, 4);
	// The example button color
	float color[4] = { 1.f, 1.f, 1.f, 1.f };
	double olddelta = 0;
	double delta = 0;
	double begin_time = clock();
	dim::Object* robot = new dim::Object(dim::Mesh::Cube(), dim::Material(dim::Color(0, 0, 1), 0.1f, 0.5f, 0.6f, 30.f));
	robot->set_size(dim::Vector3(1, 1, 4));
	pyramid_objects.push_back(robot);
	pyramid_objects.back()->set_position(dim::Vector3(0,-10,0));
	int p = pyramid_objects.size();
	int k = pyramid_objects.size();// Main loop
	int d = 0;
	double delta_acum = 0;
	dim::Object* object_1 = new dim::Object(dim::Mesh::Cube(), dim::Material(dim::Color(0, 0, 1), 0.1f, 0.5f, 0.6f, 30.f));
	object_1->set_size(dim::Vector3(4, 4, 4));
	pyramid_objects.push_back(object_1);
	pyramid_objects.back()->set_position(pyramid_points[d]);
	auto* cube = pyramid_objects[k];
	auto target_position = pyramid_points[d];
	int c = 1;
	
	while (d < pyramid_points.size())
	{

		dim::Object* object_1 = new dim::Object(dim::Mesh::Cube(), dim::Material(dim::Color(0, 0, 1), 0.1f, 0.5f, 0.6f, 30.f));
		object_1->set_size(dim::Vector3(4, 4, 4));
		pyramid_objects.push_back(object_1);
		pyramid_objects.back()->set_position(pyramid_points[d]);
		d++;
		k++;
	}
	auto start_time = std::chrono::high_resolution_clock::now();

	// Find the path
	std::vector<dim::Vector3> path;

	// Stop the timer
	auto end_time = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	// Print the elapsed time in milliseconds
	std::cout << "Pathfinding took " << elapsed_time.count() << " milliseconds.\n";
	int cos = 0;

 start_time = std::chrono::high_resolution_clock::now();

		 path = findpath(robot->get_position(), pyramid_points[pyramid_points.size()-1] + dim::Vector3(0,0,4), robot, pyramid_objects);


	while (dim::Window::running)
	{
		double now = clock();
		delta = now - begin_time;
		begin_time = now;
		delta = delta / CLOCKS_PER_SEC;
		delta_acum += delta;
		// Check events
		sf::Event sf_event;
		while (dim::Window::poll_event(sf_event))
		{
			dim::Window::check_events(sf_event);
			scene.check_events(sf_event);
		}
		
		if (delta_acum > 0.1f)
		{
				delta_acum -= 0.1f;
				if (cos < path.size())
				{
					robot->move(path[cos]-robot->get_position());
					cos++;
				}

		}
	
		// Clear the screen
		dim::Window::clear();
		scene.clear();
		// Update interactive stuff
		dim::Window::update();
		scene.update();
		// Draw the objects
		int i = 0;
		for (auto& object : pyramid_objects)
		{
			scene.draw(*object);
		}

		// Display the scenes to the window
		scene.display();

		// Display the window to the screen
		dim::Window::display();
	}

	// Close the main window and shut down the libraries
	dim::Window::close();
	return EXIT_SUCCESS;
}
